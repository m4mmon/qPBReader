/* -------------------------------------------------------------------------- *\
|    This file is part of qPBReader                                            |
|    Copyright (C) 2015  m4mmon                                                |
|                                                                              |
|    This program is free software: you can redistribute it and/or modify      |
|    it under the terms of the GNU General Public License as published by      |
|    the Free Software Foundation, either version 3 of the License, or         |
|    (at your option) any later version.                                       |
|    This program is distributed in the hope that it will be useful,           |
|    but WITHOUT ANY WARRANTY; without even the implied warranty of            |
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
|    GNU General Public License for more details.                              |
|                                                                              |
|    You should have received a copy of the GNU General Public License         |
|    along with this program.  If not, see <http://www.gnu.org/licenses/>.     |
\* -------------------------------------------------------------------------- */

#include "qPBReaderPageCounter.h"

// qt
#include <QStringList>
#include <QTimer>
#include <QWebFrame>

// project
#include "qPBReaderDoc.h"
#include "qPBReaderEpub.h"
#include "qPBReaderPlatform.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderPageCounter::qPBReaderPageCounter() :
   _pBook(0), _pDoc(0), _currSection(-1), _nbSections(0), _bInterrupt(false)
{
   TRSCOPE(pages, "qPBReaderPageCounter::qPBReaderPageCounter");

}

// -----------------------------------------------------------------------------

qPBReaderPageCounter::qPBReaderPageCounter(qPBReaderEpub * ipBook,
      const QSize & iSz) :
   _pBook(ipBook), _Sz(iSz), _pDoc(0), _currSection(-1), _bInterrupt(false)
{
   TRSCOPE(pages, "qPBReaderPageCounter::qPBReaderPageCounter");
}

// -----------------------------------------------------------------------------

qPBReaderPageCounter::~qPBReaderPageCounter()
{
   TRSCOPE(pages, "qPBReaderPageCounter::~qPBReaderPageCounter");

   if (_pDoc)
   {
      delete _pDoc;
      _pDoc = 0;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderPageCounter::Interrupt()
{
   TRSCOPE(pages, "qPBReaderPageCounter::Interrupt");
   _bInterrupt = true;
   qPBReaderPlatform::DisplayPanelMessage(tr("Interrupting count..."));
}

// -----------------------------------------------------------------------------

void qPBReaderPageCounter::CountPages()
{
   TRSCOPE(pages, "qPBReaderPageCounter::CountPages");

   if (_pBook)
   {
      _nbSections = _pBook->GetNbSections();

      // first section

      _currSection = 0;

      // and load it

      GoLoad();
   }
}

// -----------------------------------------------------------------------------

void qPBReaderPageCounter::LoadFinished(bool bOk)
{
   TRSCOPE(pages, "qPBReaderPageCounter::LoadFinished");

   TRACE << "load ok=" << bOk << endl;

   // install everything

   _pDoc->LoadJSLibraries();
   _pDoc->ProcessFonts();
   _pDoc->AfterLoad();

   // go at then end of the section

   _pDoc->SetScrollFraction(1.);

   // and get current page

   int pos = _pDoc->GetCurrentPage();
   TRACE << "         current page=" << pos << endl;

   // store result

   _lnPages.append(pos);

   // and go to next section

   _currSection++;

   if (_currSection < _nbSections)
   {
      QTimer::singleShot(300, this, SLOT(GoLoad()));
   }

   // or dispatch results

   else
   {
      emit PagesCounted(_lnPages);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderPageCounter::GoLoad()
{
   TRSCOPE(pages, "qPBReaderPageCounter::GoLoad");

   // stop here if required

   if (_bInterrupt)
   {
      _lnPages.clear();
      emit PagesCounted(_lnPages);
   }

   // or load next section to count its pages

   else
   {
      // display some status

      QString sMsg(tr("Section "));
      sMsg += QString::number(_currSection + 1);
      sMsg += " / ";
      sMsg += QString::number(_nbSections);
      qPBReaderPlatform::DisplayPanelMessage(sMsg);

      if (_pDoc)
      {
         delete _pDoc;
         _pDoc = 0;
      }

      // create new page

      if (!_pDoc)
      {
         _pDoc = new qPBReaderDoc;

         // set it up as what is displayed

         _pDoc->SetHyphenPatternLang(_pBook->GetHyphenPatternLang());

         _pDoc->SetUseBookMargin(_pBook->UseBookMargin());
         TRACE << "Use margin=" << _pBook->UseBookMargin() << endl;
         int top = 0, side = 0, bottom = 0;
         _pBook->GetMargins(top, side, bottom);
         TRACE << "Get margins=" << top << ", " << side << ", " << bottom << endl;

         _pDoc->SetMargins(top, side, bottom);

         _pDoc->SetFonts(_pBook->GetStandardFont(),
                         _pBook->GetSerifFamily(),
                         _pBook->GetSansFamily(),
                         _pBook->GetMonoFamily());

         _pDoc->SetIgnoreFonts(_pBook->GetIgnoreFonts());

         _pDoc->mainFrame()->setZoomFactor(_pBook->GetMagnification());
         TRACE << "zoom=" << _pBook->GetMagnification() << endl;

         _pDoc->setViewportSize(_Sz);

         TRACE << "_Sz h=" << _Sz.height() << " w=" << _Sz.width() << endl;

         // install slot in loading finished signal
         // this is where we load all the stuff and count pages

         connect(_pDoc->mainFrame(), SIGNAL(loadFinished(bool)),
                 this              , SLOT(LoadFinished(bool)));
      }

      // get section file

      QString s = _pBook->GetSectionFile(_currSection);
      TRACE << "Loading section #" << _currSection << " " << ENC(s) << endl;

      // and load it

      _pDoc->mainFrame()->load(QUrl(s));
   }
}

// -----------------------------------------------------------------------------
