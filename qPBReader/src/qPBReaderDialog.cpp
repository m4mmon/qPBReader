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

#include "qPBReaderDialog.h"

// qt
#include <QVBoxLayout>

// project
#include "qPBReaderDialogHeader.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderDialog::qPBReaderDialog(QWidget * parent,
                                 const QString & isTitle,
                                 Qt::WindowFlags f) :
   QDialog(parent, f),
   _pHeader(0),
   _pVLayout(new  QVBoxLayout(this))
{
   TRSCOPE(viewer, "qPBReaderDialog::qPBReaderDialog");
   setContentsMargins(0, 0, 0, 0);

   if (!isTitle.isEmpty())
   {
      _pVLayout->setSpacing(0);
      _pVLayout->setContentsMargins(0, 0, 0, 0);
      _pHeader = new qPBReaderDialogHeader(this, isTitle);
      _pVLayout->addWidget(_pHeader);
   }
}

// -----------------------------------------------------------------------------

qPBReaderDialog::~qPBReaderDialog()
{
   TRSCOPE(viewer, "qPBReaderDialog::~qPBReaderDialog");
}

// -----------------------------------------------------------------------------

void qPBReaderDialog::addWidget(QWidget * widget, int stretch,
                                Qt::Alignment alignment)
{
   TRSCOPE(viewer, "qPBReaderDialog::addWidget");
   _pVLayout->addWidget(widget, stretch, alignment);
}

// -----------------------------------------------------------------------------
