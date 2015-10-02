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

#include "qPBReaderFontSettingsDialog.h"

// qt
#include <QBoxLayout>
#include <QCheckBox>
#include <QFontComboBox>
#include <QLabel>

// project
#include "qPBReaderDocView.h"
#include "qPBReaderFontSelectorWidget.h"
#include "qPBReaderSetValueWidget.h"
#include "qPBReaderTracer.h"
#include "qPBReaderTypeSelectorWidget.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderViewer.h"

// -----------------------------------------------------------------------------

qPBReaderFontSettingsDialog::qPBReaderFontSettingsDialog(
   qPBReaderDocView * ipView,
   qPBReaderViewer * parent
) :
   qPBReaderDialog(parent, tr("Font options")),
   _pView(ipView)
{
   TRSCOPE(viewer, "qPBReaderFontSettingsDialog::qPBReaderFontSettingsDialog");


   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);
   addWidget(pVert);

   _pIgnoreFontsCheckBox = new QCheckBox(tr("Ignore book fonts"), pVert);
   pVert->AddWidget(_pIgnoreFontsCheckBox);

   _pFonts[0] = new qPBReaderFontSelectorWidget(tr("Serif"), pVert);
   _pFonts[1] = new qPBReaderFontSelectorWidget(tr("Sans-serif"), pVert);
   _pFonts[2] = new qPBReaderFontSelectorWidget(tr("Monospace"), pVert);
   _pType = new qPBReaderTypeSelectorWidget(tr("Standard"), pVert);

   pVert->AddWidget(_pFonts[0]);
   pVert->AddWidget(_pFonts[1]);
   pVert->AddWidget(_pFonts[2]);
   pVert->AddWidget(_pType);


   // and plug stuff

   QObject::connect(_pIgnoreFontsCheckBox, SIGNAL(stateChanged(int)),
                    _pView               , SLOT(IgnoreFontsChanged(int)));

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

// -----------------------------------------------------------------------------

qPBReaderFontSettingsDialog::~qPBReaderFontSettingsDialog()
{
   TRSCOPE(viewer, "qPBReaderFontSettingsDialog::~qPBReaderFontSettingsDialog");
}

// -----------------------------------------------------------------------------

int qPBReaderFontSettingsDialog::exec()
{
   TRSCOPE(viewer, "qPBReaderFontSettingsDialog::exec");

   // don't send notifications

   _pIgnoreFontsCheckBox->blockSignals(true);
   _pType->_pComboBox->blockSignals(true);
   _pFonts[0]->_pFontComboBox->blockSignals(true);
   _pFonts[1]->_pFontComboBox->blockSignals(true);
   _pFonts[2]->_pFontComboBox->blockSignals(true);

   // read what's going on in user interface

   bool bIgnoreFonts = _pView->GetIgnoreFonts();
   Qt::CheckState state = bIgnoreFonts ? Qt::Checked : Qt::Unchecked;
   _pIgnoreFontsCheckBox->setCheckState(state);
   TRACE << VAR(bIgnoreFonts) << endl;


   QString sStd = _pView->page()->settings()->fontFamily(
                     QWebSettings::StandardFont);
   QString sSerif = _pView->page()->settings()->fontFamily(
                       QWebSettings::SerifFont);
   QString sSans = _pView->page()->settings()->fontFamily(
                      QWebSettings::SansSerifFont);
   QString sMono = _pView->page()->settings()->fontFamily(QWebSettings::FixedFont);

   TRACE << "current std font   " << sStd << endl;
   TRACE << "current serif font " << sSerif << endl;
   TRACE << "current sans font  " << sSans << endl;
   TRACE << "current mono font  " << sMono << endl;

   // set standard combo, default to serif

   if (sStd == sSerif)
   {
      _pType->_pComboBox->setCurrentIndex(0);
   }

   else if (sStd == sSans)
   {
      _pType->_pComboBox->setCurrentIndex(1);
   }

   else if (sStd == sMono)
   {
      _pType->_pComboBox->setCurrentIndex(2);
   }

   _pFonts[0]->_pFontComboBox->setCurrentFont(QFont(sSerif));
   _pFonts[1]->_pFontComboBox->setCurrentFont(QFont(sSans));
   _pFonts[2]->_pFontComboBox->setCurrentFont(QFont(sMono));

   // restore

   _pIgnoreFontsCheckBox->blockSignals(false);
   _pType->_pComboBox->blockSignals(false);
   _pFonts[0]->_pFontComboBox->blockSignals(false);
   _pFonts[1]->_pFontComboBox->blockSignals(false);
   _pFonts[2]->_pFontComboBox->blockSignals(false);

   return QDialog::exec();
}

// -----------------------------------------------------------------------------

void qPBReaderFontSettingsDialog::SelectionChanged()
{
   TRSCOPE(viewer, "qPBReaderFontSettingsDialog::SelectionChanged");

   _pView->UpdateFonts(_pType->_pComboBox->currentIndex(),
                       _pFonts[0]->_pFontComboBox->currentFont().family(),
                       _pFonts[1]->_pFontComboBox->currentFont().family(),
                       _pFonts[2]->_pFontComboBox->currentFont().family());
}

// -----------------------------------------------------------------------------
