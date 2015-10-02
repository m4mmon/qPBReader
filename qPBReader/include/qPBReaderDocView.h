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

#ifndef _qPBReaderDocView_h__
#define _qPBReaderDocView_h__

#include <QtWebKit>

class qPBReaderDoc;
class qPBReaderPagePos;
class qPBReaderViewer;

class qPBReaderDocView : public QWebView
{
      Q_OBJECT

   public :
      qPBReaderDocView(qPBReaderViewer * ipMgr);
      ~qPBReaderDocView();

      void InitializeView();

      inline void SetInitialPos(double idPos);

      void Load(const QString & isPath, double dPos);

      QSize sizeHint() const
      {
         return _sizeHint;
      }

      double GetMultiplier();
      void SetMultiplier(double val);
      void ScrollTo(QVariant pos, bool bNotify);
      bool UseBookMargin() const;
      void SetUseBookMargin(bool bUseBookMargin);
      void GetForcedMargins(int & oTop, int & oSide, int & oBottom);
      void SetForcedMargins(int iTop, int iSide, int iBottom);
      void SetFonts(int i,
                    const QString & isSerifFamily,
                    const QString & isSansFamily,
                    const QString & isMonoFamily);
      bool GetIgnoreFonts();
      void SetIgnoreFonts(bool ibIgnoreFonts);

      void AfterLoad();

      int GetCurrentFilePageNumber();

      bool GetWordAt(QPoint iScreenPt,
                     QString & osSelectedWord,
                     QRect & oScreenRect);

      inline void SetInDico(bool ibInDico);
      void SetViewZoom(double d);

      void SetHyphenPatternLang(const QString & isLang);
      QString GetHyphenPatternLang();

      qPBReaderPagePos * GetPagePos();

   public slots :

      void PreviousPage();
      void NextPage();

      void MagnifyFonts(double step = 0.1);
      void ShrinkFonts(double step = 0.1);

      void BookMarginChanged(int state);
      void IgnoreFontsChanged(int state);
      void UseHyphensChanged(const QString & isHyphenPatternLang);
      void TopMarginMinus();
      void TopMarginPlus();
      void SideMarginMinus();
      void SideMarginPlus();
      void BottomMarginMinus();
      void BottomMarginPlus();
      void DefaultMarginValues();
      void UpdateFonts(int i,
                       const QString & isSerifFamily,
                       const QString & isSansFamily,
                       const QString & isMonoFamily);

   signals:

      void MagnificationChanged(double);
      void ForcedMarginChanged(int, int, int);

   protected :
public:
      double GetScrollFraction();
      void SetScrollFraction(double val);
//      bool event(QEvent * pEvent);

   protected slots :

      void LoadFinished(bool ok);

   private :

      void ForcedMarginChanged();

      QSize _sizeHint;
      double _initialPos;
      bool _toBottom;
      qPBReaderDoc * _pDoc;
      qPBReaderViewer * _pMgr;
      double _dScrollFraction;
      bool _bInDico;
};

void qPBReaderDocView::SetInitialPos(double idPos)
{
   _initialPos = idPos;
}

void qPBReaderDocView::SetInDico(bool ibInDico)
{
   _bInDico = ibInDico;
}

#endif
