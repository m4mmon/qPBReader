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

#ifndef _qPBReaderViewer_h__
#define _qPBReaderViewer_h__

#include <QMainWindow>
#include <QThread>

#include "qPBReaderEpub.h"

class qPBReaderDocView;
class qPBReaderGoBackDlg;
class qPBReaderSettingsDialog;
class qPBReaderTextLayoutSettingsDialog;
class QGesture;
class QGridLayout;
class QPinchGesture;
class QTapAndHoldGesture;
class QTapGesture;
class QUrl;


class qPBReaderViewer : public QMainWindow
{
      Q_OBJECT

   public :
      qPBReaderViewer(QString & isEpub);
      ~qPBReaderViewer();
      bool Start();
      void PreviousSection();
      void NextSection();
      void Scrolled(double val);
      void LoadFinished(bool ok);
      void UseBookMarginChanged(bool bUseBookMargin);
      void FontsChanged(const QString & isStandardFamily,
                        const QString & isSerifFamily,
                        const QString & isSansFamily,
                        const QString & isMonoFamily);
      void HyphenLangChanged(const QString & isHyphenPatternLang);
      void IgnoreFontsChanged(bool ibIgnoreFonts);
      void UpdatePageDisplay();
#ifdef Q_OS_POCKETBOOK
      void InkViewEventHandler(bool main, int type, int parm1, int parm2);
#endif
      void Dictionary(QPoint iPt);

   public slots :

      void MagnificationChanged(double val);
      void ForcedMarginChanged(int iTop, int iSide, int iBottom);
      void TextLayout();
      void FontOptions();
      void Orientation();
      void ComputePagesCount();
      void TableOfContents();
      void Exit();
      void BackFromLink();

   protected :

      bool event(QEvent * pEvent);

   private slots :

      void LinkClicked(const QUrl & iUrl);
      void Refresh();
      void PagesCounted(QList<int>);

   private :
      qPBReaderViewer();
      void Settings();
      void SetLocation(const QString & isPath, double pos = 0.);
      void Reload();
      void RestoreState();
      void keyPressEvent(QKeyEvent * iEvt);
      void ResetPagesCount();
      void RecalcPagesCountFromSettings();
      void SaveState();
      bool OnGesture(QList<QGesture *> & ilpGest);
      bool OnPinchGesture(QPinchGesture * ipGest);
      bool OnTapGesture(QTapGesture * ipGest);
      bool OnTapAndHoldGesture(QTapAndHoldGesture * ipGest);

      qPBReaderDocView * _pView;
      qPBReaderSettingsDialog * _pSettingsDlg;
      qPBReaderTextLayoutSettingsDialog * _pTextLayoutDlg;
      qPBReaderGoBackDlg * _pBackDlg;
      qPBReaderEpub _book;

      bool _bDisplayingToc;
      QString _sPendingAnchor;

      QString _sPrevFile;
      double _dPrevPos;

      QList<int> _lnPagesCount;
      int _nTotal;
      int _nDelta;
      int _nPageInSection;
      bool _bCounting;
      bool _bLoading;

      QThread _pageCounterThread;

   signals :
      void CountPages();
      void InterruptPagesCount();
};

#endif
