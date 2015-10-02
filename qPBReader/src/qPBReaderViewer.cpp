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

#include "qPBReaderViewer.h"

// qt
#include <QGesture>
#include <QGridLayout>
#include <QUrl>

// project
#include "qPBReaderContextMenu.h"
#include "qPBReaderDictionaryDialog.h"
#include "qPBReaderDoc.h"
#include "qPBReaderDocView.h"
#include "qPBReaderFontSettingsDialog.h"
#include "qPBReaderGoBackDlg.h"
#include "qPBReaderPageCounter.h"
#include "qPBReaderPlatform.h"
#include "qPBReaderSettingsDialog.h"
#include "qPBReaderTextLayoutSettingsDialog.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderViewer::qPBReaderViewer(QString & isEpub) :
   QMainWindow(),
   _pView(new qPBReaderDocView(this)),
   _pSettingsDlg(0),
   _pBackDlg(new qPBReaderGoBackDlg(this)),
   _book(isEpub),
   _bDisplayingToc(false),
   _dPrevPos(-1.),
   _nTotal(-1),
   _nDelta(-1),
   _nPageInSection(0),
   _bCounting(false),
   _bLoading(false)
{
   TRSCOPE(viewer, "qPBReaderViewer::qPBReaderViewer");

   setWindowTitle("E-book viewer");
   setObjectName("EbookViewer");

   QWidget * pCentralWidget = new QWidget(this);
   pCentralWidget->setObjectName("centralWidget");
   setCentralWidget(pCentralWidget);

   QGridLayout * pLayout = new QGridLayout(pCentralWidget);
   pCentralWidget->setLayout(pLayout);
   pLayout->setContentsMargins(0, 0, 0, 0);

   _pView->setMinimumSize(100, 100);
   _pView->InitializeView();
   _pView->setObjectName("view");
   pLayout->addWidget(_pView);

   // prepare dialogs

   _pSettingsDlg = new qPBReaderSettingsDialog(_pView, this);
   _pTextLayoutDlg = new qPBReaderTextLayoutSettingsDialog(_pView, this);

   connect(
      _pView->page(), SIGNAL(linkClicked(const QUrl &)),
      this, SLOT(LinkClicked(const QUrl &))
   );

   connect(
      _pView, SIGNAL(MagnificationChanged(double)),
      this  ,   SLOT(MagnificationChanged(double))
   );

   connect(
      _pView, SIGNAL(ForcedMarginChanged(int, int, int)),
      this  ,   SLOT(ForcedMarginChanged(int, int, int))
   );

   grabGesture(Qt::TapGesture);
   grabGesture(Qt::TapAndHoldGesture);
   grabGesture(Qt::PinchGesture);
}

// -----------------------------------------------------------------------------

qPBReaderViewer::~qPBReaderViewer()
{
   TRSCOPE(viewer, "qPBReaderViewer::~qPBReaderViewer");

   SaveState();
}

// -----------------------------------------------------------------------------

bool qPBReaderViewer::Start()
{
   TRSCOPE(viewer, "qPBReaderViewer::Start");

   bool b = _book.Open();

   if (b)
   {
      qPBReaderPlatform::SubscribeToInkViewEvents(this);

      qPBReaderPlatform::ReaderApplicationStarted(_book.GetFile());

      RestoreState();

      Reload();
   }

   return b;
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::PreviousSection()
{
   TRSCOPE(viewer, "qPBReaderViewer::PreviousSection");

   // not viewing toc

   if (!_bDisplayingToc)
   {
      if (_book.PreviousSection())
      {
         _book.SavePos(1.);
         SetLocation(_book.GetCurrentSectionFile(), 1.);
      }

      else
      {
         TRACE << "No previous" << endl;
      }
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::NextSection()
{
   TRSCOPE(viewer, "qPBReaderViewer::NextSection");

   // not viewing toc

   if (!_bDisplayingToc)
   {
      if (_book.NextSection())
      {
         _book.SavePos(0.);
         SetLocation(_book.GetCurrentSectionFile(), 0.);
      }

      else
      {
         TRACE << "No next" << endl;
      }
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Scrolled(double val)
{
   TRSCOPE(viewer, "qPBReaderViewer::Scrolled");

   TRACE << VAR(val) << endl;

   if (!_bDisplayingToc)
   {
      _book.SavePos(val);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::LoadFinished(bool ok)
{
   TRSCOPE(viewer, "qPBReaderViewer::LoadFinished");

   _bLoading = false;

   if (ok && !_sPendingAnchor.isEmpty())
   {
      _pView->ScrollTo(_sPendingAnchor, true);
      _sPendingAnchor.clear();
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::UseBookMarginChanged(bool bUseBookMargin)
{
   TRSCOPE(viewer, "qPBReaderViewer::UseBookMarginChanged");

   _book.SaveUseBookMargin(bUseBookMargin);

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::FontsChanged(const QString & isStandardFamily,
                                   const QString & isSerifFamily,
                                   const QString & isSansFamily,
                                   const QString & isMonoFamily)
{
   TRSCOPE(viewer, "qPBReaderViewer::FontsChanged");

   _book.SaveSerifFamily(isSerifFamily);
   _book.SaveSansFamily(isSansFamily);
   _book.SaveMonoFamily(isMonoFamily);

   int i = 0;

   if (isStandardFamily == isSerifFamily)
   {
      i = 0;
   }

   else if (isStandardFamily == isSansFamily)
   {
      i = 1;
   }

   else if (isStandardFamily == isMonoFamily)
   {
      i = 2;
   }

   _book.SaveStandardFont(i);

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::HyphenLangChanged(const QString & isHyphenPatternLang)
{
   TRSCOPE(viewer, "qPBReaderViewer::HyphenLangChanged");

   _book.SaveHyphenPatternLang(isHyphenPatternLang);

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::IgnoreFontsChanged(bool ibIgnoreFonts)
{
   TRSCOPE(viewer, "qPBReaderViewer::IgnoreFontsChanged");

   _book.SaveIgnoreFonts(ibIgnoreFonts);

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::UpdatePageDisplay()
{
   TRSCOPE(viewer, "qPBReaderViewer::UpdatePageDisplay");

   if (_nTotal > -1)
   {
      if (!_bDisplayingToc)
      {
         _nPageInSection = _pView->GetCurrentFilePageNumber();

         TRACE << VAR(_nPageInSection) << VAR(_nDelta) << VAR(_nTotal) << endl;

         if (_nPageInSection > 0)
         {
            qPBReaderPlatform::DisplayPageNumber(_book.GetTitle(),
                                                 _nDelta + _nPageInSection,
                                                 _nTotal);
         }

         else
         {
            TRACE << "Page position not yet available" << endl;
         }
      }

      else
      {
         TRACE << "do not change" << endl;
      }
   }

   else if (!_bCounting)
   {
      TRACE << "Erase panel" << endl;
      qPBReaderPlatform::ErasePanelMessage();
   }
}
#ifdef Q_OS_POCKETBOOK

// -----------------------------------------------------------------------------

#include <inkview.h>

void qPBReaderViewer::InkViewEventHandler(bool main,
      int type,
      int parm1,
      int parm2)
{
   TRSCOPE(XXXXXXXXX, "qPBReaderViewer::InkViewEventHandler");

   TRACE << type << " " << ENC(iv_evttype(type))
         << ", " << parm1 << ", "
         << parm2 << ", "
         << " from mainproc=" << main << endl;

   if (type == EVT_EXIT)
   {
      TRACE << "Application is terminating, save while we can" << endl;
      SaveState();
   }
}
#endif

// -----------------------------------------------------------------------------

void qPBReaderViewer::Dictionary(QPoint iPt)
{
   TRSCOPE(viewer, "qPBReaderViewer::Dictionary");

   QPoint pt = mapFromGlobal(iPt);
   QString sWord;
   QRect rect;

   if (_pView->GetWordAt(pt, sWord, rect))
   {
      qPBReaderDictionaryDialog::Instance().DisplayDefinition(_pView, rect, sWord);
   }

   else
   {
      qPBReaderPlatform::MessageTimeOut(
         "qPBReader",
         tr("Could not find word at selected point!"),
         2000
      );
   }
}

// -----------------------------------------------------------------------------
//  public slots
// -----------------------------------------------------------------------------

void qPBReaderViewer::MagnificationChanged(double val)
{
   TRSCOPE(viewer, "qPBReaderViewer::MagnificationChanged");

   TRACE << VAR(val) << endl;

   _book.SaveMagnification(val);

   if (!_bDisplayingToc)
   {
      _book.SavePos(_book.GetPos());
   }

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::ForcedMarginChanged(int iTop, int iSide, int iBottom)
{
   TRSCOPE(viewer, "qPBReaderViewer::ForcedMarginChanged");

   _book.SaveMargins(iTop, iSide, iBottom);
   _pTextLayoutDlg->SetMarginValues(iTop, iSide, iBottom);

   ResetPagesCount();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::TextLayout()
{
   TRSCOPE(viewer, "qPBReaderViewer::TextLayout");

   _pTextLayoutDlg->exec();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::FontOptions()
{
   TRSCOPE(viewer, "qPBReaderViewer::FontOptions");

   qPBReaderFontSettingsDialog(_pView, this).exec();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Orientation()
{
   TRSCOPE(viewer, "qPBReaderViewer::FontOptions");

   // special pb platform integration event.
   // found by "playing" with browser

   QEvent e((QEvent::Type)1111);
   QCoreApplication::sendEvent(this, &e);
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::ComputePagesCount()
{
   TRSCOPE(viewer, "qPBReaderViewer::ComputePagesCount");

   if (_bCounting)
   {
      return;
   }

   _pSettingsDlg->accept();

   RecalcPagesCountFromSettings();
   TRACE << "Got " << _lnPagesCount.size() << " sections counted" << endl;

   if (_lnPagesCount.size() == 0)
   {
      TRACE << "Needs computation" << endl;
      _bCounting = true;
      qPBReaderPageCounter * pCounter = new qPBReaderPageCounter(&_book,
            _pView->page()->viewportSize());

      connect(this    , SIGNAL(CountPages()),
              pCounter, SLOT(CountPages()),
              Qt::QueuedConnection);

      connect(this    , SIGNAL(InterruptPagesCount()),
              pCounter, SLOT(Interrupt()),
              Qt::QueuedConnection);

      connect(pCounter, SIGNAL(PagesCounted(QList<int>)),
              this    , SLOT(PagesCounted(QList<int>)));

      connect(pCounter, SIGNAL(PagesCounted(QList<int>)),
              pCounter, SLOT(deleteLater()));

      emit CountPages();
   }

   else
   {
      TRACE << "Computation not needed" << endl;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::TableOfContents()
{
   TRSCOPE(viewer, "qPBReaderViewer::TableOfContents");

   // close dialog

   _pSettingsDlg->accept();

   // get toc

   QString sToc;

   if (_book.GetHTMLToc(sToc))
   {
      // and load it

      SetLocation(sToc, 0.);
      _bDisplayingToc = true;
   }

   else
   {
      qPBReaderPlatform::MessageTimeOut(
         "qPBReader", tr("Unable to display table of contents"), 2000
      );
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Exit()
{
   TRSCOPE(viewer, "qPBReaderViewer::Exit");

   _pSettingsDlg->accept();

   qPBReaderPlatform::UnsubscribeToInkViewEvents();

   close();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::BackFromLink()
{
   TRSCOPE(viewer, "qPBReaderViewer::BackFromLink");

   _pBackDlg->Hide();
   if (_sPrevFile != _book.GetCurrentSectionFile())
   {
      _book.SwitchToSectionByFileName(_sPrevFile);
   }
   SetLocation(_sPrevFile, _dPrevPos);
}

// -----------------------------------------------------------------------------
//  protected
// -----------------------------------------------------------------------------

bool qPBReaderViewer::event(QEvent * pEvent)
{
   TRSCOPE(evt, "qPBReaderViewer::event");

   QEvent::Type type = pEvent->type();

   TRACE << "Event=" << (int)type << endl;

   bool b = false;

   // handle gestures

   if (type == QEvent::Gesture)
   {
      QList<QGesture *> lpGest = ((QGestureEvent *)pEvent)->gestures();
      b = OnGesture(lpGest);
   }

   // call parent event handler if nothing was handled before

   if (!b)
   {
      b = QMainWindow::event(pEvent);
      TRACE << "parent event handler=" << b << endl;
   }

   // in those cases, parent eent handler somehow erased panel, so refresh it

   if (b && (type == QEvent::UpdateRequest || type == QEvent::WindowActivate))
   {
      UpdatePageDisplay();
   }

   // this event, if settings dialog is present, means that display was rotated
   // settings panel position needs to be updated

   else if (b && type == QEvent::Resize && _pSettingsDlg->isVisible())
   {
      // forget about pages count

      ResetPagesCount();

      // refresh view

      Reload();

      // and move dialog to the center of screen

      int w = width();
      int h = height();
      TRACE << VAR(w) << VAR(h) << endl;

      QPoint pt = _pSettingsDlg->pos();
      TRACE << "curr pos " << pt.x() << ", " << pt.y() << endl;

      int newx = w / 2 - _pSettingsDlg->width() / 2;
      int newy = h / 2 - _pSettingsDlg->height() / 2;

      _pSettingsDlg->move(newx, newy);
   }

   return b;
}

// -----------------------------------------------------------------------------
//  private slots
// -----------------------------------------------------------------------------

void qPBReaderViewer::LinkClicked(const QUrl & iUrl)
{
   TRSCOPE(viewer, "qPBReaderViewer::LinkClicked");

   // save anchor

   _sPendingAnchor = iUrl.fragment();

   TRACE << "Url fragment " << ENC(_sPendingAnchor) << endl;

   // local file

   if (iUrl.isLocalFile())
   {
      // not in TOC

      bool bInToc = _bDisplayingToc;
      if (!_bDisplayingToc)
      {
         // store current file

         _sPrevFile = _book.GetCurrentSectionFile();

         // store current position

         _dPrevPos = _pView->GetScrollFraction();
         TRACE << VAR(_sPrevFile) << VAR(_dPrevPos) << endl;
      }

      // get local file

      QString sFile = iUrl.path();
      TRACE << ENC(sFile) << endl;

      // verify if actual epub section

      int nCurrSection = _book.GetCurrentSectionIndex();
      if (_book.SwitchToSectionByFileName(sFile))
      {
         int nNewCurrSection = _book.GetCurrentSectionIndex();
         if (nCurrSection == nNewCurrSection)
         {
            TRACE << "Same section, jump to bookmark" << endl;

            LoadFinished(true);
         }
         else
         {
            TRACE << "Load new section" << endl;

            // jump to it

            SetLocation(_book.GetCurrentSectionFile(), 0.);
         }

         // do not show back button if in TOC

         if (!bInToc)
         {
            _pBackDlg->Show();
         }
      }

      else
      {
         _dPrevPos = -1.;

         qPBReaderPlatform::MessageTimeOut(
            "qPBReader",
            tr("Link to local content not corresponding to an epub section!"),
            4000
         );
      }
   }

   else
   {
      TRACE << "External link, TODO" << endl;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Refresh()
{
   qPBReaderPlatform::UpdateScreenFull();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::PagesCounted(QList<int> iln)
{
   TRSCOPE(viewer, "qPBReaderViewer::PagesCounted");

   _bCounting = false;

   int n = iln.size();
   TRACE << "Got " << n << " counts" << endl;

   for (int idx = 0; idx < n; idx++)
   {
      TRACE << " " << idx << "=" << iln[idx] << endl;
   }

   _book.SavePagesCount(iln);
   RecalcPagesCountFromSettings();
   UpdatePageDisplay();
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

qPBReaderViewer::qPBReaderViewer() :
   QMainWindow(),
   _pView(new qPBReaderDocView(this)),
   _pSettingsDlg(0),
   _pTextLayoutDlg(0),
   _book(""),
   _bDisplayingToc(false)
{
   TRSCOPE(viewer, "qPBReaderViewer::qPBReaderViewer");
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Settings()
{
   TRSCOPE(viewer, "qPBReaderViewer::Settings");

   int rc = _pSettingsDlg->exec();
   TRACE << "qPBReaderSettingsDialog().exec() => " << rc << endl;
#ifdef Q_OS_POCKETBOOK

   // reset the top-level widget as the active window. if not done, after
   // closing the dialog, keyboard events do not reach handler

   activateWindow();

   // force an update some time after dialog had hopefully disappeared

   QTimer::singleShot(1000, this, SLOT(Refresh()));
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::SetLocation(const QString & isPath, double pos)
{
   TRSCOPE(viewer, "qPBReaderViewer::setLocation");

   TRACE << "Setting location " << ENC(isPath) << endl;

   _bLoading = true;
   _bDisplayingToc = false;
   _pView->Load(isPath, pos);
   RecalcPagesCountFromSettings();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::Reload()
{
   TRSCOPE(viewer, "qPBReaderViewer::Reload");

   // reload page from current section and last saved position

   SetLocation(_book.GetCurrentSectionFile(), _book.GetPos());
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::RestoreState()
{
   TRSCOPE(viewer, "qPBReaderViewer::RestoreState");

   // hyphen lang

   _pView->SetHyphenPatternLang(_book.GetHyphenPatternLang());

   // margins

   _pView->SetUseBookMargin(_book.UseBookMargin());

   int top, side, bottom;
   _book.GetMargins(top, side, bottom);
   _pView->SetForcedMargins(top, side, bottom);

   // magnification

   double val = _book.GetMagnification();
   _pView->SetMultiplier(val);

   // fonts

   _pView->SetIgnoreFonts(_book.GetIgnoreFonts());

   _pView->SetFonts(_book.GetStandardFont(),
                    _book.GetSerifFamily(),
                    _book.GetSansFamily(),
                    _book.GetMonoFamily());

   // page count

   RecalcPagesCountFromSettings();
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::keyPressEvent(QKeyEvent * iEvt)
{
   TRSCOPE(viewer, "qPBReaderViewer::keyPressEvent");

   int nKeyCode = 0;
   bool bAutoRepeat = false;

   if (iEvt)
   {
      nKeyCode = iEvt->key();
      bAutoRepeat = iEvt->isAutoRepeat();
   }

   TRACE << "nKeyCode=" << (void *)nKeyCode << " " << VAR(bAutoRepeat) << endl;

   if (_bLoading)
   {
      TRACE << "Loading in progress, ignore input" << endl;
      return;
   }

   switch (nKeyCode)
   {
      case Qt::Key_PageUp:
         _pView->PreviousPage();
         break;

      case Qt::Key_PageDown:
         _pView->NextPage();
         break;
#ifdef Q_OS_POCKETBOOK

      case Qt::Key_Menu:
#else

      //case Qt::Key_Home:
      // F6 because home does not work anymore when editable property is set
      // on frame
      case Qt::Key_F6:
#endif
         if (_pBackDlg->isVisible())
         {
            _pBackDlg->Hide();
         }

         else if (_bDisplayingToc)
         {
            Reload();
         }

         else
         {
            Settings();
         }

         break;

      default:
         TRACE << "Unhandled key code " << nKeyCode;
   };
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::ResetPagesCount()
{
   TRSCOPE(viewer, "qPBReaderViewer::ResetPagesCount");

   // interrupt count if required

   emit InterruptPagesCount();

   //

   if (_lnPagesCount.size())
   {
      _lnPagesCount.clear();

      // erase from settings

      _book.SavePagesCount(_lnPagesCount);
   }

   _nTotal = -1;
   _nDelta = -1;
   _nPageInSection = 0;
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::RecalcPagesCountFromSettings()
{
   TRSCOPE(viewer, "qPBReaderViewer::RecalcPagesCountFromSettings");

   _book.GetPagesCount(_lnPagesCount);

   int n = _lnPagesCount.size();

   if (n > 0)
   {
      _nTotal = 0;
      _nDelta = 0;
      _nPageInSection = 0;
      int nCurrSection = _book.GetCurrentSectionIndex();

      for (int idx = 0; idx < n; idx++)
      {
         _nTotal += _lnPagesCount[idx];

         if (idx < nCurrSection)
         {
            _nDelta += _lnPagesCount[idx];
         }
      }
   }

   else
   {
      _nTotal = -1;
      _nDelta = -1;
      _nPageInSection = 0;
   }

   TRACE << "Got " << n << " counts, " << VAR(_nTotal) << VAR(_nDelta) <<
         VAR(_nPageInSection) << endl;
}

// -----------------------------------------------------------------------------

void qPBReaderViewer::SaveState()
{
   TRSCOPE(viewer, "qPBReaderViewer::SaveState");

   if (_nTotal > 0)
   {
      if (_nPageInSection > 0)
      {
         qPBReaderPlatform::SaveBookState(_book.GetFile(),
                                          _nDelta + _nPageInSection,
                                          _nTotal);
      }

      else
      {
         TRACE << "Unable to get relative page in section" << endl;
      }
   }

   else
   {
      TRACE << "Pages not computed, not saving state" << endl;
   }
}

// -----------------------------------------------------------------------------

bool qPBReaderViewer::OnGesture(QList<QGesture *> & ilpGest)
{
   TRSCOPE(viewer, "qPBReaderViewer::OnGesture");

   bool b = false;

   // process first gesture only

   int nGestures = ilpGest.size();
   TRACE << "Have " << nGestures << " gestures" << endl;

   for (int idx = 0; !b && idx < nGestures; idx++)
   {
      QGesture * pCurrGest = ilpGest[idx];

      if (!pCurrGest || pCurrGest->state() != Qt::GestureFinished)
      {
         continue;
      }

      TRACE << "Got gesture type " << (int)pCurrGest->gestureType() << endl;

      switch (pCurrGest->gestureType())
      {
         case Qt::TapGesture :
            b = OnTapGesture((QTapGesture *)pCurrGest);
            break;

         case Qt::TapAndHoldGesture :
            b = OnTapAndHoldGesture((QTapAndHoldGesture *)pCurrGest);
            break;

         case Qt::PinchGesture :
            b = OnPinchGesture((QPinchGesture *)pCurrGest);
            break;

         case Qt::PanGesture :
         case Qt::SwipeGesture :
         case Qt::CustomGesture :
         case Qt::LastGestureType :
            break;
      };
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderViewer::OnPinchGesture(QPinchGesture * ipGest)
{
   TRSCOPE(viewer, "qPBReaderViewer::OnTapGesture");

   bool b = false;

   double scale = ipGest->totalScaleFactor();
   double scale2 = ipGest->scaleFactor();
   TRACE << "Pinch(" << scale << "|" << scale2 << ")" << endl;

   if (scale < 0.75)
   {
      scale = 0.75;
   }

   if (scale > 1.25)
   {
      scale = 1.25;
   }

   TRACE << "Corrected scale=" << scale << endl;

   double d = _pView->GetMultiplier();
   double newd = ((int)(d * scale * 100)) / 100.;
   TRACE << "Before=" << d << " after=" << newd << endl;
   TRACE << "Before=" << (int)(d * 100) << " after=" << (int)(newd * 100) << endl;

   if ((int)(d * 100) != (int)(newd * 100))
   {
      _pView->SetViewZoom(newd);
   }


   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderViewer::OnTapGesture(QTapGesture * ipGest)
{
   TRSCOPE(viewer, "qPBReaderViewer::OnTapGesture");

   bool b = false;

   if (qPBReaderDictionaryDialog::Instance().IsOpened())
   {
      QPoint pt = ipGest->position().toPoint();
      QString sWord;
      QRect rect;
      b = _pView->GetWordAt(pt, sWord, rect);

      if (b)
      {
         qPBReaderDictionaryDialog::Instance().UpdateDefinition(rect, sWord);
      }
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderViewer::OnTapAndHoldGesture(QTapAndHoldGesture * ipGest)
{
   TRSCOPE(viewer, "qPBReaderViewer::OnTapAndHoldGesture");

   bool b = false;

   if (!qPBReaderDictionaryDialog::Instance().IsOpened())
   {
      int rc = qPBReaderContextMenu(this, ipGest->position().toPoint()).exec();
      TRACE << "qPBReaderContextMenu rc=" << rc << endl;
   }

   return b;
}

// -----------------------------------------------------------------------------
