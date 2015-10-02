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

#ifndef _qPBReaderEpubSettings_h__
#define _qPBReaderEpubSettings_h__

#include <QSettings>

class qPBReaderEpubSettings : public QSettings
{
   public :
      qPBReaderEpubSettings(const QString & iFileName);
      int GetArraySize(const QString & isArrayName);
      QString GetArrayValue(const QString & isArrayName,
                            const QString & isItemsName,
                            int idx);
      double GetDoubleValue(const QString & isKey, double iDefault = -1.);
      int GetIntValue(const QString & isKey, int iDefault = -1);

      // opf list management

      int GetNbOpf();
      void SaveCurrentOpf(int n);
      int GetCurrentOpf();
      QString GetOpfFile(int n);
      bool GetOpf(QStringList & olsOpf);
      void SaveOpf(const QStringList & ilsOpf);

      // opf management

      int GetNbSectionsInOpf(int iOpfIdx);
      int GetCurrentSectionInOpf(int iOpfIdx);
      void SaveCurrentSectionInOpf(int iOpfIdx, int iSectionIdx);
      QString GetSectionFileName(int iOpfIdx, int iSectionIdx);
      void SaveSectionsInOpf(int iOpfIdx, const QStringList & ilsSections);
      void SavePagesCountInOpf(int iOpfIdx, const QList<int> & ilnPagesCount);
      void GetPagesCountInOpf(int iOpfIdx, QList<int> & olnPagesCount);
      void SaveTitle(int iOpfIdx, const QString & isTitle);
      QString GetTitle(int iOpfIdx);
      QString GetNCXFile(int iOpfIdx);
      void SaveNCXFile(int iOpfIdx, const QString & isNCX);

      // book stuff

      void SaveEpubFileName(const QString & isEpubFile);
      void SavePagePos(double pos);
      double GetPagePos();
      void SaveMagnification(double val);
      double GetMagnification();
      int GetStandardFont();
      void SaveStandardFont(int i);
      bool HasStandardFont();
      QString GetSerifFamily();
      bool HasSerifFamily();
      QString GetSansFamily();
      bool HasSansFamily();
      QString GetMonoFamily();
      bool HasMonoFamily();
      void SaveSerifFamily(const QString & isFamily);
      void SaveSansFamily(const QString & isFamily);
      void SaveMonoFamily(const QString & isFamily);
      bool IgnoreFontsDefined();
      bool GetIgnoreFonts();
      void SaveIgnoreFonts(bool ibIgnoreFonts);

      bool UseBookMarginDefined();
      bool UseBookMargin();
      void SaveUseBookMargin(bool ibUseBookMargin);
      bool GetMarginsDefined();
      void GetMargins(int & oTop, int & oSide, int & oBottom);
      void SaveMargins(int iTop, int iSide, int iBottom);

      bool HyphenPatternLangDefined();
      QString GetHyphenPatternLang();
      void SaveHyphenPatternLang(const QString & isLang);

   private :
      qPBReaderEpubSettings();
};

#endif
