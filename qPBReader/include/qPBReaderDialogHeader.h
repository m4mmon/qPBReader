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

#ifndef _qPBReaderDialogHeader_h__
#define _qPBReaderDialogHeader_h__

#include <QFrame>
class QHBoxLayout;
class QLabel;
class QPushButton;

/**
 * Class for decorating dialogs a la PocketBook.
 */
class qPBReaderDialogHeader : public QFrame
{
      Q_OBJECT;

   public :
      qPBReaderDialogHeader(QWidget * parent = 0, const QString & isLabel = "");
      ~qPBReaderDialogHeader();


   private :
      QHBoxLayout * horizontalLayout2;
      QLabel * pLabelHeader;
      QPushButton * pExitButton;
};

#endif
