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

#ifndef _qPBReaderGoBackDlg_h__
#define _qPBReaderGoBackDlg_h__

#include "qPBReaderDialog.h"

class qPBReaderViewer;

class qPBReaderGoBackDlg : public qPBReaderDialog
{
      Q_OBJECT;

   public :

      qPBReaderGoBackDlg(qPBReaderViewer * parent);
      ~qPBReaderGoBackDlg();

      void Show();
      void Hide();

   private slots:

      void Move();

   private :

      int _nPos;
      qPBReaderViewer * _pViewer;
};

#endif
