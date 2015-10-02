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

#include "qPBReaderConfigDialog.h"

// qt
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QFontComboBox>
#include <QLabel>

// project
#include "qPBReaderComboBox.h"
#include "qPBReaderConfig.h"
#include "qPBReaderFontSelectorWidget.h"
#include "qPBReaderLangLists.h"
#include "qPBReaderSetValueWidgetWithLabel.h"
#include "qPBReaderTracer.h"
#include "qPBReaderTypeSelectorWidget.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderZoomWidget.h"
#include "qPBReaderZoomWidgetWithLabel.h"

// -----------------------------------------------------------------------------

qPBReaderConfigDialog::qPBReaderConfigDialog() :
   qPBReaderDialog(0, tr("qPBReader configuration"), Qt::Popup)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::qPBReaderConfigDialog");

   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);
   addWidget(pVert);

   // ----------------------------------
   // main language
   // ----------------------------------
   {
      _pCBLang = new qPBReaderComboBox(tr("Application language"), this);
      const QStringList & lsLang = qPBReaderLangLists::GetAvailableApplicationLanguages();
      _pCBLang->_pComboBox->addItems(lsLang);
      pVert->AddWidget(_pCBLang);

      int nPos = lsLang.indexOf(qPBReaderConfig::GetApplicationLanguage());
      if (nPos < 0)
      {
         nPos = 0;
      }
      _pCBLang->_pComboBox->setCurrentIndex(nPos);


      QObject::connect(
         _pCBLang->_pComboBox, SIGNAL(currentIndexChanged(const QString &)),
         this                , SLOT  (ApplicationLanguageChanged(const QString &))
      );
   }

   // ----------------------------------
   // magnification
   // ----------------------------------

   {
      _pZoom = new qPBReaderZoomWidgetWithLabel(tr("Font size"), this);
      _pZoom->_pZoomWidget->SetValue(qPBReaderConfig::GetDefaultMagnificationFactor());
      pVert->AddWidget(_pZoom);

      QObject::connect(_pZoom->_pZoomWidget, SIGNAL(Minus()), this, SLOT(MagnificationStepMinus()));
      QObject::connect(_pZoom->_pZoomWidget, SIGNAL(Plus()),  this, SLOT(MagnificationStepPlus()));
   }

   // ----------------------------------
   // margins
   // ----------------------------------

   {
      bool bUseBookMargin = qPBReaderConfig::GetDefaultUseBookMargin();
      Qt::CheckState state = bUseBookMargin ? Qt::Checked : Qt::Unchecked;
      int top, side, bottom;
      qPBReaderConfig::GetDefaultMargins(top, side, bottom);

      // default margin
      _pUseBookMarginCheckBox = new QCheckBox(tr("Use book margin"), pVert);
      _pUseBookMarginCheckBox->setCheckState(state);
      pVert->AddWidget(_pUseBookMarginCheckBox);

      // top margin
      _pTop = new qPBReaderSetValueWidgetWithLabel(tr("Top margin"), this);
      _pTop->_pSetValueWidget->SetText(QString::number(top));
      _pTop->_pSetValueWidget->setEnabled(!bUseBookMargin);
      pVert->AddWidget(_pTop);

      // side margin
      _pSide = new qPBReaderSetValueWidgetWithLabel(tr("Side margin"), this);
      _pSide->_pSetValueWidget->SetText(QString::number(side));
      _pSide->_pSetValueWidget->setEnabled(!bUseBookMargin);
      pVert->AddWidget(_pSide);

      // bottom margin
      _pBottom = new qPBReaderSetValueWidgetWithLabel("Bottom margin", this);
      _pBottom->_pSetValueWidget->SetText(QString::number(bottom));
      _pBottom->_pSetValueWidget->setEnabled(!bUseBookMargin);
      pVert->AddWidget(_pBottom);

      QObject::connect(_pUseBookMarginCheckBox, SIGNAL(stateChanged(int)),
                       this, SLOT(UseBookMarginChanged(int)));

      QObject::connect(_pTop->_pSetValueWidget, SIGNAL(Minus()),
                       this                   , SLOT(TopMarginMinus()));
      QObject::connect(_pTop->_pSetValueWidget, SIGNAL(Plus()),
                       this                   , SLOT(TopMarginPlus()));
      QObject::connect(_pSide->_pSetValueWidget, SIGNAL(Minus()),
                       this                    , SLOT(SideMarginMinus()));
      QObject::connect(_pSide->_pSetValueWidget, SIGNAL(Plus()),
                       this                    , SLOT(SideMarginPlus()));
      QObject::connect(_pBottom->_pSetValueWidget, SIGNAL(Minus()),
                       this                      , SLOT(BottomMarginMinus()));
      QObject::connect(_pBottom->_pSetValueWidget, SIGNAL(Plus()),
                       this                      , SLOT(BottomMarginPlus()));
   }


   // ----------------------------------
   // hyphens
   // ----------------------------------

   {
      QString sHyphenPatt = qPBReaderConfig::GetDefaultHyphenPatternLang();
      bool bUseHyphens = !sHyphenPatt.isEmpty();
      Qt::CheckState state = bUseHyphens ? Qt::Checked : Qt::Unchecked;
      TRACE << VAR(bUseHyphens) << endl;
      const QStringList & lsLang = qPBReaderLangLists::GetAvailableHyphenPatterns();

      // use hyphens

      _pUseHyphens = new QCheckBox(tr("Hyphenation"), pVert);
      _pUseHyphens->setCheckState(state);
      pVert->AddWidget(_pUseHyphens);

      // hyphen lang

      _pHyphenDic = new qPBReaderComboBox(tr("Hyphen dictionary language"), this);
      _pHyphenDic->_pComboBox->addItems(lsLang);
      int nPos = lsLang.indexOf(sHyphenPatt);
      if (nPos < 0)
      {
         nPos = 9; // en-us
      }
      _pHyphenDic->_pComboBox->setCurrentIndex(nPos);
      _pHyphenDic->_pComboBox->setEnabled(bUseHyphens);
      pVert->AddWidget(_pHyphenDic);

      QObject::connect(_pUseHyphens, SIGNAL(stateChanged(int)),
                       this, SLOT(UseHyphensChanged(int)));

      QObject::connect(_pHyphenDic->_pComboBox,
                       SIGNAL(currentIndexChanged(const QString &)),
                       this, SLOT(HyphenDicChanged(const QString &)));
   }

   // ----------------------------------
   // fonts
   // ----------------------------------

   {   
      bool bIgnoreFonts = qPBReaderConfig::GetDefaultIgnoreFonts();

      // ignore book fonts

      _pIgnoreFontsCheckBox = new QCheckBox(tr("Ignore book fonts"), pVert);
      Qt::CheckState state = bIgnoreFonts ? Qt::Checked : Qt::Unchecked;
      _pIgnoreFontsCheckBox->setCheckState(state);
      pVert->AddWidget(_pIgnoreFontsCheckBox);

      // serif

      _pFonts[0] = new qPBReaderFontSelectorWidget(tr("Serif"), pVert);
      QString sSerif = qPBReaderConfig::GetDefaultSerifFont();
      _pFonts[0]->_pFontComboBox->setCurrentFont(QFont(sSerif));
      pVert->AddWidget(_pFonts[0]);

      // sans

      _pFonts[1] = new qPBReaderFontSelectorWidget(tr("Sans-serif"), pVert);
      QString sSans = qPBReaderConfig::GetDefaultSansFont();
      _pFonts[1]->_pFontComboBox->setCurrentFont(QFont(sSans));
      pVert->AddWidget(_pFonts[1]);

      // mono

      _pFonts[2] = new qPBReaderFontSelectorWidget(tr("Monospace"), pVert);
      QString sMono = qPBReaderConfig::GetDefaultMonoFont();
      _pFonts[2]->_pFontComboBox->setCurrentFont(QFont(sMono));
      pVert->AddWidget(_pFonts[2]);

      // std

      _pType = new qPBReaderTypeSelectorWidget(tr("Standard"), pVert);
      int idx = qPBReaderConfig::GetDefaultStandardFont();
      _pType->_pComboBox->setCurrentIndex(idx);
      pVert->AddWidget(_pType);



      QObject::connect(_pIgnoreFontsCheckBox, SIGNAL(stateChanged(int)),
                       this                 , SLOT(IgnoreFontsChanged(int)));

      QObject::connect(_pFonts[0]->_pFontComboBox,
                       SIGNAL(currentFontChanged(const QFont &)),
                       this, SLOT(SelectionChanged()));
      QObject::connect(_pFonts[1]->_pFontComboBox,
                       SIGNAL(currentFontChanged(const QFont &)),
                       this, SLOT(SelectionChanged()));
      QObject::connect(_pFonts[2]->_pFontComboBox,
                       SIGNAL(currentFontChanged(const QFont &)),
                       this, SLOT(SelectionChanged()));
      QObject::connect(_pType->_pComboBox, SIGNAL(currentIndexChanged(int)),
                       this, SLOT(SelectionChanged()));
   }


    QObject::connect(this, SIGNAL(finished(int)), QCoreApplication::instance(), SLOT(quit()));

}

// -----------------------------------------------------------------------------

qPBReaderConfigDialog::~qPBReaderConfigDialog()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::~qPBReaderConfigDialog");
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::ApplicationLanguageChanged(const QString & isLang)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::ApplicationLanguageChanged");

   TRACE << VAR(isLang) << endl;

   qPBReaderConfig::SetApplicationLanguage(isLang);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::MagnificationStepPlus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::MagnificationStepPlus");

   double dMagVal = qPBReaderConfig::GetDefaultMagnificationFactor() + 0.1;

   TRACE << VAR(dMagVal) << endl;

   _pZoom->_pZoomWidget->SetValue(dMagVal);
   qPBReaderConfig::SetDefaultMagnificationFactor(dMagVal);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::MagnificationStepMinus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::MagnificationStepMinus");

   double dMagVal = qPBReaderConfig::GetDefaultMagnificationFactor();
   if (dMagVal > 0.1)
   {
      dMagVal -= 0.1;

      TRACE << VAR(dMagVal) << endl;

      _pZoom->_pZoomWidget->SetValue(dMagVal);
      qPBReaderConfig::SetDefaultMagnificationFactor(dMagVal);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::UseBookMarginChanged(int state)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::UseBookMarginChanged");

   bool bUseBookMargin = state == Qt::Checked;
   TRACE << VAR(bUseBookMargin) << endl;
   _pTop->_pSetValueWidget->setEnabled(!bUseBookMargin);
   _pSide->_pSetValueWidget->setEnabled(!bUseBookMargin);
   _pBottom->_pSetValueWidget->setEnabled(!bUseBookMargin);

   qPBReaderConfig::SetDefaultUseBookMargin(bUseBookMargin);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::TopMarginMinus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::TopMarginMinus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   top -= 2;
   TRACE << VAR(top) << endl;
   _pTop->_pSetValueWidget->SetText(QString::number(top));
   qPBReaderConfig::SetDefaultTopMargin(top);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::TopMarginPlus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::TopMarginPlus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   top += 2;
   TRACE << VAR(top) << endl;
   _pTop->_pSetValueWidget->SetText(QString::number(top));
   qPBReaderConfig::SetDefaultTopMargin(top);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::SideMarginMinus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::SideMarginMinus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   side -= 2;
   TRACE << VAR(side) << endl;
   _pSide->_pSetValueWidget->SetText(QString::number(side));
   qPBReaderConfig::SetDefaultSideMargin(side);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::SideMarginPlus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::SideMarginPlus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   side += 2;
   TRACE << VAR(side) << endl;
   _pSide->_pSetValueWidget->SetText(QString::number(side));
   qPBReaderConfig::SetDefaultSideMargin(side);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::BottomMarginMinus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::BottomMarginMinus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   bottom -= 2;
   TRACE << VAR(bottom) << endl;
   _pBottom->_pSetValueWidget->SetText(QString::number(bottom));
   qPBReaderConfig::SetDefaultBottomMargin(bottom);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::BottomMarginPlus()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::BottomMarginPlus");

   int top, side, bottom;
   qPBReaderConfig::GetDefaultMargins(top, side, bottom);
   bottom += 2;
   TRACE << VAR(bottom) << endl;
   _pBottom->_pSetValueWidget->SetText(QString::number(bottom));
   qPBReaderConfig::SetDefaultBottomMargin(bottom);
}


// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::UseHyphensChanged(int state)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::UseHyphensChanged");

   bool bUseHyphens = state == Qt::Checked;

   TRACE << VAR(bUseHyphens) << endl;

   _pHyphenDic->_pComboBox->setEnabled(bUseHyphens);

   QString sHyphenPatt;
   if (bUseHyphens)
   {
      sHyphenPatt = _pHyphenDic->_pComboBox->currentText();
   }
   HyphenDicChanged(sHyphenPatt);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::HyphenDicChanged(const QString & isLang)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::HyphenDicChanged");

   TRACE << VAR(isLang) << endl;

   qPBReaderConfig::SetDefaultHyphenPatternLang(isLang);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::IgnoreFontsChanged(int state)
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::IgnoreFontsChanged");

   bool bIgnoreFonts = state == Qt::Checked;
   TRACE << VAR(bIgnoreFonts) << endl;

   qPBReaderConfig::SetDefaultIgnoreFonts(bIgnoreFonts);
}

// -----------------------------------------------------------------------------

void qPBReaderConfigDialog::SelectionChanged()
{
   TRSCOPE(cfg, "qPBReaderConfigDialog::SelectionChanged");

   qPBReaderConfig::SetDefaultStandardFont(_pType->_pComboBox->currentIndex());
   qPBReaderConfig::SetDefaultSerifFont(_pFonts[0]->_pFontComboBox->currentFont().family());
   qPBReaderConfig::SetDefaultSansFont(_pFonts[1]->_pFontComboBox->currentFont().family());
   qPBReaderConfig::SetDefaultMonoFont(_pFonts[2]->_pFontComboBox->currentFont().family());
}

// -----------------------------------------------------------------------------
