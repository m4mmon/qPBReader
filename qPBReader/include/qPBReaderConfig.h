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

#ifndef _qPBReaderConfig_h__
#define _qPBReaderConfig_h__

class QSettings;
class QString;

class qPBReaderConfig
{
   public :

      static QString GetDataDir();

      static QString GetTempDir();

      static QString GetMathjaxDir();

      static void SetDefaultMagnificationFactor(double idFactor);
      static double GetDefaultMagnificationFactor();

      static void SetDefaultStandardFont(int idx);
      static int GetDefaultStandardFont();

      static void SetDefaultSerifFont(const QString & isFont);
      static QString GetDefaultSerifFont();

      static void SetDefaultSansFont(const QString & isFont);
      static QString GetDefaultSansFont();

      static void SetDefaultMonoFont(const QString & isFont);
      static QString GetDefaultMonoFont();

      static void SetDefaultIgnoreFonts(bool ibIgnoreFonts);
      static bool GetDefaultIgnoreFonts();

      static void SetDefaultUseBookMargin(bool ibUseBookMargin);
      static bool GetDefaultUseBookMargin();

      static void SetDefaultTopMargin(int iTop);
      static void SetDefaultSideMargin(int iSide);
      static void SetDefaultBottomMargin(int iBottom);
      static void GetDefaultMargins(int & oTop, int & oSide, int & oBottom);

      static void SetDefaultHyphenPatternLang(const QString & isLang);
      static QString GetDefaultHyphenPatternLang();

      static void SetApplicationLanguage(const QString & isLang);
      static QString GetApplicationLanguage();

   private :

      /**
       * Gets singleton.
       */
      static qPBReaderConfig & Get();

      static QString GetStrValue(const QString & isKey,
                                 const QString & isFallbackValue);

      static QSettings & Settings()
      {
         return *(Get()._pSettings);
      }

      qPBReaderConfig();
      ~qPBReaderConfig();

      QSettings * _pSettings;
};

#endif
