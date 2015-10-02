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

#include "qPBReaderTracer.h"
#undef endl

// std
#include <iomanip>
#include <iostream>
using namespace std;

// qt
#include <QFile>
#include <QStringList>
#include <QTextStream>

// -----------------------------------------------------------------------------

class qPBReaderTrace
{
   public :

      qPBReaderTrace()
      {
         QString sDecFic = qgetenv("TraKeysDir");

         if (sDecFic.isEmpty())
         {
            sDecFic = ".";
         }

         sDecFic += "/trakeys";

         cerr << "expected trakey file is [" << sDecFic.toLocal8Bit().data() << "]" << endl;

         QFile f(sDecFic);

         if (f.exists() && f.open(QIODevice::ReadOnly | QIODevice::Text))
         {
            QTextStream stream(&f);

            while (!stream.atEnd())
            {
               QString line = stream.readLine();
               QStringList ls = line.split(" ");
               int nb = ls.size();

               for (int idx = 0; idx < nb; idx++)
               {
                  Register(ls[idx], 1);
               }
            }
         }
      }

      ~qPBReaderTrace()
      {
      }

      int Register(const char * iTraceKey)
      {
         return Register(QString(iTraceKey), 0);
      }

      int Active(int inKey)
      {
         int nActive = 0;

         if (inKey >= 0 && inKey < _lnActive.size())
         {
            nActive = _lnActive[inKey];
         }

         return nActive;
      }

      void TraceIn(int inKey, const char * pcName)
      {
         if (!_nEOL)
         {
            Endl();
         }

         Prefix(inKey);
         cerr << pcName << "() {";
         Endl();
         _nDepth++;
      }

      void TraceOut(int inKey)
      {
         if (!_nEOL)
         {
            Endl();
         }

         _nDepth--;
         Prefix(inKey);
         cerr << "}";
         Endl();
      }

      void Print(int inKey, const char * ipc)
      {
         if (ipc)
         {
            const char * n1 = ipc;
            const char * n2 = 0;

            while ((n2 = strchr(n1, '\n')) != 0)
            {
               Prefix(inKey);
               cerr.write(n1, n2 - n1);
               Endl();
               n1 = n2 + 1;
            }

            if (*n1)
            {
               Prefix(inKey);
               cerr << n1;
            }
         }

         else
         {
            cerr << "<null>";
         }
      }

      void Print(int inKey, int n)
      {
         Prefix(inKey);
         cerr << n;
      }

      void Print(int inKey, unsigned int n)
      {
         Prefix(inKey);
         cerr << n;
      }

      void Print(int inKey, long long n)
      {
         Prefix(inKey);
         cerr << n;
      }

      void Print(int inKey, double d)
      {
         Prefix(inKey);
         char buf[16];
         sprintf(buf, "%f", d);
         cerr << buf;
      }

      void Print(int inKey, void * p)
      {
         Prefix(inKey);
         char buf[16];
         sprintf(buf, "0x%08x", (unsigned int)p);
         cerr << buf;
      }

      void Print(int inKey, signed char c)
      {
         Prefix(inKey);

         if (c == '\n')
         {
            Endl();
         }

         else
         {
            cerr << c;
         }
      }

      void Print(int inKey, unsigned char c)
      {
         Prefix(inKey);

         if (c == '\n')
         {
            Endl();
         }

         else
         {
            cerr << c;
         }
      }

      void Print(int inKey, char c)
      {
         Prefix(inKey);

         if (c == '\n')
         {
            Endl();
         }

         else
         {
            cerr << c;
         }
      }

      void Endl()
      {
         cerr << endl << flush;
         _nEOL = 1;
      }

   private :

      int Register(const QString & isKey, int nActive)
      {
         // key are 16 chars max

         QString sKey = isKey.left(16);

         // if not already registered

         int nPos = _lsTraceKeys.indexOf(sKey);

         if (nPos < 0)
         {
            // register trace key

            _lsTraceKeys.append(sKey);
            _lnActive.append(nActive);

            nPos = _lsTraceKeys.size() - 1;
         }

         return nPos;
      }

      void Prefix(int inKey)
      {
         if (_nEOL && inKey >= 0 && inKey < _lsTraceKeys.size())
         {
            cerr << setiosflags(ios::left) << setw(17) <<
                 _lsTraceKeys[inKey].toLocal8Bit().data();

            for (int idx = 0; idx < _nDepth; idx++)
            {
               cerr << ": ";
            }

            _nEOL = 0;
         }
      }

      QStringList _lsTraceKeys;
      QList<int> _lnActive;

      static int _nDepth;
      static int _nEOL;
};

int qPBReaderTrace::_nDepth = 0;
int qPBReaderTrace::_nEOL = 1;

static qPBReaderTrace tr;

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderTraceScope::qPBReaderTraceScope(const char * iTraceKey,
      const char * ipScopeName) :
   _nKey(-1), _nActive(0), _pcName(0)
{
   if (iTraceKey && *iTraceKey && ipScopeName && *ipScopeName)
   {
      _nKey = tr.Register(iTraceKey);
      _nActive = tr.Active(_nKey);
      _pcName = strdup(ipScopeName);

      if (_nActive)
      {
         tr.TraceIn(_nKey, _pcName);
      }
   }
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope::~qPBReaderTraceScope()
{
   if (_pcName)
   {
      free(_pcName);
      _pcName = 0;
   }

   if (_nActive)
   {
      tr.TraceOut(_nKey);
   }
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (const char * ipc)
{
   if (_nActive)
   {
      tr.Print(_nKey, ipc);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (int n)
{
   if (_nActive)
   {
      tr.Print(_nKey, n);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (unsigned int n)
{
   if (_nActive)
   {
      tr.Print(_nKey, n);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (long long n)
{
   if (_nActive)
   {
      tr.Print(_nKey, n);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (double d)
{
   if (_nActive)
   {
      tr.Print(_nKey, d);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (void * p)
{
   if (_nActive)
   {
      tr.Print(_nKey, p);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (signed char c)
{
   if (_nActive)
   {
      tr.Print(_nKey, c);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (unsigned char c)
{
   if (_nActive)
   {
      tr.Print(_nKey, c);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (char c)
{
   if (_nActive)
   {
      tr.Print(_nKey, c);
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (const QString & s)
{
   if (_nActive)
   {
      tr.Print(_nKey, s.toLocal8Bit().data());
   }

   return *this;
}

// -----------------------------------------------------------------------------

qPBReaderTraceScope & qPBReaderTraceScope::operator << (bool b)
{
   if (_nActive)
   {
      tr.Print(_nKey, (b ? "true" : "false"));
   }

   return *this;
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

qPBReaderTraceScope::qPBReaderTraceScope() :
   _nKey(-1), _nActive(0), _pcName(0)
{
}

// -----------------------------------------------------------------------------
// global operators
// -----------------------------------------------------------------------------

qPBReaderTraceScope & operator << (qPBReaderTraceScope & t,
                                   const qPBReaderBuffer & dmp)
{
   // Test trace activation

   if (t.IsActive())
   {
      char buf [16];

      // If no data

      if (!dmp._pData)
      {
         t << "(null)";
      }

      else
      {
         // Data pointer value

         sprintf(buf, "0x%08X:", (int)dmp._pData);
         t << buf << "\n<start of dump>\n";

         // For each line of data

         for (int offset = 0; offset < dmp._nLength; offset += dmp._nWidth)
         {
            // Display the offset of the first byte of the row

            sprintf(buf, "%08X", offset);
            t << buf << " :";

            // Hexadecimal view

            {
               // For each byte on this line

               for (int j = 0; j < dmp._nWidth; j++)
               {
                  if (!(j % 4))
                  {
                     t << " ";
                  }

                  // If data length exceeded, print dots

                  if (offset + j >= dmp._nLength)
                  {
                     t << ".. ";
                  }

                  // There is a byte there

                  else
                  {
                     // Display byte hexadecimal value

                     sprintf(buf, "%02X ",
                             ((const unsigned char *)dmp._pData)[offset + j]);
                     t << buf;
                  }
               }
            }

            // Separator column

            t << "| ";

            // Ascii view

            {
               // For each byte in this line (stopping before end of block)

               for (int j = 0; j < dmp._nWidth && offset + j < dmp._nLength; j++)
               {
                  // Get byte

                  char c = ((const char *)dmp._pData)[offset + j];

                  // If character is printable

                  if (isprint(c))
                  {
                     t << c;
                  }

                  // Replace not printable character with a dot

                  else
                  {
                     t << '.';
                  }
               }
            }

            // End of line

            t << '\n';
         }

         // End of dump

         t << "<End of dump>";
      }
   }

   return t;
}

// -----------------------------------------------------------------------------
