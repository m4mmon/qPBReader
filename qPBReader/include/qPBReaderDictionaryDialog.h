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

#ifndef _qPBReaderDictionaryDialog_h__
#define _qPBReaderDictionaryDialog_h__

#include <QObject>
#include <QRect>
#include <QString>

class qPBReaderDocView;
class qPBReaderTraceScope;

typedef int (*EvtLoopHandler)(int, int, int);

class qPBReaderDictionaryDialog : public QObject
{
      Q_OBJECT

   public :
      static qPBReaderDictionaryDialog & Instance();
      ~qPBReaderDictionaryDialog();
      void DisplayDefinition(qPBReaderDocView * ipView,
                             const QRect & iRect,
                             const QString & isWord);

      void UpdateDefinition(const QRect & iRect,
                            const QString & isWord);
      void Close();
      inline bool IsOpened();

   private :

      qPBReaderDictionaryDialog();

      void SetWord(const QRect & iRect,
                   const QString & isWord);

      void CleanWord();
      void ReplaceIVProcBeforeDic();
      void RestoreIVProcBeforeDic();


      qPBReaderDocView * _pView;
      bool _bOpened;
      bool _bReenter;

      static void ShowState(qPBReaderTraceScope & tr);
      static int OriginalProcHandler(int type, int par1, int par2);
      static int InternalDicProcHandler(int type, int par1, int par2);

      static EvtLoopHandler _pOrgProc;
      static EvtLoopHandler _pDicInternalProc;
};

bool qPBReaderDictionaryDialog::IsOpened()
{
   return _bOpened;
}

#endif
