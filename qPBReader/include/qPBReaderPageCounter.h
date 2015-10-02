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

#ifndef _qPBReaderPageCounter_h__
#define _qPBReaderPageCounter_h__

#include <QList>
#include <QSize>
#include <QThread>

class qPBReaderDoc;
class qPBReaderEpub;

class qPBReaderPageCounter : public QObject
{
      Q_OBJECT

   public :

      qPBReaderPageCounter(qPBReaderEpub * ipBook, const QSize & iSz);
      ~qPBReaderPageCounter();

   public slots :

      void Interrupt();
      void CountPages();

   signals :

      void PagesCounted(QList<int>);

   private slots :

      void LoadFinished(bool bOk);
      void GoLoad();

   private :
      qPBReaderPageCounter();

      qPBReaderEpub * _pBook;
      QSize _Sz;
      qPBReaderDoc * _pDoc;
      int _currSection;
      int _nbSections;
      QList<int> _lnPages;
      bool _bInterrupt;
};

#endif
