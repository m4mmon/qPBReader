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

#include "qPBReaderDictionaryDialog.h"
#ifdef Q_OS_POCKETBOOK

// platform
#include <inkinternal.h>
#include <inkview.h>
#endif

// std
#include <assert.h>
#include <sys/syscall.h>

// project
#include "qPBReaderDocView.h"
#include "qPBReaderTracer.h"

#ifdef Q_OS_POCKETBOOK
// -----------------------------------------------------------------------------

static iv_wlist pWord[2] =
{
   {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
};

#endif
// -----------------------------------------------------------------------------
//  static
// -----------------------------------------------------------------------------

EvtLoopHandler qPBReaderDictionaryDialog::_pOrgProc = 0;
EvtLoopHandler qPBReaderDictionaryDialog::_pDicInternalProc = 0;

qPBReaderDictionaryDialog & qPBReaderDictionaryDialog::Instance()
{
   static qPBReaderDictionaryDialog instance;
   return instance;
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::ShowState(qPBReaderTraceScope & tr)
{
   tr << "_pOrgProc              " << (void *)_pOrgProc << endl
      << "_pDicInternalProc      " << (void *)_pDicInternalProc << endl
#ifdef Q_OS_POCKETBOOK
      << "ivstate.hproc          " << (void *)ivstate.hproc << endl
#endif
      << "OriginalProcHandler    " << (void *)OriginalProcHandler << endl
      << "InternalDicProcHandler " << (void *)InternalDicProcHandler << endl
      << "_bOpened               " << Instance()._bOpened << endl
      << "_bReenter              " << Instance()._bReenter << endl;
}

// -----------------------------------------------------------------------------

int qPBReaderDictionaryDialog::OriginalProcHandler(int type, int par1, int par2)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::OriginalProcHandler");

   int rc = 0;

#ifdef Q_OS_POCKETBOOK
   assert(_pOrgProc != 0);

   TRACE << type << " " << ENC(iv_evttype(type)) << " " << par1 << " " << par2 <<
         endl;

   TRACE << "thread id=" << (int)syscall(SYS_gettid) << endl;

   switch (type)
   {
      case EVT_OPENDIC:
         TRACE << "Dictionary opening" << endl;
         OpenDictionaryView(pWord, 0);
         rc = 1;
         break;

      case EVT_SHOW :
         TRACE << "Dictionary is closing" << endl;
         qPBReaderDictionaryDialog::Instance().Close();
         rc = 1;
         break;

      case EVT_HIDE:

         TRACE << "Flagging dico as opened" << endl;
         Instance()._bOpened = true;

         TRACE << "Original loop has become a secondary loop." << endl;
         _pDicInternalProc = ivstate.hproc;
         ivstate.hproc = InternalDicProcHandler;
         TRACE << "Hooked internal dic proc with InternalDicProcHandler" << endl;


         SoftUpdate();
         rc = 1;
         break;

      case EVT_PREVPAGE:
      case EVT_NEXTPAGE:
      case EVT_KEYPRESS:
      case EVT_KEYUP:
      case EVT_KEYREPEAT:
         TRACE << "Blocking keyboard" << endl;
         rc = 1;
         break;
   }

   if (!rc)
   {
      rc = _pOrgProc(type, par1, par2);
      TRACE << "_pOrgProc rc=" << rc << endl;
   }

#else
   Q_UNUSED(type);
   Q_UNUSED(par1);
   Q_UNUSED(par2);

#endif
   return rc;
}

// -----------------------------------------------------------------------------

int qPBReaderDictionaryDialog::InternalDicProcHandler(int type, int par1,
      int par2)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::InternalDicProcHandler");

   int rc = 0;

#ifdef Q_OS_POCKETBOOK
   assert(_pDicInternalProc != 0);

   TRACE << type << " " << ENC(iv_evttype(type)) << " " << par1 << " " << par2 <<
         endl;

   TRACE << "thread id=" << (int)syscall(SYS_gettid) << endl;

   rc = _pDicInternalProc(type, par1, par2);
   TRACE << "_pDicInternalProc rc=" << rc << endl;

   if (!rc && !Instance()._bReenter && !ISKEYEVENT(type))
   {
      assert(_pOrgProc != 0);
      TRACE << "Calling _pOrgProc" << endl;
      rc = _pOrgProc(type, par1, par2);
   }

#else
   Q_UNUSED(type);
   Q_UNUSED(par1);
   Q_UNUSED(par2);

#endif
   return rc;
}

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderDictionaryDialog::~qPBReaderDictionaryDialog()
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::~qPBReaderDictionaryDialog");
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::DisplayDefinition(qPBReaderDocView * ipView,
      const QRect & iRect,
      const QString & isWord)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::DisplayDefinition");

   ShowState(TRACE);

#ifdef Q_OS_POCKETBOOK
   TRACE << "thread id=" << (int)syscall(SYS_gettid) << endl;

#endif

   if (!_bOpened)
   {
      SetWord(iRect, isWord);
      _pView = ipView;
      ReplaceIVProcBeforeDic();
#ifdef Q_OS_POCKETBOOK
      OpenDictionaryView(pWord, 0);
#endif
   }

   else
   {
      TRACE << "Called while dictionary already opened" << endl;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::UpdateDefinition(const QRect & iRect,
      const QString & isWord)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::UpdateDefinition");

   ShowState(TRACE);

#ifdef Q_OS_POCKETBOOK
   TRACE << "thread id=" << (int)syscall(SYS_gettid) << endl;

#endif

   if (_bOpened)
   {
#ifdef Q_OS_POCKETBOOK
      SetWord(iRect, isWord);

      // dirty trick to force dictionary dialog update

      _bReenter = true;
      int x = (iRect.right() - iRect.left()) / 2 + iRect.left();
      int y = (iRect.bottom() - iRect.top()) / 2 + iRect.top();
      _pDicInternalProc(30, x, y);
      _pDicInternalProc(29, x, y);
      _bReenter = false;
#else
      Q_UNUSED(iRect);
      Q_UNUSED(isWord);
#endif
   }

   else
   {
      TRACE << "Called while dictionary not opened yet" << endl;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::Close()
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::Close");

   RestoreIVProcBeforeDic();
   _bOpened = false;
   CleanWord();
#ifdef Q_OS_POCKETBOOK
   ::FullUpdate();
#endif
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

qPBReaderDictionaryDialog::qPBReaderDictionaryDialog() :
   QObject(), _pView(0), _bOpened(false), _bReenter(false)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::qPBReaderDictionaryDialog");
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::SetWord(const QRect & iRect,
                                        const QString & isWord)
{
   TRSCOPE(dic, "qPBReaderDictionaryDialog::SetWord");

   CleanWord();
#ifdef Q_OS_POCKETBOOK

   pWord[0].word = strdup(isWord.toUtf8().data());
   pWord[0].x1 = iRect.left();
   pWord[0].y1 = iRect.top();
   pWord[0].x2 = iRect.right();
   pWord[0].y2 = iRect.bottom();
#else

   Q_UNUSED(iRect);
   Q_UNUSED(isWord);
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::CleanWord()
{
#ifdef Q_OS_POCKETBOOK

   if (pWord[0].word)
   {
      free(pWord[0].word);
      pWord[0].word = 0;
   }

#endif
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::ReplaceIVProcBeforeDic()
{
#ifdef Q_OS_POCKETBOOK
   assert(!_bOpened);

   _pOrgProc = ivstate.hproc;
   ivstate.hproc = OriginalProcHandler;
#endif
}

// -----------------------------------------------------------------------------

void qPBReaderDictionaryDialog::RestoreIVProcBeforeDic()
{
#ifdef Q_OS_POCKETBOOK
   assert(_bOpened);

   ivstate.hproc = _pOrgProc;
#endif
}

// -----------------------------------------------------------------------------
