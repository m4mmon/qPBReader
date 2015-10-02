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

#ifndef _qPBReaderSeparatorWidget_h__
#define _qPBReaderSeparatorWidget_h__

#include <QFrame>

class qPBReaderSeparatorWidget : public QFrame
{
      Q_OBJECT;

   public :
      qPBReaderSeparatorWidget(bool bHorizontalBar,
                               QWidget * parent = 0,
                               Qt::WindowFlags f = 0,
                               const QString & isName = "");
      virtual ~qPBReaderSeparatorWidget();
};

class qPBReaderHSeparatorWidget : public qPBReaderSeparatorWidget
{
      Q_OBJECT;

   public :
      qPBReaderHSeparatorWidget(QWidget * parent = 0,
                                Qt::WindowFlags f = 0,
                                const QString & isName = "");
      ~qPBReaderHSeparatorWidget();
};

class qPBReaderVSeparatorWidget : public qPBReaderSeparatorWidget
{
      Q_OBJECT;

   public :
      qPBReaderVSeparatorWidget(QWidget * parent = 0,
                                Qt::WindowFlags f = 0,
                                const QString & isName = "");
      ~qPBReaderVSeparatorWidget();
};




#endif
