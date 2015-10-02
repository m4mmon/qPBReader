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

#include "qPBReaderDocView.h"

// qt
#include <QGesture>

// project
#include "qPBReaderConfig.h"
#include "qPBReaderDoc.h"
#include "qPBReaderPlatform.h"
#include "qPBReaderTracer.h"
#include "qPBReaderViewer.h"

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderDocView::qPBReaderDocView(qPBReaderViewer * ipMgr) :
   QWebView(ipMgr),
   _pDoc(0),
   _pMgr(ipMgr),
   _bInDico(false)
{
   TRSCOPE(view, "qPBReaderDocView::qPBReaderDocView");
}

// -----------------------------------------------------------------------------

qPBReaderDocView::~qPBReaderDocView()
{
   TRSCOPE(view, "qPBReaderDocView::~qPBReaderDocView");
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::InitializeView()
{
   TRSCOPE(view, "qPBReaderDocView::InitializeView");

   setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                  QPainter::SmoothPixmapTransform);
   setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

   _sizeHint = QSize(510, 680);
   _initialPos = 0.;
   _toBottom = false;

   _pDoc = new qPBReaderDoc(this);

   setPage(_pDoc);

   // signal handling

   connect(this, SIGNAL(loadFinished(bool)),
           this, SLOT(LoadFinished(bool)));
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::Load(const QString & isPath, double dPos)
{
   TRSCOPE(view, "qPBReaderDocView::Load");

   qPBReaderPlatform::ShowHourGlass();

   _initialPos = dPos;

   load(QUrl(isPath));
   setFocus();
}

// -----------------------------------------------------------------------------

double qPBReaderDocView::GetMultiplier()
{
   return zoomFactor();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetMultiplier(double val)
{
   TRSCOPE(view, "qPBReaderDocView::SetMultiplier");
   double oldVal = GetMultiplier();
   setZoomFactor(val);

   if (val != oldVal)
   {
      qPBReaderPlatform::ShowHourGlass();
      _pDoc->UpdateContentsSizeForPagedMode();
      emit MagnificationChanged(val);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::ScrollTo(QVariant pos, bool bNotify)
{
   TRSCOPE(view, "qPBReaderDocView::ScrollTo");

   // current position

   int oldPos = _pDoc->xpos();

   // anchor

   QVariant::Type type = pos.type();

   if (type == QVariant::String)
   {
      QString sAnchor = pos.toString();
      TRACE << "Jump to anchor " << ENC(sAnchor) << endl;
      _pDoc->JumpToAnchor(sAnchor);
   }

   // position value

   else if (type == QVariant::Double)
   {
      double d = pos.toDouble();
      TRACE << "Jump to pos " << d << endl;
      _pDoc->SetScrollFraction(d);
   }

   // new position

   int newPos = _pDoc->xpos();

   TRACE << VAR(oldPos) << VAR(newPos) << endl;

   // notify if required and needed

   if (bNotify && oldPos != newPos)
   {
      _pMgr->Scrolled(GetScrollFraction());
   }
}

// -----------------------------------------------------------------------------

bool qPBReaderDocView::UseBookMargin() const
{
   return _pDoc->GetUseBookMargin();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetUseBookMargin(bool bUseBookMargin)
{
   _pDoc->SetUseBookMargin(bUseBookMargin);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::GetForcedMargins(int & oTop, int & oSide, int & oBottom)
{
   _pDoc->GetMargins(oTop, oSide, oBottom);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetForcedMargins(int iTop, int iSide, int iBottom)
{
   _pDoc->SetMargins(iTop, iSide, iBottom);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetFonts(int i,
                                const QString & isSerifFamily,
                                const QString & isSansFamily,
                                const QString & isMonoFamily)
{
   _pDoc->SetFonts(i, isSerifFamily, isSansFamily, isMonoFamily);
}

// -----------------------------------------------------------------------------

bool qPBReaderDocView::GetIgnoreFonts()
{
   return _pDoc->GetIgnoreFonts();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetIgnoreFonts(bool ibIgnoreFonts)
{
   _pDoc->SetIgnoreFonts(ibIgnoreFonts);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::AfterLoad()
{
   _pDoc->AfterLoad();
}

// -----------------------------------------------------------------------------

int qPBReaderDocView::GetCurrentFilePageNumber()
{
   return _pDoc->GetCurrentPage();
}

// -----------------------------------------------------------------------------

bool qPBReaderDocView::GetWordAt(QPoint iScreenPt,
                                 QString & osSelectedWord,
                                 QRect & oScreenRect)
{
   TRSCOPE(view, "qPBReaderDocView::GetWordAt");

   osSelectedWord.clear();
   oScreenRect = QRect();

   QWebHitTestResult hit = _pDoc->mainFrame()->hitTestContent(iScreenPt);
   QWebElement e = hit.enclosingBlockElement();
   TRACE << ENC(e.localName()) << endl;
   QString js = "qPBReaderFindClickedWord(this, " + QString::number(
                   iScreenPt.x()) + "," + QString::number(iScreenPt.y()) + ")";
   TRACE << ENC(js) << endl;

   QList<QVariant> lv = e.evaluateJavaScript(js).toList();
   bool b = lv.size() == 5;
   TRACE << "Javascript word search = " << b << endl;

   if (b)
   {
      osSelectedWord = lv[0].toString();
      oScreenRect = QRect(QPoint(lv[1].toInt(), lv[2].toInt()), QPoint(lv[3].toInt(),
                          lv[4].toInt()));
      TRACE << ENC(osSelectedWord)
            << " rect (" << oScreenRect.left() << ", "
            << oScreenRect.top() << ", "
            << oScreenRect.right() << ", "
            << oScreenRect.bottom() << ")" << endl;
   }

   TRACE << TRANAME << " ended with " << b << endl;

   return b;
}

// -----------------------------------------------------------------------------
//  protected
// -----------------------------------------------------------------------------

double qPBReaderDocView::GetScrollFraction()
{
   return _pDoc->GetScrollFraction();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetScrollFraction(double val)
{
   _pDoc->SetScrollFraction(val);
}

// -----------------------------------------------------------------------------
//  slots
// -----------------------------------------------------------------------------

void qPBReaderDocView::LoadFinished(bool ok)
{
   TRSCOPE(view, "qPBReaderDocView::LoadFinished");

   TRACE << "ok: " << ok << endl;


   _pDoc->ProcessFonts();
   _pDoc->LoadJSLibraries();
   _pDoc->AfterLoad();

   _sizeHint = _pDoc->mainFrame()->contentsSize();

   bool bScrolled = false;

   if (_toBottom)
   {
      _toBottom = false;
      _initialPos = 1.;
   }

   if (_initialPos > 0.)
   {
      bScrolled = true;
   }

   TRACE << VAR(_initialPos) << endl;

   ScrollTo(_initialPos, false);

   _initialPos = 0.;

   update();

   _pMgr->LoadFinished(ok);

   if (bScrolled)
   {
      _pMgr->Scrolled(_pDoc->GetScrollFraction());
   }

   qPBReaderPlatform::HideHourGlass();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::PreviousPage()
{
   TRSCOPE(view, "qPBReaderDocView::PreviousPage");

   TRACE << "TODO loading_url check" << endl;

   int loc = _pDoc->EvalJSInt("paged_display.previous_screen_location()");
   TRACE << "paged_display.previous_screen_location() => " << loc << endl;

   if (loc < 0)
   {
      _pMgr->PreviousSection();
   }

   else
   {
      _pDoc->ScrollTo(loc, 0);
      _pMgr->Scrolled(_pDoc->GetScrollFraction());
      _pDoc->EvalJS("py_bridge.value = book_indexing.anchor_positions(py_bridge.value, true);");
      _pDoc->EvalJS("py_bridge.value = paged_display.column_boundaries();");
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::NextPage()
{
   TRSCOPE(view, "qPBReaderDocView::NextPage");

   TRACE << "TODO loading_url check" << endl;

   int loc = _pDoc->EvalJSInt("paged_display.next_screen_location()");
   TRACE << "paged_display.next_screen_location() => " << loc << endl;

   if (loc < 0)
   {
      _pMgr->NextSection();
   }

   else
   {
      _pDoc->ScrollTo(loc, 0);
      _pMgr->Scrolled(_pDoc->GetScrollFraction());
      _pDoc->EvalJS("py_bridge.value = book_indexing.anchor_positions(py_bridge.value, true);");
      _pDoc->EvalJS("py_bridge.value = paged_display.column_boundaries();");
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::MagnifyFonts(double step)
{
   TRSCOPE(view, "qPBReaderDocView::MagnifyFonts");

   SetViewZoom(GetMultiplier() + step);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::ShrinkFonts(double step)
{
   TRSCOPE(view, "qPBReaderDocView::ShrinkFonts");

   double dm = GetMultiplier();

   if (dm > step)
   {
      SetViewZoom(dm - step);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::BookMarginChanged(int state)
{
   TRSCOPE(view, "qPBReaderDocView::BookMarginChanged");

   qPBReaderPlatform::ShowHourGlass();

   bool bUseBookMargin = state == Qt::Checked;

   TRACE << VAR(bUseBookMargin) << endl;

   _pDoc->SetUseBookMargin(bUseBookMargin);
   _pDoc->AfterLoad();

   _pMgr->UseBookMarginChanged(bUseBookMargin);
   _pMgr->Scrolled(_pDoc->GetScrollFraction());

   qPBReaderPlatform::HideHourGlass();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::IgnoreFontsChanged(int state)
{
   TRSCOPE(view, "qPBReaderDocView::IgnoreFontsChanged");

   bool bIgnoreFonts = state == Qt::Checked;

   TRACE << VAR(bIgnoreFonts) << endl;

   SetIgnoreFonts(bIgnoreFonts);

   _pMgr->IgnoreFontsChanged(bIgnoreFonts);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::UseHyphensChanged(const QString & isHyphenPatternLang)
{
   TRSCOPE(view, "qPBReaderDocView::UseHyphensChanged");


   TRACE << ENC(isHyphenPatternLang) << endl;

   _pDoc->SetHyphenPatternLang(isHyphenPatternLang);

   _pMgr->HyphenLangChanged(isHyphenPatternLang);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::TopMarginMinus()
{
   TRSCOPE(view, "qPBReaderDocView::TopMarginMinus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetTopMargin();

   _pDoc->SetTopMargin(val - 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetTopMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::TopMarginPlus()
{
   TRSCOPE(view, "qPBReaderDocView::TopMarginPlus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetTopMargin();

   _pDoc->SetTopMargin(val + 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetTopMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SideMarginMinus()
{
   TRSCOPE(view, "qPBReaderDocView::SideMarginMinus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetSideMargin();

   _pDoc->SetSideMargin(val - 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetSideMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SideMarginPlus()
{
   TRSCOPE(view, "qPBReaderDocView::SideMarginPlus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetSideMargin();

   _pDoc->SetSideMargin(val + 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetSideMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::BottomMarginMinus()
{
   TRSCOPE(view, "qPBReaderDocView::BottomMarginMinus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetBottomMargin();

   _pDoc->SetBottomMargin(val - 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetBottomMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::BottomMarginPlus()
{
   TRSCOPE(view, "qPBReaderDocView::BottomMarginPlus");

   qPBReaderPlatform::ShowHourGlass();

   int val = _pDoc->GetBottomMargin();

   _pDoc->SetBottomMargin(val + 2);

   _pDoc->AfterLoad();

   TRACE << "before " << val << " after " << _pDoc->GetBottomMargin() << endl;

   ForcedMarginChanged();
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::DefaultMarginValues()
{
   TRSCOPE(view, "qPBReaderDocView::DefaultMarginValues");

   // get current used values

   int nCurrentTop = _pDoc->GetTopMargin(),
       nCurrentSide = _pDoc->GetSideMargin(),
       nCurrentBottom = _pDoc->GetBottomMargin();

   // get application default values

   int nDefaultTop = 0,
       nDefaultSide = 0,
       nDefaultBottom = 0;

   qPBReaderConfig::GetDefaultMargins(
      nDefaultTop, nDefaultSide, nDefaultBottom
   );

   // if something is different

   if (nCurrentTop != nDefaultTop ||
         nCurrentSide != nDefaultSide ||
         nCurrentBottom != nDefaultBottom)
   {
      qPBReaderPlatform::ShowHourGlass();

      // update

      _pDoc->SetMargins(nDefaultTop, nDefaultSide, nDefaultBottom);
      _pDoc->AfterLoad();

      // and notify

      ForcedMarginChanged();
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::UpdateFonts(int i,
                                   const QString & isSerifFamily,
                                   const QString & isSansFamily,
                                   const QString & isMonoFamily)
{
   QString sStdBefore, sSerifBefore, sSansBefore, sMonoBefore;
   _pDoc->GetFonts(sStdBefore, sSerifBefore, sSansBefore, sMonoBefore);


   _pDoc->SetFonts(i, isSerifFamily, isSansFamily, isMonoFamily);

   QString sStd, sSerif, sSans, sMono;
   _pDoc->GetFonts(sStd, sSerif, sSans, sMono);

   if (sStdBefore != sStd)
   {
      qPBReaderPlatform::ShowHourGlass();

      _pDoc->AfterLoad();
   }

   _pMgr->FontsChanged(sStd, sSerif, sSans, sMono);
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

void qPBReaderDocView::ForcedMarginChanged()
{
   int nTop = _pDoc->GetTopMargin(),
       nSide = _pDoc->GetSideMargin(),
       nBottom = _pDoc->GetBottomMargin();

   emit ForcedMarginChanged(nTop, nSide, nBottom);

   _pMgr->Scrolled(_pDoc->GetScrollFraction());
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetViewZoom(double d)
{
   TRSCOPE(view, "qPBReaderDocView::SetViewZoom");

   _pDoc->SavePagePos();

   SetMultiplier(d);

   _pDoc->RestorePagePos();

   double dsf = _pDoc->GetScrollFraction();

   _pMgr->Scrolled(dsf);
}

// -----------------------------------------------------------------------------

void qPBReaderDocView::SetHyphenPatternLang(const QString & isLang)
{
   _pDoc->SetHyphenPatternLang(isLang);
}

// -----------------------------------------------------------------------------

QString qPBReaderDocView::GetHyphenPatternLang()
{
   return _pDoc->GetHyphenPatternLang();
}

// -----------------------------------------------------------------------------

qPBReaderPagePos * qPBReaderDocView::GetPagePos()
{
   return _pDoc->GetPagePos();
}

// -----------------------------------------------------------------------------
