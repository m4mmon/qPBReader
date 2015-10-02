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

#include "qPBReaderTextLayoutSettingsDialog.h"

// qt
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

// project
#include "qPBReaderComboBox.h"
#include "qPBReaderDocView.h"
#include "qPBReaderLangLists.h"
#include "qPBReaderSetValueWidget.h"
#include "qPBReaderTracer.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderViewer.h"
#include "qPBReaderZoomWidget.h"

// -----------------------------------------------------------------------------

qPBReaderTextLayoutSettingsDialog::qPBReaderTextLayoutSettingsDialog(
   qPBReaderDocView * ipView, qPBReaderViewer * parent
) :
   qPBReaderDialog(parent, tr("Text layout")),
   _pView(ipView),
   _pParent(parent)
{
   TRSCOPE(viewer,
            "qPBReaderTextLayoutSettingsDialog::qPBReaderTextLayoutSettingsDialog");

   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);

   _pZoomWidget = new qPBReaderZoomWidget(pVert);
   pVert->AddWidget(_pZoomWidget, false);

   _pUseBookMarginCheckBox = new QCheckBox(tr("Use book margin"), pVert);
   pVert->AddWidget(_pUseBookMarginCheckBox);

   _pTopMarginValueWidget = new qPBReaderSetValueWidget(pVert);
   pVert->AddWidget(_pTopMarginValueWidget);

   _pSideMarginValueWidget = new qPBReaderSetValueWidget(pVert);
   pVert->AddWidget(_pSideMarginValueWidget);

   _pBottomMarginValueWidget = new qPBReaderSetValueWidget(pVert);
   pVert->AddWidget(_pBottomMarginValueWidget);

   _pDefaultMarginButton = new QPushButton(pVert);
   _pDefaultMarginButton->setText(tr("Default margin values"));
   pVert->AddWidget(_pDefaultMarginButton);

   _pUseHyphens = new QCheckBox(tr("Hyphenation"), pVert);
   pVert->AddWidget(_pUseHyphens);
   
   _pHyphenDic = new qPBReaderComboBox(tr("Language"), this);
   _pHyphenDic->_pComboBox->addItems(qPBReaderLangLists::GetAvailableHyphenPatterns());
   pVert->AddWidget(_pHyphenDic);
   

   addWidget(pVert);

   // and plug stuff

   // connect minus/plus signals to shrink magnify view slots

   QObject::connect(_pZoomWidget  , SIGNAL(Minus()), _pView, SLOT(ShrinkFonts()));
   QObject::connect(_pZoomWidget  , SIGNAL(Plus()),  _pView, SLOT(MagnifyFonts()));
   QObject::connect(_pView, SIGNAL(MagnificationChanged(double)),  _pZoomWidget, SLOT(SetValue(double)));

   QObject::connect(_pUseBookMarginCheckBox, SIGNAL(stateChanged(int)),
                    this, SLOT(CheckBoxStateChanged(int)));

   QObject::connect(_pUseBookMarginCheckBox, SIGNAL(stateChanged(int)),
                    _pView                 , SLOT(BookMarginChanged(int)));

   QObject::connect(_pTopMarginValueWidget, SIGNAL(Minus()),
                    _pView                , SLOT(TopMarginMinus()));
   QObject::connect(_pTopMarginValueWidget, SIGNAL(Plus()),
                    _pView                , SLOT(TopMarginPlus()));

   QObject::connect(_pSideMarginValueWidget, SIGNAL(Minus()),
                    _pView                 , SLOT(SideMarginMinus()));
   QObject::connect(_pSideMarginValueWidget, SIGNAL(Plus()),
                    _pView                 , SLOT(SideMarginPlus()));

   QObject::connect(_pBottomMarginValueWidget, SIGNAL(Minus()),
                    _pView                   , SLOT(BottomMarginMinus()));
   QObject::connect(_pBottomMarginValueWidget, SIGNAL(Plus()),
                    _pView                   , SLOT(BottomMarginPlus()));

   QObject::connect(_pDefaultMarginButton, SIGNAL(clicked(bool)),
                    _pView               , SLOT(DefaultMarginValues()));


   QObject::connect(_pUseHyphens, SIGNAL(stateChanged(int)),
                    this        , SLOT(UseHyphensChanged(int)));

   QObject::connect(_pHyphenDic->_pComboBox,
                    SIGNAL(currentIndexChanged(const QString &)),
                    this, SLOT(HyphenDicChanged(const QString &)));

}

// -----------------------------------------------------------------------------

qPBReaderTextLayoutSettingsDialog::~qPBReaderTextLayoutSettingsDialog()
{
   TRSCOPE(viewer,
            "qPBReaderTextLayoutSettingsDialog::~qPBReaderTextLayoutSettingsDialog");
   _pView = 0;
   _pParent = 0;
}

// -----------------------------------------------------------------------------

void qPBReaderTextLayoutSettingsDialog::SetMarginValues(int top,
      int side,
      int bottom)
{
   _pTopMarginValueWidget->SetText(
      tr("Top") + "\n" + QString::number(top)
   );
   _pSideMarginValueWidget->SetText(
      tr("Side") + "\n" + QString::number(side)
   );
   _pBottomMarginValueWidget->SetText(
      tr("Bottom") + "\n" + QString::number(bottom)
   );
}

// -----------------------------------------------------------------------------

int qPBReaderTextLayoutSettingsDialog::exec()
{
   TRSCOPE(viewer, "qPBReaderTextLayoutSettingsDialog::exec");

   // set current zoom value

   _pZoomWidget->SetValue(_pView->GetMultiplier());

   // properly setup margins setters

   int top = 0, side = 0, bottom = 0;
   _pView->GetForcedMargins(top, side, bottom);
   SetMarginValues(top, side, bottom);

   // get internal book margin status

   Qt::CheckState state = _pView->UseBookMargin() ? Qt::Checked : Qt::Unchecked;
   _pUseBookMarginCheckBox->setCheckState(state);

   _pUseHyphens->blockSignals(true);
   _pHyphenDic->_pComboBox->blockSignals(true);

   _pUseHyphens->setCheckState(Qt::Unchecked);
   QString sHyphenPatt = _pView->GetHyphenPatternLang();
   TRACE << "Current pattern lang" << ENC(sHyphenPatt);
   _pHyphenDic->_pComboBox->setCurrentIndex(14); // fr
   _pHyphenDic->setEnabled(false);
   if (!sHyphenPatt.isEmpty())
   {
      _pUseHyphens->setCheckState(Qt::Checked);
      int nPos = qPBReaderLangLists::GetAvailableHyphenPatterns().indexOf(sHyphenPatt);
      TRACE << "pos in list is " << nPos << endl;
      if (nPos >= 0)
      {
         _pHyphenDic->_pComboBox->setCurrentIndex(nPos);
      }
      _pHyphenDic->setEnabled(true);
   }

   _pUseHyphens->blockSignals(false);
   _pHyphenDic->_pComboBox->blockSignals(false);

   return QDialog::exec();
}

// -----------------------------------------------------------------------------

void qPBReaderTextLayoutSettingsDialog::CheckBoxStateChanged(int state)
{
   TRSCOPE(viewer, "qPBReaderTextLayoutSettingsDialog::CheckBoxStateChanged");

   bool bActive = state == Qt::Unchecked;

   TRACE << "Setters active ? " << bActive << endl;

   _pTopMarginValueWidget->setEnabled(bActive);
   _pSideMarginValueWidget->setEnabled(bActive);
   _pBottomMarginValueWidget->setEnabled(bActive);
   _pDefaultMarginButton->setEnabled(bActive);
}

// -----------------------------------------------------------------------------

void qPBReaderTextLayoutSettingsDialog::UseHyphensChanged(int state)
{
   TRSCOPE(viewer, "qPBReaderTextLayoutSettingsDialog::UseHyphensChanged");

   bool bActive = state == Qt::Checked;

   TRACE << "Setters active ? " << bActive << endl;

   _pHyphenDic->setEnabled(bActive);

   QString sHyphenPatt;
   if (bActive)
   {
      sHyphenPatt = _pHyphenDic->_pComboBox->currentText();
   }
   HyphenDicChanged(sHyphenPatt);
}

// -----------------------------------------------------------------------------

void qPBReaderTextLayoutSettingsDialog::HyphenDicChanged(const QString & isHyphenPatternLang)
{
   _pView->UseHyphensChanged(isHyphenPatternLang);
}

// -----------------------------------------------------------------------------
