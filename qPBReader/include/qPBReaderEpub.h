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

#ifndef _qPBReaderEpub_h__
#define _qPBReaderEpub_h__

#include <QString>

class qPBReaderEpubSettings;
class qPBReaderEpubOpf;

/**
 * Sloppy epub representation.
 */
class qPBReaderEpub
{
   public :

      qPBReaderEpub(const QString & isEpubFile);

      ~qPBReaderEpub();

      /**
       * Opens the epub.
       *
       */
      bool Open();

      bool Close();

      /**
       * Gets the directory where the epub has been extracted.
       * Open must have succeeded in order to get a valid value.
       */
      inline QString GetExtractedDirectory() const;

      inline QString GetFile() const;

      /**
       * Get the number of OEB opf declared in container.
       */
      int GetNbOpf() const;

      /**
       * Gets the current OEB opf index.
       */
      int GetCurrentOpf() const;

      bool GetCurrentOpfFileName(QString & osFileName);
      QString GetCurrentOpfFileName();

      /**
       * Sets the current OEB container.
       * Open must have succeeded in order to get a valid value.
       * From 0 to GetNbOpf() - 1.
       * Returns true if valid index has been provided.
       */
      bool SetCurrentOpf(int nOpfNum);

      /**
       * Gets the number of sections in the current OEB opf.
       * Open must have succeeded in order to get a valid value.
       */
      int GetNbSections() const;

      int GetCurrentSectionIndex();


      bool GetSectionFiles(QStringList & ols);


      QString GetCurrentSectionFile();

      /**
       * Gets the path to the inSectionIndex-th section file.
       * Open must have succeeded in order to get a valid value.
       * From 0 to GetNbSections() - 1.
       * Returns true if valid index has been provided.
       */
      bool GetSectionFile(int inSectionIndex, QString & osFile);

      /**
       * Gets the path to the inSectionIndex-th section file.
       * Open must have succeeded in order to get a valid value.
       * From 0 to GetNbSections() - 1.
       */
      QString GetSectionFile(int inSectionIndex);

      /**
       * Switches to next section.
       * If return is true, next section file name can be obtained by calling
       * GetCurrentSectionFile().
       */
      bool PreviousSection();

      /**
       * Switches to previous section.
       * If return is true, previous section file name can be obtained by
       * calling GetCurrentSectionFile().
       */
      bool NextSection();

      QString GetNCX();

      QString GetTitle();

      bool GetHTMLToc(QString & osHTMLToc);

      /**
       * Changes section provided a file name.
       * If the file is an existing section, swithc to it and return true.
       */
      bool SwitchToSectionByFileName(const QString & isFile);


      void SavePagesCount(const QList<int> & ilnPagesCount);
      void GetPagesCount(QList<int> & olnPagesCount);


      /**
       * Saves page position in current section.
       */
      void SavePos(double val);

      /**
       * Gets page position in current section.
       */
      double GetPos();

      /**
       * Saves zoom level applied to current book.
       */
      void SaveMagnification(double val);

      /**
       * Gets zoom level applied to current book.
       */
      double GetMagnification();

      void SaveStandardFont(int i);
      int GetStandardFont();

      QString GetSerifFamily();
      QString GetSansFamily();
      QString GetMonoFamily();

      void SaveSerifFamily(const QString & isFamily);
      void SaveSansFamily(const QString & isFamily);
      void SaveMonoFamily(const QString & isFamily);

      bool GetIgnoreFonts();
      void SaveIgnoreFonts(bool ibIgnoreFonts);

      bool UseBookMargin();
      void SaveUseBookMargin(bool ibUseBookMargin);

      void GetMargins(int & oTop, int & oSide, int & oBottom);
      void SaveMargins(int iTop, int iSide, int iBottom);

      QString GetHyphenPatternLang();
      void SaveHyphenPatternLang(const QString & isLang);

   private :

      /**
       * Forbidden constructor.
       */
      qPBReaderEpub();

      /**
       * The source file.
       */
      QString _sFile;

      /**
       * Source file MD5.
       */
      QString _sMD5;

      /**
       * Temporary working directory.
       * Where the epub is extracted.
       * <TempDir>/MD5
       */
      QString _sTempDir;

      /**
       * Current book settings file.
       */
      qPBReaderEpubSettings * _pSettings;

      /**
       * In-use opf.
       */
      qPBReaderEpubOpf * _pCurrentOpf;

      /**
       * Removes temporary files and resets internal stuff.
       */
      void Clean();

      /**
       * Initializes settings.
       */
      bool InitSettings(const QString & isFileName = "");

      /**
       * Retrieves suitable OEB.
       */
      bool AnalyzeDocStructure();

      /**
       * Active opf index.
       */
      int _nActiveOpf;

      /**
       * Retrieves OPF files corresponding to suitable OEB.
       */
      bool RetrieveOEB(QStringList & olsOpf);

   public:
      /**
       * Retrieves all css files declared in current opf.
       */
      bool RetrieveCSS(QStringList & olsCssFiles);
};


QString qPBReaderEpub::GetExtractedDirectory() const
{
   return _sTempDir;
}

QString qPBReaderEpub::GetFile() const
{
   return _sFile;
}

#endif
