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

#include "qPBReaderContextMenu.h"

// qt
#include <QKeyEvent>

// project
#include "qPBReaderTracer.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderViewer.h"

// -----------------------------------------------------------------------------

qPBReaderContextMenu::qPBReaderContextMenu(qPBReaderViewer * parent,
                                           QPoint iPt) :
   qPBReaderDialog(parent),
   _pt(iPt),
   _pViewer(parent)
{
   TRSCOPE(viewer, "qPBReaderContextMenu::qPBReaderContextMenu");


   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);

   pVert->AddButton(tr("Dictionary"), this, SLOT(Dictionary()), ":dico.bmp");
   //pVert->AddButton(tr("xxx"), parent, SLOT(FontOptions()), ":fonts.bmp");


   addWidget(pVert);
}

// -----------------------------------------------------------------------------

qPBReaderContextMenu::~qPBReaderContextMenu()
{
   TRSCOPE(viewer, "qPBReaderContextMenu::~qPBReaderContextMenu");
}

// -----------------------------------------------------------------------------

void qPBReaderContextMenu::keyPressEvent(QKeyEvent * iEvt)
{
   TRSCOPE(viewer, "qPBReaderContextMenu::keyPressEvent");

   int nKeyCode = 0;
   bool bAutoRepeat = false;

   if (iEvt)
   {
      nKeyCode = iEvt->key();
      bAutoRepeat = iEvt->isAutoRepeat();
   }

   TRACE << "nKeyCode=" << (void *)nKeyCode << " " << VAR(bAutoRepeat) << endl;


   switch (nKeyCode)
   {
#ifdef Q_OS_POCKETBOOK

      case Qt::Key_Menu:
#else
      case Qt::Key_F6:
#endif
      Close();
   }
}

// -----------------------------------------------------------------------------

void qPBReaderContextMenu::Dictionary()
{
   TRSCOPE(viewer, "qPBReaderContextMenu::Dictionary");

   Close();

   _pViewer->Dictionary(_pt);
}

// -----------------------------------------------------------------------------

void qPBReaderContextMenu::Close()
{
   TRSCOPE(viewer, "qPBReaderContextMenu::Close");

   hide();
   accept();
   _pViewer->activateWindow();
}

// -----------------------------------------------------------------------------
