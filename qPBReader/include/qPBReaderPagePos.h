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

#ifndef _qPBReaderPagePos_h__
#define _qPBReaderPagePos_h__

#include <QString>

class qPBReaderDoc;

class qPBReaderPagePos
{
   public :

      qPBReaderPagePos(qPBReaderDoc * ipDoc);
      ~qPBReaderPagePos();

      void Save(bool bOverwrite = true);
      void Restore();
      QString Get() const;
      void Set(const QString & isPos) const;


   private :
      qPBReaderPagePos();

      qPBReaderDoc * _pDoc;
      QString _savedPos;
      double _dFallbackPos;
};

#endif
