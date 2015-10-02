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

#include "qPBReaderSeparatorWidget.h"

// -----------------------------------------------------------------------------

qPBReaderSeparatorWidget::qPBReaderSeparatorWidget(bool bHorizontalBar,
      QWidget * parent,
      Qt::WindowFlags f,
      const QString & isName) :
   QFrame(parent, f)
{
   setObjectName(isName);
   setFrameShadow(QFrame::Plain);
   setFrameShape(bHorizontalBar ? QFrame::HLine : QFrame::VLine);
}

// -----------------------------------------------------------------------------

qPBReaderSeparatorWidget::~qPBReaderSeparatorWidget()
{
}

// -----------------------------------------------------------------------------

qPBReaderHSeparatorWidget::qPBReaderHSeparatorWidget(QWidget * parent,
      Qt::WindowFlags f,
      const QString & isName) :
   qPBReaderSeparatorWidget(false, parent, f, isName)
{
}

// -----------------------------------------------------------------------------

qPBReaderHSeparatorWidget::~qPBReaderHSeparatorWidget()
{
}

// -----------------------------------------------------------------------------

qPBReaderVSeparatorWidget::qPBReaderVSeparatorWidget(QWidget * parent,
      Qt::WindowFlags f,
      const QString & isName) :
   qPBReaderSeparatorWidget(true, parent, f, isName)
{
}

// -----------------------------------------------------------------------------

qPBReaderVSeparatorWidget::~qPBReaderVSeparatorWidget()
{
}

// -----------------------------------------------------------------------------
