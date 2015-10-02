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

#include "qPBReaderVerticalWidget.h"

// qt
#include <QCheckBox>
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

// project
#include "qPBReaderSeparatorWidget.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderVerticalWidget::qPBReaderVerticalWidget(QWidget * parent,
      Qt::WindowFlags f) :
   QWidget(parent, f),
   _pVerticalLayout(new QVBoxLayout(this)),
   _nItems(0),
   _nSeps(0)
{
   TRSCOPE(viewer, "qPBReaderVerticalWidget::qPBReaderVerticalWidget");

   setObjectName("qPBReaderVerticalWidget");
   _pVerticalLayout->setSpacing(0);
   _pVerticalLayout->setContentsMargins(0, 0, 0, 0);
}

// -----------------------------------------------------------------------------

qPBReaderVerticalWidget::~qPBReaderVerticalWidget()
{
   TRSCOPE(viewer, "qPBReaderVerticalWidget::~qPBReaderVerticalWidget");
}

// -----------------------------------------------------------------------------

void qPBReaderVerticalWidget::AddWidget(QWidget * ipWidget, bool bSetObjectName)
{
   TRSCOPE(viewer, "qPBReaderVerticalWidget::AddWidget");

   // if a previous item exists

   if (_nItems > 0)
   {
      // add a separator

      AddSeparator();
   }

   // build a name if requested

   if (bSetObjectName)
   {
      QString sObjName = "item_" + QString::number(_nItems);
      ipWidget->setObjectName(sObjName);
   }

   _pVerticalLayout->addWidget(ipWidget);

   TRACE << "Added widget " << ENC(ipWidget->objectName()) << endl;

   _nItems++;
}

// -----------------------------------------------------------------------------

void qPBReaderVerticalWidget::AddButton(const QString & isText,
                                        const QObject * ipReceiver,
                                        const char * ipcMethod,
                                        const QString & isIcon)
{
   TRSCOPE(viewer, "qPBReaderVerticalWidget::AddButton");

   // create button

   QPushButton * pPushButton = new QPushButton(this);

   // set display text

   pPushButton->setText(isText);

   // add icon if provided

   if (!isIcon.isEmpty())
   {
      pPushButton->setIcon(QIcon(isIcon));
   }

   // add it

   AddWidget(pPushButton);

   // and connect button click to slot

   QObject::connect(pPushButton, SIGNAL(clicked(bool)), ipReceiver, ipcMethod);
}

// -----------------------------------------------------------------------------

void qPBReaderVerticalWidget::AddToggle(const QString & isText,
                                        bool bChecked,
                                        const QObject * ipReceiver,
                                        const char * ipcMethod)
{
   TRSCOPE(viewer, "qPBReaderVerticalWidget::AddToggle");

   // create checkbox

   QCheckBox * pCheckBox = new QCheckBox(isText, this);

   // set initial state

   pCheckBox->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);

   // add it

   AddWidget(pCheckBox);

   // and connect checkbox state change to slot

   QObject::connect(pCheckBox, SIGNAL(stateChanged(int)), ipReceiver, ipcMethod);
}

// -----------------------------------------------------------------------------

void qPBReaderVerticalWidget::AddSeparator()
{
   // build a name

   QString sObjName = "line_" + QString::number(_nSeps);

   // and add separator to layout

   _pVerticalLayout->addWidget(
      new qPBReaderVSeparatorWidget(
         this, 0, sObjName
      )
   );

   _nSeps++;
}

// -----------------------------------------------------------------------------
