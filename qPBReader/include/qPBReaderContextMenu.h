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

#ifndef _qPBReaderContextMenu_h__
#define _qPBReaderContextMenu_h__

#include "qPBReaderDialog.h"

class qPBReaderDocView;
class qPBReaderViewer;

class qPBReaderContextMenu : public qPBReaderDialog
{
      Q_OBJECT;

   public :

      qPBReaderContextMenu(qPBReaderViewer * parent,
                           QPoint iPt);
      ~qPBReaderContextMenu();

   protected :

      void keyPressEvent(QKeyEvent * iEvt);

   private slots :

      void Dictionary();

   private :

      void Close();

      QPoint _pt;
      qPBReaderViewer * _pViewer;
};

#endif
