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

#include "qPBReaderGoBackDlg.h"

// qt
#include <QPushButton>
#include <QTimer>

// project
#include "qPBReaderGoBackBtn.h"
#include "qPBReaderPlatform.h"
#include "qPBReaderTracer.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderViewer.h"

// -----------------------------------------------------------------------------

qPBReaderGoBackDlg::qPBReaderGoBackDlg(qPBReaderViewer * parent) :
   qPBReaderDialog(parent),
   _nPos(1),
   _pViewer(parent)
{
   TRSCOPE(viewer, "qPBReaderGoBackDlg::qPBReaderGoBackDlg");


   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);


   qPBReaderGoBackBtn * pBtn = new qPBReaderGoBackBtn(this);
   pVert->AddWidget(pBtn);


   // plug stuff

   QObject::connect(pBtn->_pActionButton, SIGNAL(clicked(bool)),
                     _pViewer, SLOT(BackFromLink()));

   QObject::connect(pBtn->_pMoveButton, SIGNAL(clicked(bool)),
                     this, SLOT(Move()));



/*
   pVert->AddButton(
      tr("Back"), _pViewer, SLOT(BackFromLink()), ":back.bmp"
   );
*/
   addWidget(pVert);
}

// -----------------------------------------------------------------------------

qPBReaderGoBackDlg::~qPBReaderGoBackDlg()
{
   TRSCOPE(viewer, "qPBReaderGoBackDlg::~qPBReaderGoBackDlg");
}

// -----------------------------------------------------------------------------

void qPBReaderGoBackDlg::Show()
{
   TRSCOPE(viewer, "qPBReaderGoBackDlg::Show");

   show();

   Move();

   _pViewer->activateWindow();
}

// -----------------------------------------------------------------------------

void qPBReaderGoBackDlg::Hide()
{
   TRSCOPE(viewer, "qPBReaderGoBackDlg::Hide");
   hide();
   _pViewer->activateWindow();
}

// -----------------------------------------------------------------------------

void qPBReaderGoBackDlg::Move()
{
   TRSCOPE(viewer, "qPBReaderGoBackDlg::Move");

   int newx = _pViewer->width() - size().width();
   int newy = _nPos * _pViewer->height() / 3;

   TRACE << "Setting pos (" << newx << ", " << newy << ")" << endl;

   move(newx, newy);


   _nPos++;
   if (_nPos > 2)
   {
      _nPos = 1;
   }

   update();
   _pViewer->activateWindow();
}

// -----------------------------------------------------------------------------
