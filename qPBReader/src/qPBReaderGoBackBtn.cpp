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

#include "qPBReaderGoBackBtn.h"

// qt
#include <QPushButton>
#include <QVBoxLayout>

// project
#include "qPBReaderTracer.h"

qPBReaderGoBackBtn::qPBReaderGoBackBtn(QWidget * ipParent) :
   QWidget(ipParent),
   _pActionButton(0),
   _pMoveButton(0)
{
   TRSCOPE(viewer, "qPBReaderGoBackBtn::qPBReaderGoBackBtn");

   setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   setContentsMargins(0, 0, 0, 0);

   QBoxLayout * horizontalLayout = new QHBoxLayout(this);
   horizontalLayout->setObjectName("horizontalLayout");
   horizontalLayout->setSpacing(0);
   horizontalLayout->setContentsMargins(0, 0, 0, 0);

   _pActionButton = new QPushButton(this);
   _pActionButton->setObjectName("actionbtn");
   _pActionButton->setText(tr("Back"));
   _pActionButton->setIcon(QIcon(":back.bmp"));
   horizontalLayout->addWidget(_pActionButton);


   _pMoveButton = new QPushButton(this);
   _pMoveButton->setObjectName("movebtn");
   _pMoveButton->setIcon(QIcon(":move.bmp"));
   horizontalLayout->addWidget(_pMoveButton);
}


qPBReaderGoBackBtn::~qPBReaderGoBackBtn()
{
   TRSCOPE(viewer, "qPBReaderGoBackBtn::~qPBReaderGoBackBtn");
}
