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

#ifndef _qPBReaderTracer_h__
#define _qPBReaderTracer_h__

class QString;

class qPBReaderTraceScope
{
   public :

      qPBReaderTraceScope(const char * iTraceKey, const char * ipScopeName);
      ~qPBReaderTraceScope();
      inline int IsActive() const;
      inline const char * Name() const;

      qPBReaderTraceScope & operator << (const char * ipc);
      qPBReaderTraceScope & operator << (int n);
      qPBReaderTraceScope & operator << (unsigned int n);
      qPBReaderTraceScope & operator << (long long n);
      qPBReaderTraceScope & operator << (double d);
      qPBReaderTraceScope & operator << (void * p);
      qPBReaderTraceScope & operator << (signed char c);
      qPBReaderTraceScope & operator << (unsigned char c);
      qPBReaderTraceScope & operator << (char c);
      qPBReaderTraceScope & operator << (const QString & s);
      qPBReaderTraceScope & operator << (bool b);

   private :

      qPBReaderTraceScope();

      int _nKey;
      int _nActive;
      char * _pcName;
};

int qPBReaderTraceScope::IsActive() const
{
   return _nActive;
}

const char * qPBReaderTraceScope::Name() const
{
   return _pcName;
}

class qPBReaderBuffer
{
   public :
      const void * _pData;
      int _nLength;
      int _nWidth;
      qPBReaderBuffer(const void * ipData, int inLength, int inWidth = 16) :
         _pData(ipData), _nLength(inLength), _nWidth(inWidth)
      {
      }
};

qPBReaderTraceScope & operator << (qPBReaderTraceScope  & tr,
                                   const qPBReaderBuffer & buf);

#define TRSCOPE(key, name) qPBReaderTraceScope __tr(#key, name)

#define TRACE __tr
#define TRACEON __tr.IsActive()
#define TRANAME __tr.Name()
#define ENC(exp) " [" << exp << "] "
#define VAR(v) #v "=" << v << " "
#define HEXDUMP(p, len) qPBReaderBuffer(p, len)
#define HEXDUMPW(p, len, w) qPBReaderBuffer(p, len, w)
#define endl '\n'

#endif
