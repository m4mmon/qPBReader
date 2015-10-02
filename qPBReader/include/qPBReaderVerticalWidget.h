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

#ifndef _qPBReaderVerticalWidget_h__
#define _qPBReaderVerticalWidget_h__

#include <QWidget>
class QVBoxLayout;

class qPBReaderVerticalWidget : public QWidget
{
      Q_OBJECT;

   public :
      qPBReaderVerticalWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
      ~qPBReaderVerticalWidget();

      void AddWidget(QWidget * ipWidget, bool bSetObjectName = true);

      void AddButton(const QString & isText,
                     const QObject * ipReceiver,
                     const char * ipcMethod,
                     const QString & isIcon = "");
      void AddToggle(const QString & isText,
                     bool bChecked,
                     const QObject * ipReceiver,
                     const char * ipcMethod);

   private :
      void AddSeparator();

      QVBoxLayout * _pVerticalLayout;
      int _nItems;
      int _nSeps;
};


#endif
