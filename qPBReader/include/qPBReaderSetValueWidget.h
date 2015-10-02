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

#ifndef _qPBReaderSetValueWidget_h__
#define _qPBReaderSetValueWidget_h__

#include <QWidget>

class QLabel;

class qPBReaderSetValueWidget : public QWidget
{
      Q_OBJECT;

   public :
      qPBReaderSetValueWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
      ~qPBReaderSetValueWidget();

      void SetText(const QString & isText);
      QString GetText() const;

   signals :

      void Minus();
      void Plus();

   protected :

      QLabel * _pTextLabel;

   private slots :

      void MinusPressed();
      void PlusPressed();

};

#endif
