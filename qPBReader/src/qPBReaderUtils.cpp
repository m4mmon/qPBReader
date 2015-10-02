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

#include "qPBReaderUtils.h"

// qt
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>

// project
#include "qPBReaderTracer.h"
#include "qPBReaderUnzip.h"

// -----------------------------------------------------------------------------

bool qPBReaderUtils::MkdirFor(const QString & iFile)
{
   TRSCOPE(utils, "qPBReaderUtils::MkdirFor");

   QFileInfo fi(iFile);
   bool b = QDir().mkpath(fi.dir().path());

   TRACE << "Directory creation for " << ENC(iFile) << ": " << b << endl;

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderUtils::Rmdir(const QString & iPath)
{
   TRSCOPE(utils, "qPBReaderUtils::Rmdir");

   TRACE << "Removing " << ENC(iPath) << endl;

   bool b = true;

   QDir dir(iPath);

   if (dir.exists())
   {
      Q_FOREACH (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
                 QDir::System |
                 QDir::Hidden |
                 QDir::AllDirs |
                 QDir::Files,
                 QDir::DirsFirst))
      {
         QString sCurEntry = info.absoluteFilePath();

         // recursively remove child directory

         if (info.isDir())
         {
            b = Rmdir(sCurEntry);
         }

         // file

         else
         {
            TRACE << "Removing file " << ENC(sCurEntry) << endl;
            b = QFile::remove(sCurEntry);
         }

         if (!b)
         {
            break;
         }
      }

      // and remove directory

      if (b)
      {
         b = QDir().rmdir(iPath);
      }
   }

   TRACE << "Delete succeeded: " << b << endl;

   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderUtils::MD5(const QString & isFileName)
{
   TRSCOPE(utils, "qPBReaderUtils::MD5");

   TRACE << ENC(isFileName) << endl;

   QString sMd5;

   // open file

   QFile f(isFileName);

   if (f.exists() && f.open(QIODevice::ReadOnly))
   {
      qint64 nSize = f.size();

      // map into memory

      uchar * pMap = f.map(0, nSize);

      // compute md5

      if (pMap)
      {
         QCryptographicHash hash(QCryptographicHash::Md5);
         hash.addData((const char *)pMap, nSize);
         sMd5 = hash.result().toHex();

         // and clean map

         f.unmap(pMap);
         pMap = 0;
      }

      else
      {
         TRACE << "File not mapped!" << endl;
      }

      f.close();
   }

   else
   {
      TRACE << "File not found or could not be opened for reading" << endl;
   }

   TRACE << ENC(sMd5) << endl;

   return sMd5;
}

// -----------------------------------------------------------------------------

QString qPBReaderUtils::CleanPath(const QString & isFileName)
{
   TRSCOPE(utils, "qPBReaderUtils::CleanPath");

   return QFileInfo(isFileName).absoluteFilePath();
}

// -----------------------------------------------------------------------------
