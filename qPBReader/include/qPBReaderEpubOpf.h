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

#ifndef _qPBReaderEpubOpf_h__
#define _qPBReaderEpubOpf_h__

#include <QString>

class qPBReaderEpubSettings;

class qPBReaderEpubOpf
{
   public :
      qPBReaderEpubOpf();
      ~qPBReaderEpubOpf();

      void Clear();

      bool Set(qPBReaderEpubSettings * ipBookSettings,
               int inOpfIdx,
               const QString & isFile);

      int GetOpfIndex() const
      {
         return _nOpfIdx;
      }

      QString GetOpfFileName() const
      {
         return _sFile;
      }

      QString GetOpfBaseDir() const
      {
         return _sBaseDir;
      }

      int GetNbSections()
      {
         return _nNbSections;
      }

      int GetCurrentSectionIndex()
      {
         return _nCurrentSection;
      }

      QString GetTitle()
      {
         return _sTitle;
      }

      bool PreviousSection();

      bool NextSection();

      bool IsFirstSection();

      bool IsLastSection();

      bool SetCurrentSection(int idx);

      QString GetCurrentSectionFile();

      bool GetSectionFileName(int idx, QString & osFileName);

      bool GetCSSFiles(QStringList & olsCssFiles);

      QString GetNCX();

      /**
       * Return section file index.
       * -1 if not found.
       */
      int GetSectionNumber(const QString & isFileName);

      bool SetCurrentSection(const QString & isFileName);

      void SavePagesCount(const QList<int> & ilnPagesCount);

      void GetPagesCount(QList<int> & olnPagesCount);

   private :

      bool RetrieveSections();

      bool RetrieveNCX();

      bool RetrieveTitle();

      qPBReaderEpubSettings * _pBookSettings;

      int _nOpfIdx;

      QString _sFile;

      QString _sBaseDir;

      int _nNbSections;

      int _nCurrentSection;

      QString _sTitle;
};

#define OPFNS "http://www.idpf.org/2007/opf"
#define DCNS  "http://purl.org/dc/elements/1.1/"

#endif
