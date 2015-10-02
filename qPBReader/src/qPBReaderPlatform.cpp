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

#include "qPBReaderPlatform.h"

#ifdef Q_OS_POCKETBOOK
// platform
#include "bookstate.h"
#include "inkinternal.h"
#include "inkview.h"

// -----------------------------------------------------------------------------

// The following symbols are present on 623 firmware 4.4 but
// not available with 1.1 SDK, and provided in SDK481.
// so I assume it is safe to use them
extern "C"
{

   void ShowPureHourglassForce();

   // nice centered display if reading mode is enabled
   int DrawPanel2(const ibitmap * icon,
                  const char * text,
                  const char * title,
                  int percent,
                  int readingModeEnable);

   // Allows nice page count display like AdobeViewer or official fbreader
   int DrawPanel4(const ibitmap * icon,
                  const char * bookName,
                  int currentPage,
                  int totalPages,
                  int readingModeEnable);

   // sets panel in reading mode
   void SetReadingMode(int enable, int after_time, int update);
}
#endif

// qt
#include <QLocale>
#include <QString>

// project
#include "qPBReaderViewer.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderViewer * qPBReaderPlatform::_pViewer = 0;
IVEventHandlerFn qPBReaderPlatform::_pProc = 0;
IVEventHandlerFn qPBReaderPlatform::_pMainProc = 0;

// -----------------------------------------------------------------------------

int qPBReaderPlatform::IVProc(int type, int parm1, int parm2)
{
#ifdef Q_OS_POCKETBOOK
   int rc = _pMainProc(type, parm1, parm2);

   if (_pViewer)
   {
      _pViewer->InkViewEventHandler(false, type, parm1, parm2);
   }

   else
   {
      fprintf(stderr, "      ---> IVProc(%d, %d, %d)\n", type, parm1, parm2);
      fflush(stderr);
   }

#else
   Q_UNUSED(type);
   Q_UNUSED(parm1);
   Q_UNUSED(parm2);
   int rc = 0;

#endif
   return rc;
}

// -----------------------------------------------------------------------------

int qPBReaderPlatform::IVMainProc(int type, int parm1, int parm2)
{
#ifdef Q_OS_POCKETBOOK
   int rc = _pMainProc(type, parm1, parm2);

   if (_pViewer)
   {
      _pViewer->InkViewEventHandler(true, type, parm1, parm2);
   }

   else
   {
      fprintf(stderr, "      ---> IVMainProc(%d, %d, %d)\n", type, parm1, parm2);
      fflush(stderr);
   }

   if (rc == 0 && type == EVT_FOREGROUND)
   {
      ::SoftUpdate();
   }

#else
   Q_UNUSED(type);
   Q_UNUSED(parm1);
   Q_UNUSED(parm2);
   int rc = 0;

#endif
   return rc;
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::SubscribeToInkViewEvents(qPBReaderViewer * ipViewer)
{
   TRSCOPE(pb, "qPBReaderPlatform::SubscribeToInkViewEvents");
#ifdef Q_OS_POCKETBOOK

   if (_pProc || _pMainProc)
   {
      TRACE << "Event handlers already installed" << endl;
   }

   else if (ipViewer)
   {
      _pViewer = ipViewer;

      TRACE << "Installing event handlers" << endl;

      _pProc = ivstate.hproc;
      ivstate.hproc = IVProc;

      TRACE << "------- REPLACED " << (void *)_pProc << " by " <<
            (void *)IVProc <<  endl;

      _pMainProc = ivstate.mainhproc;
      ivstate.mainhproc = IVMainProc;

      TRACE << "------- REPLACED " << (void *)_pMainProc << " by " <<
            (void *)IVMainProc <<  endl;
   }

   else
   {
      TRACE << "Bad usage" << endl;
   }
#else

   Q_UNUSED(ipViewer);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::UnsubscribeToInkViewEvents()
{
   TRSCOPE(pb, "qPBReaderPlatform::UnsubscribeToInkViewEvents");
#ifdef Q_OS_POCKETBOOK

   if (_pViewer)
   {
      _pViewer = 0;
   }

#endif
}


// -----------------------------------------------------------------------------

void qPBReaderPlatform::UpdateScreenFull()
{
   TRSCOPE(pb, "qPBReaderPlatform::UpdateScreenFull");
#ifdef Q_OS_POCKETBOOK

   ::FullUpdate();
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::ShowHourGlass()
{
   TRSCOPE(pb, "qPBReaderPlatform::ShowHourGlass");
#ifdef Q_OS_POCKETBOOK

   ::ShowPureHourglassForce();
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::HideHourGlass()
{
   TRSCOPE(pb, "qPBReaderPlatform::HideHourGlass");
#ifdef Q_OS_POCKETBOOK

   ::HideHourglass();
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::SetReadingMode(bool ibReadingMode)
{
   TRSCOPE(pb, "qPBReaderPlatform::SetReadingMode");
#ifdef Q_OS_POCKETBOOK

   ::SetReadingMode(ibReadingMode ? 1 : 0, 0, 0);
#else

   Q_UNUSED(ibReadingMode);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::ReaderApplicationStarted(const QString & isBookFile)
{
   TRSCOPE(pb, "qPBReaderPlatform::ReaderApplicationStarted");
#ifdef Q_OS_POCKETBOOK

   TRACE << "Declaring " << ENC(isBookFile) << endl;

   // set application icon
   // taskmgr looks for app_%s_icon.bmp file in theme, %s being the
   // task name. Since I have not found another way to provide an icon at
   // runtime, setting task name to "reader" does the job.

   int task = ::GetCurrentTask();
   taskinfo * pTask = ::GetTaskInfo(GetCurrentTask());

   if (pTask)
   {
      int rc = ::SetTaskParameters(task, "reader", 0, 0, pTask->flags);
      TRACE << "SetTaskParameters " << VAR(rc) << endl;
   }

   // switch to reading mode, seems to change panel appearance

   SetReadingMode(true);

   // and register book as read by this program

   ::BookReady(isBookFile.toUtf8().data());
#else

   Q_UNUSED(isBookFile);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::SaveBookState(const QString & isBookFile,
                                      int inCurrentPage,
                                      int inTotal)
{
   TRSCOPE(pb, "qPBReaderPlatform::SaveBookState");
#ifdef Q_OS_POCKETBOOK

   TRACE << ENC(isBookFile)
         << " [" << inCurrentPage << ", " << inTotal << "]" << endl;

   bsHandle h = ::bsLoad(isBookFile.toUtf8().data());

   if (h)
   {
      ::bsSetCPage(h, inCurrentPage);
      ::bsSetNPage(h, inTotal);
      ::bsSetOpenTime(h, time(0));

      if (::bsSave(h))
      {
         TRACE << "Book state saved" << endl;
      }

      else
      {
         TRACE << "Book state save failed" << endl;
      }

      ::bsClose(h);
      h = 0;
   }
#else

   Q_UNUSED(isBookFile);
   Q_UNUSED(inCurrentPage);
   Q_UNUSED(inTotal);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::UpdatePanelDisplay()
{
   TRSCOPE(pb, "qPBReaderPlatform::UpdatePanelDisplay");
#ifdef Q_OS_POCKETBOOK

   ::PartialUpdate(0,
                   ScreenHeight() - PanelHeight(),
                   ScreenWidth(),
                   PanelHeight());
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::DisplayPageNumber(const QString & isTitle,
      int inCurrentPage,
      int inTotal)
{
   TRSCOPE(pb, "qPBReaderPlatform::DisplayPageNumber");
#ifdef Q_OS_POCKETBOOK

   TRACE << "Panel " << ENC(isTitle) << " " << inCurrentPage << " / " << inTotal <<
         endl;
   ::DrawPanel4(0, isTitle.toUtf8().data(), inCurrentPage, inTotal, 1);
   UpdatePanelDisplay();
#else

   Q_UNUSED(isTitle);
   Q_UNUSED(inCurrentPage);
   Q_UNUSED(inTotal);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::DisplayPanelMessage(const QString & isMessage)
{
   TRSCOPE(pb, "qPBReaderPlatform::DisplayPanelMessage");
#ifdef Q_OS_POCKETBOOK

   TRACE << "Panel " << ENC(isMessage) << endl;
   ::DrawPanel2(0, isMessage.toUtf8().data(), "qPBReader", -1, 1);
   UpdatePanelDisplay();
#else

   Q_UNUSED(isMessage);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::ErasePanelMessage()
{
   TRSCOPE(pb, "qPBReaderPlatform::ErasePanelMessage");
#ifdef Q_OS_POCKETBOOK

   ::DrawPanel2(0, 0, 0, -1, 1);
   UpdatePanelDisplay();
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::MessageTimeOut(const QString & isTitle,
                                       const QString & isMessage,
                                       int inTimeoutMs)
{
   TRSCOPE(pb, "qPBReaderPlatform::MessageTimeOut");
#ifdef Q_OS_POCKETBOOK

   TRACE << "Message " << ENC(isTitle) << " " << ENC(isMessage) << VAR(inTimeoutMs) << endl;
   ::Message(ICON_INFORMATION,
             isTitle.toUtf8().data(),
             isMessage.toUtf8().data(),
             inTimeoutMs);
#else
   Q_UNUSED(isTitle);
   Q_UNUSED(isMessage);
   Q_UNUSED(inTimeoutMs);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderPlatform::InitializeEnv()
{
   TRSCOPE(pb, "qPBReaderPlatform::InitializeEnv");
#ifdef Q_OS_POCKETBOOK

   // use PocketBook Qt platform integration plugin

   setenv("QT_QPA_PLATFORM", "pocketbook", 1);

   // do not present network connection panel

   setenv("PB_CONNECT_NETWORK_FROM_START", "false", 1);
#endif
}

// -----------------------------------------------------------------------------

bool qPBReaderPlatform::CheckForExisting(const QString & isEbook)
{
   TRSCOPE(pb, "qPBReaderPlatform::CheckForExisting");

   bool bExists = false;
#ifdef Q_OS_POCKETBOOK

   int task = 0, subtask = 0;
   int rc = ::FindTaskByBook(isEbook.toUtf8().data(), &task, &subtask);
   TRACE << VAR(rc) << VAR(task) << VAR(subtask) << endl;

   if (rc == 1)
   {
      TRACE << "Book already opened, pop existing task" << endl;

      ::SetActiveTask(task, subtask);

      bExists = true;
   }
#else

   Q_UNUSED(isEbook);
#endif

   return bExists;
}

// -----------------------------------------------------------------------------

QString qPBReaderPlatform::LocaleName()
{
   TRSCOPE(pb, "qPBReaderPlatform::LocaleName");

#ifdef Q_OS_POCKETBOOK
   QString sLocaleName = ivstate.lang;
#else
   QString sLocaleName = QLocale::system().name();
#endif

   TRACE << VAR(sLocaleName) << endl;
   return sLocaleName;
}

// -----------------------------------------------------------------------------
