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

#include "qPBReaderEpubOpf.h"

// qt
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QXmlQuery>

// project
#include "qPBReaderEpubSettings.h"
#include "qPBReaderTracer.h"
#include "qPBReaderUtils.h"

// -----------------------------------------------------------------------------

// media-types
#define CSSTYPE1    "text/css"
#define CSSTYPE2    "text/x-oeb1-css"
#define CSSTYPE3    "text/x-oeb-css"
#define NCXTYPE     "application/x-dtbncx+xml"

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderEpubOpf::qPBReaderEpubOpf() :
   _pBookSettings(0),
   _nOpfIdx(-1),
   _nNbSections(0),
   _nCurrentSection(-1)
{
   TRSCOPE(opf, "qPBReaderEpubOpf::qPBReaderEpubOpf");

}

// -----------------------------------------------------------------------------

qPBReaderEpubOpf::~qPBReaderEpubOpf()
{
   TRSCOPE(opf, "qPBReaderEpubOpf::~qPBReaderEpubOpf");

   Clear();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubOpf::Clear()
{
   _pBookSettings = 0;
   _nOpfIdx = -1;
   _sFile.clear();
   _sBaseDir.clear();
   _nNbSections = 0;
   _nCurrentSection = -1;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::Set(qPBReaderEpubSettings * ipBookSettings,
                           int inOpfIdx,
                           const QString & isFile)
{
   TRSCOPE(opf, "qPBReaderEpubOpf::Set");

   Clear();

   _pBookSettings = ipBookSettings;
   _nOpfIdx = inOpfIdx;
   _sFile = isFile;
   _sBaseDir = QFileInfo(_sFile).dir().path();

   TRACE << "Using " << ENC(_sBaseDir) << " as base dir" << endl;

   bool b = RetrieveSections();

   if (b)
   {
      b = RetrieveNCX();
   }

   //
   if (b)
   {
      RetrieveTitle();
   }

   if (b)
   {
      // default

      _nCurrentSection = 0;

      // try settings

      int n = _pBookSettings->GetCurrentSectionInOpf(_nOpfIdx);
      TRACE << "Settings value =" << n << endl;

      if (n < 0 || n >= _nNbSections)
      {
         n = 0;
      }

      SetCurrentSection(n);

      TRACE << VAR(_nCurrentSection) << endl;
   }

   if (!b)
   {
      Clear();
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::PreviousSection()
{
   return SetCurrentSection(_nCurrentSection - 1);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::NextSection()
{
   return SetCurrentSection(_nCurrentSection + 1);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::IsFirstSection()
{
   return _nCurrentSection == 0;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::IsLastSection()
{
   return (_nCurrentSection + 1) == _nNbSections;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::SetCurrentSection(int idx)
{
   // if valid

   bool b = idx >= 0 && idx < _nNbSections;

   // change current section index
   // and store in settings

   if (b)
   {
      _nCurrentSection = idx;
      _pBookSettings->SaveCurrentSectionInOpf(_nOpfIdx, _nCurrentSection);
   }

   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubOpf::GetCurrentSectionFile()
{
   return _pBookSettings->GetSectionFileName(_nOpfIdx, _nCurrentSection);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::GetSectionFileName(int idx, QString & osFileName)
{
   osFileName.clear();

   bool b = idx >= 0 && idx < _nNbSections;

   if (b)
   {
      osFileName = _pBookSettings->GetSectionFileName(_nOpfIdx, idx);
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::GetCSSFiles(QStringList & olsCssFiles)
{
   TRSCOPE(opf, "qPBReaderEpubOpf::GetCSSFiles");

   olsCssFiles.clear();

   // create query from file

   QXmlQuery qry;
   bool b = qry.setFocus(QUrl(_sFile));
   TRACE << "qry.setFocus(): " << b << endl;


   // prepare query
   // retrieve all idref values from package/spine/itemref

   if (b)
   {
      QString sCrit = "lower-case(@media-type)='" CSSTYPE1 "' "
                      "or lower-case(@media-type)='" CSSTYPE2 "' "
                      "or lower-case(@media-type)='" CSSTYPE3 "'";
      QString sQry = "declare default element namespace \"" OPFNS "\";"
                     "package/manifest/item[" + sCrit + "]/@href/string()";

      TRACE << ENC(sQry) << endl;
      qry.setQuery(sQry);
      b = qry.isValid();
      TRACE << "qry.isValid(): " << b << endl;
   }

   // execute it. No result is not an error, so don't check there is at least
   // one result.

   QStringList lsCss;

   if (b)
   {
      b = qry.evaluateTo(&lsCss);
      TRACE << "evaluation " << b << " nbcss=" << lsCss.size() << endl;
   }

   // and build result

   if (b)
   {
      for (int idx = 0; idx < lsCss.size(); idx++)
      {
         olsCssFiles.append(_sBaseDir + "/" + lsCss[idx]);
      }
   }

   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubOpf::GetNCX()
{
   return _pBookSettings->GetNCXFile(_nOpfIdx);
}

// -----------------------------------------------------------------------------

int qPBReaderEpubOpf::GetSectionNumber(const QString & isFileName)
{
   int pos = -1;

   for (int idx = 0; pos < 0 && idx < _nNbSections; idx++)
   {
      QString sSection = qPBReaderUtils::CleanPath(
         _sBaseDir + "/" + _pBookSettings->GetSectionFileName(_nOpfIdx, idx)
      );
      if (isFileName == sSection)
      {
         pos = idx;
      }
   }

   return pos;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::SetCurrentSection(const QString & isFileName)
{
   return SetCurrentSection(GetSectionNumber(isFileName));
}

// -----------------------------------------------------------------------------

void qPBReaderEpubOpf::SavePagesCount(const QList<int> & ilnPagesCount)
{
   _pBookSettings->SavePagesCountInOpf(_nOpfIdx, ilnPagesCount);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubOpf::GetPagesCount(QList<int> & olnPagesCount)
{
   _pBookSettings->GetPagesCountInOpf(_nOpfIdx, olnPagesCount);
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::RetrieveSections()
{
   TRSCOPE(opf, "qPBReaderEpubOpf::RetrieveSections");

   bool b = true;

   _nNbSections = _pBookSettings->GetNbSectionsInOpf(_nOpfIdx);

   TRACE << VAR(_nNbSections) << endl;

   if (_nNbSections > 0)
   {
      TRACE << "done" << endl;
      return b;
   }

   TRACE << "Reading opf" << endl;

   QXmlQuery qry;
   b = qry.setFocus(QUrl(_sFile));
   TRACE << "qry.setFocus(): " << b << endl;

   // prepare query
   // retrieve all idref values from package/spine/itemref

   if (b)
   {
      qry.setQuery("declare default element namespace \"" OPFNS "\";"
                   "package/spine/itemref/@idref/string()");
      b = qry.isValid();
      TRACE << "qry.isValid(): " << b << endl;
   }

   // execute it

   QStringList lsSectionId;

   if (b)
   {
      b = qry.evaluateTo(&lsSectionId) && lsSectionId.size() > 0;
      TRACE << "evaluation " << b << " nbsections=" << lsSectionId.size() << endl;
   }

   // for each section id

   if (b)
   {
      QStringList lsSections;

      // for each id

      for (int idx = 0; idx < lsSectionId.size(); idx++)
      {
         // prepare query
         // retrieve path to corresponding file stored as href in package/manifest/item where id matches

         qry.setQuery("declare default element namespace \"" OPFNS "\";"
                      "package/manifest/item[@id='" + lsSectionId[idx] + "']/@href/string()");
         b = qry.isValid();
         TRACE << "qry.isValid(): " << b << endl;

         // execute it

         QStringList lsHref;

         if (b)
         {
            b = qry.evaluateTo(&lsHref) && lsHref.size() == 1;
            TRACE << "evaluation " << b << " nb href=" << lsHref.size() << endl;
         }

         // check result
         // path to section is relative to opf path

         QString sSectionFile;

         if (b)
         {
            sSectionFile = _sBaseDir + "/" + lsHref[0];
            QFile f(sSectionFile);
            b = f.exists();
            TRACE << "section " << ENC(sSectionFile) " exists:" << b << endl;
            TRACE << "        " << ENC(f.fileName()) << endl;
         }
         if (b)
         {
            //lsSections.append(QFileInfo(sSectionFile).absoluteFilePath());
            lsSections.append(lsHref[0]);
         }
      }

      // and save list in settings

      if (b)
      {
         _pBookSettings->SaveSectionsInOpf(_nOpfIdx, lsSections);
      }
   }

   TRACE << "result=" << b << endl;

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::RetrieveNCX()
{
   TRSCOPE(opf, "qPBReaderEpubOpf::RetrieveNCX");

   bool b = true;

   QString sNCXFile = _pBookSettings->GetNCXFile(_nOpfIdx);

   // found in settings

   if (!sNCXFile.isEmpty())
   {
      TRACE << "Found in settings " << ENC(sNCXFile) << endl;
      return b;
   }

   QXmlQuery qry;
   b = qry.setFocus(QUrl(_sFile));
   TRACE << "qry.setFocus(): " << b << endl;

   // prepare query
   // retrieve ncx

   if (b)
   {
      QString sQry = "declare default element namespace \"" OPFNS "\";"
                     "package/manifest/item[lower-case(@media-type)='" NCXTYPE "']/@href/string()";

      TRACE << ENC(sQry) << endl;
      qry.setQuery(sQry);
      b = qry.isValid();
      TRACE << "qry.isValid(): " << b << endl;
   }

   // execute query

   QStringList lsNcx;

   if (b)
   {
      b = qry.evaluateTo(&lsNcx) && lsNcx.size() == 1;
      TRACE << "evaluation " << b << " nb=" << lsNcx.size() << endl;
   }

   // save ncx relative path in settings

   if (b)
   {
      TRACE << "Got " << ENC(lsNcx[0]) << endl;

      _pBookSettings->SaveNCXFile(
         _nOpfIdx,
         lsNcx[0]
      );
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubOpf::RetrieveTitle()
{
   TRSCOPE(opf, "qPBReaderEpubOpf::RetrieveTitle");

   bool b = true;

   _sTitle = _pBookSettings->GetTitle(_nOpfIdx);

   // found in settings

   if (!_sTitle.isEmpty())
   {
      TRACE << "Found in settings " << ENC(_sTitle) << endl;
      return b;
   }

   QXmlQuery qry;
   b = qry.setFocus(QUrl(_sFile));
   TRACE << "qry.setFocus(): " << b << endl;

   // prepare query
   // retrieve title

   if (b)
   {
      QString sQry = "declare default element namespace \"" OPFNS "\";"
                     "declare namespace dc = \"" DCNS "\";"
                     "package/metadata/dc:title/string()";

      TRACE << ENC(sQry) << endl;
      qry.setQuery(sQry);
      b = qry.isValid();
      TRACE << "qry.isValid(): " << b << endl;
   }

   // execute it

   QStringList lsTmp;

   if (b)
   {
      b = qry.evaluateTo(&lsTmp) && lsTmp.size() > 0;
      TRACE << "evaluation " << b << " nbopf=" << lsTmp.size() << endl;
   }

   if (b)
   {
      _sTitle = lsTmp[0];
      TRACE << "Title " << ENC(_sTitle) << endl;
   }

   else
   {
      _sTitle = "Title not found";
   }

   _pBookSettings->SaveTitle(_nOpfIdx, _sTitle);

   return b;
}

// -----------------------------------------------------------------------------
