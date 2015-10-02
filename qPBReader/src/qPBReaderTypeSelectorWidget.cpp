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

#include "qPBReaderTypeSelectorWidget.h"

// qt
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>

// -----------------------------------------------------------------------------

qPBReaderTypeSelectorWidget::qPBReaderTypeSelectorWidget(const QString & isText,
      QWidget * ipParent) :
   QWidget(ipParent)
{
   QBoxLayout * horizontalLayout = new QHBoxLayout(this);
   horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

   QLabel * pLabel = new QLabel(isText, this);
   pLabel->setObjectName("selectorLabel");
   horizontalLayout->addWidget(pLabel);

   _pComboBox = new QComboBox(this);
   _pComboBox->setObjectName("itemSelector");
   _pComboBox->addItem("Serif");
   _pComboBox->addItem("Sans-serif");
   _pComboBox->addItem("Monospace");
   horizontalLayout->addWidget(_pComboBox);
}

// -----------------------------------------------------------------------------

qPBReaderTypeSelectorWidget::~qPBReaderTypeSelectorWidget()
{
}

// -----------------------------------------------------------------------------
