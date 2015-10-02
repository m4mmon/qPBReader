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

#ifndef _qPBReaderDoc_h__
#define _qPBReaderDoc_h__

#include <QTemporaryFile>
#include <QVariant>
#include <QWebPage>
#include "qPBReaderPagePos.h"

class qPBReaderJSLoader;

class qPBReaderDoc : public QWebPage
{
      Q_OBJECT

      Q_PROPERTY(QVariant _pass_json_value READ _pass_json_value_getter WRITE
                 _pass_json_value_setter)
      Q_PROPERTY(bool hyphenatable READ hyphenatable)

   public :
      qPBReaderDoc(QObject * ipParent = 0);
      ~qPBReaderDoc();


      void LoadJSLibraries();
      void AfterLoad();
      void ProcessFonts();
      QList<int> ColumnBoundaries();
      void ScrollBy(int dx, int dy);
      void ScrollTo(int x, int y);
      void JumpToAnchor(const QString & is);
      bool AtBottom();
      bool AtTop();
      int ypos();
      int window_height();
      int window_width();
      int xpos();
      double GetScrollFraction();
      void SetScrollFraction(double val);
      double hscrollFraction();
      int height();
      int width();
      void SetBottomPadding(int val);
      QString ExtractNode();

      QVariant EvalJS(const QString & iSrc);
      int EvalJSInt(const QString & iSrc);
      double EvalJSDouble(const QString & iSrc);
      float EvalJSFloat(const QString & iSrc);
      qreal EvalJSReal(const QString & iSrc);
      QString EvalJSString(const QString & iSrc);
      bool EvalJSBool(const QString & iSrc);
      void SavePagePos();
      void RestorePagePos();
      void UpdateContentsSizeForPagedMode();

      inline bool GetUseBookMargin() const;
      inline void SetUseBookMargin(bool ibUseBookMargin);
      inline void GetMargins(int & oTop, int & oSide, int & oBottom) const;
      inline int GetTopMargin() const;
      inline int GetSideMargin() const;
      inline int GetBottomMargin() const;
      inline void SetMargins(int iTop, int iSide, int iBottom);
      inline void SetTopMargin(int iTop);
      inline void SetSideMargin(int iSide);
      inline void SetBottomMargin(int iBottom);

      void SetFonts(int i,
                    const QString & isSerifFamily,
                    const QString & isSansFamily,
                    const QString & isMonoFamily);

      void GetFonts(QString & osStandardFamily,
                    QString & osSerifFamily,
                    QString & osSansFamily,
                    QString & osMonoFamily);

      int GetCurrentPage();

      inline qPBReaderPagePos * GetPagePos();

      inline void SetHyphenPatternLang(const QString & isLang);
      inline QString GetHyphenPatternLang();

      inline void SetIgnoreFonts(bool bIgnoreFonts);
      inline bool GetIgnoreFonts();
      

   public slots :

      void AddWindowObjects();
      void debug(const QString & iMsg);
      QString title()
      {
         return "title!";
      }
      QString author()
      {
         return "author!";
      }
      QString section()
      {
         return "section!";
      }
      void init_hyphenate() {}

   protected :

      void javaScriptConsoleMessage(const QString & message, int lineNumber,
                                    const QString & sourceID);

   signals :

      void mark_element(QWebElement *);

   private :


      QVariant _pass_json_value_getter();
      void _pass_json_value_setter(const QVariant & is);
      bool hyphenatable()
      {
         return false;
      }

      void SwitchToPagedMode();
      void Hyphenate();

      int GetNumberOfStyleSheets();
      int GetNumberOfStyleSheetRules(int inStyleSheetNum);
      bool IsA(int inStyleSheetNum, int inRuleNum, const QString & isType);
      bool IsFontFaceRule(int inStyleSheetNum, int inRuleNum);
      bool IsStyleRule(int inStyleSheetNum, int inRuleNum);
      QString GetRuleStylePropertyValue(int inStyleSheetNum,
                                        int inRuleNum,
                                        const QString & isProp);
      int GetRuleStylePropertySize(int inStyleSheetNum,
                                   int inRuleNum,
                                   const QString & isProp);
      QString GetRuleStylePropertyValue(int inStyleSheetNum,
                                        int inRuleNum,
                                        int inValueNum,
                                        const QString & isProp);
      void SetCssStyleProperty(int inStyleSheetNum,
                               int inRuleNum,
                               const QString & isProp,
                               const QString & isVal);
      void RemoveCssStyleProperty(int inStyleSheetNum,
                                  int inRuleNum,
                                  const QString & isProp);
      void RemoveCssRule(int inStyleSheetNum, int inRuleNum);
      void BigUglyCSSFontFaceHack();

      qPBReaderJSLoader * _pLoader;
      bool _bJSLoaded;
      qPBReaderPagePos _pp;
      QVariant bridge_value;
      bool _bUseBookMargin;
      int _marginTop;
      int _marginSide;
      int _marginBottom;
      QTemporaryFile _fUserStyleSheet;
      QString _sHyphenLang;
      bool _bIgnoreFonts;

      static QStringList _lsAddedFonts;
      static QList<int> _lnAddedFontsId;
};

// -----------------------------------------------------------------------------

bool qPBReaderDoc::GetUseBookMargin() const
{
   return _bUseBookMargin;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetUseBookMargin(bool ibUseBookMargin)
{
   _bUseBookMargin = ibUseBookMargin;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::GetMargins(int & oTop, int & oSide, int & oBottom) const
{
   oTop = GetTopMargin();
   oSide = GetSideMargin();
   oBottom = GetBottomMargin();
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetTopMargin() const
{
   return _marginTop;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetSideMargin() const
{
   return _marginSide;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetBottomMargin() const
{
   return _marginBottom;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetMargins(int iTop, int iSide, int iBottom)
{
   SetTopMargin(iTop);
   SetSideMargin(iSide);
   SetBottomMargin(iBottom);
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetTopMargin(int iTop)
{
   _marginTop = iTop;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetSideMargin(int iSide)
{
   _marginSide = iSide;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetBottomMargin(int iBottom)
{
   _marginBottom = iBottom;
}

// -----------------------------------------------------------------------------

qPBReaderPagePos * qPBReaderDoc::GetPagePos()
{
   return &_pp;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetHyphenPatternLang(const QString & isLang)
{
   _sHyphenLang = isLang;
}

// -----------------------------------------------------------------------------

QString qPBReaderDoc::GetHyphenPatternLang()
{
   return _sHyphenLang;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetIgnoreFonts(bool bIgnoreFonts)
{
   _bIgnoreFonts = bIgnoreFonts;
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::GetIgnoreFonts()
{
   return _bIgnoreFonts;
}

// -----------------------------------------------------------------------------

#endif
