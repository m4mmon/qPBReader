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

#include "qPBReaderDialogHeader.h"

// qt
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// project
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderDialogHeader::qPBReaderDialogHeader(QWidget * parent,
      const QString & isLabel) :
   QFrame(parent)
{
   TRSCOPE(viewer, "qPBReaderDialogHeader::qPBReaderDialogHeader");

   horizontalLayout2 = new QHBoxLayout(this);
   horizontalLayout2->setSpacing(0);
   horizontalLayout2->setContentsMargins(0, 0, 0, 0);

   pLabelHeader = new QLabel(this);
   pLabelHeader->setObjectName(QString::fromUtf8("captionLabel"));
   pLabelHeader->setText(isLabel);

   pExitButton = new QPushButton(this);
   pExitButton->setObjectName(QString::fromUtf8("exitButton"));
   pExitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

   horizontalLayout2->addWidget(pLabelHeader);
   horizontalLayout2->addStretch(0);
   horizontalLayout2->addWidget(pExitButton);
   QObject::connect(pExitButton, SIGNAL(clicked(bool)), parent, SLOT(accept()));

   pLabelHeader->setContentsMargins(0, 0, 1, 5);
}

// -----------------------------------------------------------------------------

qPBReaderDialogHeader::~qPBReaderDialogHeader()
{
   TRSCOPE(viewer, "qPBReaderDialogHeader::~qPBReaderDialogHeader");
}

// -----------------------------------------------------------------------------
