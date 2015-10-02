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

#ifndef _qPBReaderUtils_h__
#define _qPBReaderUtils_h__

class QString;

class qPBReaderUtils
{
   public :

      /**
       * Creates directory for a given file.
       */
      static bool MkdirFor(const QString & iFile);

      /**
       * Removes a directory.
       * Directory path can be either relative or absolute, and might not be
       * empty.
       */
      static bool Rmdir(const QString & iPath);

      /**
       * Computes file MD5.
       */
      static QString MD5(const QString & isFileName);


      /**
       * Get a clean file path.
       * For absolute pathes only, removes ..
       */
      static QString CleanPath(const QString & isFileName);
};

#endif
