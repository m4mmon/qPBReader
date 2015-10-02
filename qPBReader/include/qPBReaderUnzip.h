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

#ifndef _qPBReaderUnzip_h__
#define _qPBReaderUnzip_h__

#include <QString>
#include "unzip.h"

/**
 * Unzip class.
 */
class qPBReaderUnzip
{
   public :

      static bool ExtractAllTo(const QString & isFile,
                               const QString & isDir);


      /**
       * Constructor.
       */
      qPBReaderUnzip(const QString & isFile);

      /**
       * Destructor.
       */
      ~qPBReaderUnzip();

      /**
       * Opens the archive.
       */
      bool Open();

      /**
       * Checks if the archive is opened.
       */
      inline bool Opened() const;

      /**
       * Close the archive.
       */
      void Close();

      /**
       * Gets the number of members in the archive.
       */
      unsigned int GetNumberOfMembers();

      /**
       * After opening, archive is set on first member.
       * To go to next member, call this.
       * So to parse all members, this method has to be called
       * GetNumberOfMembers() - 2 times.
       */
      bool NextMember();

      /**
       * Gets the filename of the current member.
       */
      QString GetCurrentMemberFileName();

      /**
       * True if the member is a directory (I checked some implementations,
       * they check if the filename ends with a directory separator.
       */
      bool IsCurrentMemberDir();

      /**
       * Writes the current member to disk as iDst + GetCurrentMemberFileName();
       */
      bool WriteCurrentMember(const QString & iDst);

      /**
       * Writes the current member as iDst.
       */
      bool WriteCurrentMemberAs(const QString & iDst);

      /**
       * Opens current member.
       */
      bool OpenCurrentMember();

      /**
       * Closes current member.
       */
      bool CloseCurrentMember();

   private :

      /**
       * Forbidden constructor.
       */
      qPBReaderUnzip();

      /**
       * Archive filename.
       */
      QString _sFile;

      /**
       * Current member name.
       */
      QString _sCurrMemberName;

      /**
       * Flag indicating if current member is opened.
       */
      bool _bCurrMemberOpened;

      /**
       * Archive handle.
       */
      unzFile _f;
};

bool qPBReaderUnzip::Opened() const
{
   return _f != 0;
}

#endif
