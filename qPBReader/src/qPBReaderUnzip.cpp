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

#include "qPBReaderUnzip.h"

// qt
#include <QDir>

// project
#include "qPBReaderTracer.h"
#include "qPBReaderUtils.h"

// -----------------------------------------------------------------------------

// extracted data buffer size used when saving contents to disk
#define CHUNK_SIZE 65536

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::ExtractAllTo(const QString & isFile,
                                  const QString & isDir)
{
   qPBReaderUnzip zip(isFile);

   bool b = zip.Open();

   int n = zip.GetNumberOfMembers();

   for (int idx = 0; b && idx < n; idx++)
   {
      b = zip.WriteCurrentMember(isDir);

      if (b && idx + 1 < n)
      {
         b = zip.NextMember();
      }
   }

   return b;
}

// -----------------------------------------------------------------------------

qPBReaderUnzip::qPBReaderUnzip()
{
}

// -----------------------------------------------------------------------------

qPBReaderUnzip::qPBReaderUnzip(const QString & isFile) :
   _sFile(isFile), _bCurrMemberOpened(false), _f(0)
{
   TRSCOPE(zip, "qPBReaderUnzip::qPBReaderUnzip");
   TRACE << ENC(_sFile) << endl;
}

// -----------------------------------------------------------------------------

qPBReaderUnzip::~qPBReaderUnzip()
{
   TRSCOPE(zip, "qPBReaderUnzip::~qPBReaderUnzip");
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::Open()
{
   TRSCOPE(zip, "qPBReaderUnzip::Open");

   Close();
   _f = unzOpen64(_sFile.toUtf8().data());

   return Opened();
}

// -----------------------------------------------------------------------------

void qPBReaderUnzip::Close()
{
   TRSCOPE(zip, "qPBReaderUnzip::Close");

   if (Opened())
   {
      unzClose(_f);
      _f = 0;
   }

   _sCurrMemberName.clear();
   _bCurrMemberOpened = false;
}

// -----------------------------------------------------------------------------

unsigned int qPBReaderUnzip::GetNumberOfMembers()
{
   TRSCOPE(zip, "qPBReaderUnzip::GetNumberOfMembers");

   unsigned int nb = 0;
   unz_global_info64 gi;

   if (Opened() && unzGetGlobalInfo64(_f, &gi) == UNZ_OK)
   {
      nb = gi.number_entry;
   }

   TRACE << VAR(nb) << endl;
   return nb;
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::NextMember()
{
   TRSCOPE(zip, "qPBReaderUnzip::NextMember");

   _sCurrMemberName.clear();
   _bCurrMemberOpened = false;

   bool b = Opened() && unzGoToNextFile(_f) == UNZ_OK;

   TRACE << VAR(b) << endl;
   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderUnzip::GetCurrentMemberFileName()
{
   TRSCOPE(zip, "qPBReaderUnzip::GetCurrentMemberFileName");

   if (Opened() && _sCurrMemberName.isEmpty())
   {
      unz_file_info64 fi;

      // determine file name size

      if (unzGetCurrentFileInfo64(_f, &fi, 0, 0, 0, 0, 0, 0) == UNZ_OK)
      {
         // to get an appropriately-sized buffer on next call

         unsigned long nLen = fi.size_filename + 1;
         char * pc = new char[nLen];

         if (pc)
         {
            if (unzGetCurrentFileInfo64(_f, &fi, pc, nLen, 0, 0, 0, 0) == UNZ_OK)
            {
               _sCurrMemberName = pc;
            }

            delete [] pc;
            pc = 0;
         }
      }
   }

   TRACE << ENC(_sCurrMemberName) << endl;
   return _sCurrMemberName;
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::IsCurrentMemberDir()
{
   TRSCOPE(zip, "qPBReaderUnzip::IsCurrentMemberDir");

   bool b = false;

   QString tmp = GetCurrentMemberFileName();
   b = tmp.endsWith('/') || tmp.endsWith('\\');

   TRACE << b << endl;
   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::WriteCurrentMember(const QString & iDst)
{
   return WriteCurrentMemberAs(iDst + "/" + GetCurrentMemberFileName());
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::WriteCurrentMemberAs(const QString & iDst)
{
   TRSCOPE(zip, "qPBReaderUnzip::WriteCurrentMemberAs");

   TRACE << ENC(iDst) << endl;

   bool b = false;

   if (!Opened())
   {
      return b;
   }

   // create directory structure if needed

   b = qPBReaderUtils::MkdirFor(iDst);

   // directory

   if (b && IsCurrentMemberDir())
   {
      TRACE << "Directory" << endl;
      b = QDir().mkpath(iDst);
   }

   // or file

   else if (b)
   {
      TRACE << "File" << endl;
      b = OpenCurrentMember();

      QFile f(iDst);

      if (b)
      {
         b = f.open(QIODevice::WriteOnly);
      }

      {
         char buf[CHUNK_SIZE];
         int nSize = 1;

         while (b && nSize > 0)
         {
            nSize = unzReadCurrentFile(_f, buf, sizeof(buf));

            if (nSize < 0)
            {
               b = false;
            }

            else if (nSize > 0)
            {
               b = f.write(buf, nSize);
            }
         }
      }

      if (f.openMode() != QIODevice::NotOpen)
      {
         f.close();
      }

      // try to be as clean as possible in case an error occured

      bool b2 = CloseCurrentMember();

      if (b)
      {
         b = b2;
      }

   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::OpenCurrentMember()
{
   if (Opened() && !_bCurrMemberOpened)
   {
      _bCurrMemberOpened = unzOpenCurrentFile(_f) == UNZ_OK;
   }

   return _bCurrMemberOpened;
}

// -----------------------------------------------------------------------------

bool qPBReaderUnzip::CloseCurrentMember()
{
   bool b = false;

   if (Opened() && _bCurrMemberOpened)
   {
      _bCurrMemberOpened = false;
      b = unzCloseCurrentFile(_f) == UNZ_OK;
   }

   return b;
}

// -----------------------------------------------------------------------------
