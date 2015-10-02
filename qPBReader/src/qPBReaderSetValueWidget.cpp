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

#include "qPBReaderSetValueWidget.h"

// qt
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// project
#include "qPBReaderSeparatorWidget.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderSetValueWidget::qPBReaderSetValueWidget(QWidget * parent,
      Qt::WindowFlags f) :
   QWidget(parent, f),
   _pTextLabel(new QLabel(this))
{
   TRSCOPE(viewer, "qPBReaderSetValueWidget::qPBReaderSetValueWidget");

   setObjectName("qPBReaderSetValueWidget");
   setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   setContentsMargins(0, 0, 0, 0);

   QBoxLayout * horizontalLayout = new QHBoxLayout(this);
   horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
   horizontalLayout->setSpacing(0);
   horizontalLayout->setContentsMargins(0, 0, 0, 0);

   QPushButton * pushButton1 = new QPushButton(this);
   pushButton1->setObjectName(QString::fromUtf8("decrementButton"));
   horizontalLayout->addWidget(pushButton1);

   horizontalLayout->addWidget(
      new qPBReaderHSeparatorWidget(
         this, 0, QString::fromUtf8("leftLine")
      )
   );

   _pTextLabel->setObjectName(QString::fromUtf8("scaleLabel"));
   _pTextLabel->setIndent(0);
   _pTextLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
   horizontalLayout->addWidget(_pTextLabel);

   horizontalLayout->addWidget(
      new qPBReaderHSeparatorWidget(
         this, 0, QString::fromUtf8("rightLine")
      )
   );

   QPushButton * pushButton2 = new QPushButton(this);
   pushButton2->setObjectName(QString::fromUtf8("incrementButton"));
   horizontalLayout->addWidget(pushButton2);

   QObject::connect(pushButton1, SIGNAL(clicked(bool)), this,
                    SLOT(MinusPressed()));
   QObject::connect(pushButton2, SIGNAL(clicked(bool)), this, SLOT(PlusPressed()));


   TRACE << "_pTextLabel->indent() = " << _pTextLabel->indent() <<
         " _pTextLabel->alignment() = " << (int)_pTextLabel->alignment() << endl;
}

// -----------------------------------------------------------------------------

qPBReaderSetValueWidget::~qPBReaderSetValueWidget()
{
   TRSCOPE(viewer, "qPBReaderSetValueWidget::~qPBReaderSetValueWidget");
}

// -----------------------------------------------------------------------------

void qPBReaderSetValueWidget::SetText(const QString & isText)
{
   return _pTextLabel->setText(isText);
}

// -----------------------------------------------------------------------------

QString qPBReaderSetValueWidget::GetText() const
{
   return _pTextLabel->text();
}

// -----------------------------------------------------------------------------

void qPBReaderSetValueWidget::MinusPressed()
{
   TRSCOPE(viewer, "qPBReaderSetValueWidget::MinusPressed");

   emit Minus();
}

// -----------------------------------------------------------------------------

void qPBReaderSetValueWidget::PlusPressed()
{
   TRSCOPE(viewer, "qPBReaderSetValueWidget::PlusPressed");

   emit Plus();
}

// -----------------------------------------------------------------------------
