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

#ifndef _qPBReaderConfigDialog_h__
#define _qPBReaderConfigDialog_h__

#include "qPBReaderDialog.h"

class QCheckBox;
class qPBReaderComboBox;
class qPBReaderFontSelectorWidget;
class qPBReaderSetValueWidgetWithLabel;
class qPBReaderTypeSelectorWidget;
class qPBReaderZoomWidgetWithLabel;

class qPBReaderConfigDialog : public qPBReaderDialog
{
      Q_OBJECT;

   public :

      qPBReaderConfigDialog();
      ~qPBReaderConfigDialog();

   private slots:

      void ApplicationLanguageChanged(const QString & isLang);
      void MagnificationStepPlus();
      void MagnificationStepMinus();
      void UseBookMarginChanged(int state);
      void TopMarginMinus();
      void TopMarginPlus();
      void SideMarginMinus();
      void SideMarginPlus();
      void BottomMarginMinus();
      void BottomMarginPlus();
      void UseHyphensChanged(int state);
      void HyphenDicChanged(const QString & isLang);
      void IgnoreFontsChanged(int state);
      void SelectionChanged();

   private :
      qPBReaderComboBox * _pCBLang;
      qPBReaderZoomWidgetWithLabel * _pZoom;
      QCheckBox * _pUseBookMarginCheckBox;
      qPBReaderSetValueWidgetWithLabel * _pTop;
      qPBReaderSetValueWidgetWithLabel * _pSide;
      qPBReaderSetValueWidgetWithLabel * _pBottom;
      QCheckBox * _pUseHyphens;
      qPBReaderComboBox * _pHyphenDic;
      QCheckBox * _pIgnoreFontsCheckBox;
      qPBReaderFontSelectorWidget * _pFonts[3];
      qPBReaderTypeSelectorWidget * _pType;
};

#endif
