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

#ifndef _qPBReaderFontSettingsDialog_h__
#define _qPBReaderFontSettingsDialog_h__

#include "qPBReaderDialog.h"

class QCheckBox;
class qPBReaderDocView;
class qPBReaderFontSelectorWidget;
class qPBReaderTypeSelectorWidget;
class qPBReaderViewer;

class qPBReaderFontSettingsDialog : public qPBReaderDialog
{
      Q_OBJECT

   public :

      qPBReaderFontSettingsDialog(qPBReaderDocView * ipView,
                                  qPBReaderViewer * parent);
      ~qPBReaderFontSettingsDialog();

      int exec();

   private slots :

      void SelectionChanged();

   private :

      QCheckBox * _pIgnoreFontsCheckBox;
      qPBReaderFontSelectorWidget * _pFonts[3];
      qPBReaderTypeSelectorWidget * _pType;
      qPBReaderDocView * _pView;
};

#endif
