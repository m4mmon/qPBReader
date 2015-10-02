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

#ifndef _qPBReaderTextLayoutSettingsDialog_h__
#define _qPBReaderTextLayoutSettingsDialog_h__

#include "qPBReaderDialog.h"

class QCheckBox;
class QPushButton;
class qPBReaderComboBox;
class qPBReaderDocView;
class qPBReaderSetValueWidget;
class qPBReaderViewer;
class qPBReaderZoomWidget;

class qPBReaderTextLayoutSettingsDialog : public qPBReaderDialog
{
      Q_OBJECT;

   public :
      qPBReaderTextLayoutSettingsDialog(qPBReaderDocView * ipView,
                                        qPBReaderViewer * parent);
      ~qPBReaderTextLayoutSettingsDialog();

      void SetMarginValues(int top, int side, int bottom);


      int exec();

   signals :

      void TextLayoutBookMarginSignal(bool bUseMargin);

   private slots :

      void CheckBoxStateChanged(int state);
      void UseHyphensChanged(int state);
      void HyphenDicChanged(const QString & isHyphenPatternLang);

   private :

      qPBReaderDocView * _pView;
      qPBReaderViewer * _pParent;
      qPBReaderZoomWidget * _pZoomWidget;
      QCheckBox * _pUseBookMarginCheckBox;
      qPBReaderSetValueWidget * _pTopMarginValueWidget;
      qPBReaderSetValueWidget * _pSideMarginValueWidget;
      qPBReaderSetValueWidget * _pBottomMarginValueWidget;
      QPushButton * _pDefaultMarginButton;
      QCheckBox * _pUseHyphens;
      qPBReaderComboBox * _pHyphenDic;
};

#endif
