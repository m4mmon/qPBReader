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

#include "qPBReaderConfig.h"

// qt
#include <QDesktopServices>
#include <QSettings>

// project
#include "qPBReaderPlatform.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

// PB directories
#define CONF_FILE     "/config/qPBReader.conf"
#define APP_DIR       "/share/qPBReader/"
#define APP_DATA      APP_DIR "data"
#define APP_TEMP      "/qPBReaderTmp"
#define MATHJAX_DIR   APP_DIR "resources/mathjax"

// config keys
#define APP           "qPBReader/"
#define DATA_DIR_KEY  APP "datadir"
#define TEMP_DIR_KEY  APP "tempdir"
#define ZOOM_KEY      APP "zoom"
#define STDFONT_KEY   APP "standardfont"
#define SERIFFNT_KEY  APP "serif"
#define SANSFNT_KEY   APP "sans"
#define MONOFNT_KEY   APP "mono"
#define IGNOREFNT_KEY APP "ignorefont"
#define BOOKMRGN_KEY  APP "bookmargin"
#define MRGNTOP_KEY   APP "defaultMarginTop"
#define MRGNSIDE_KEY  APP "defaultMarginSide"
#define MRGNBOT_KEY   APP "defaultMarginBottom"
#define HYPHEN_KEY    APP "hyphenLang"
#define LANG_KEY      APP "applicationLang"

// -----------------------------------------------------------------------------

qPBReaderConfig & qPBReaderConfig::Get()
{
   static qPBReaderConfig cfg;
   return cfg;
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetStrValue(const QString & isKey,
                                     const QString & isFallbackValue)
{
   TRSCOPE(cfg, "qPBReaderConfig::GetStrValue");

   TRACE << "Getting default value for" << ENC(isKey)
         << "fallback" << ENC(isFallbackValue) << endl;

   QString sVal = Settings().value(isKey).toString();

   if (sVal.isEmpty())
   {
      TRACE << "Not found, saving fallback value in settings" << endl;
      sVal = isFallbackValue;
      Settings().setValue(isKey, sVal);
   }

   TRACE << ENC(sVal) << endl;

   return sVal;
}

// -----------------------------------------------------------------------------

qPBReaderConfig::qPBReaderConfig() :
   _pSettings(0)
{
   TRSCOPE(cfg, "qPBReaderConfig::qPBReaderConfig");

   // try to use environment-provided

   QString sConfFile = getenv("qPBReaderConfFile");

   // or default

   if (sConfFile.isEmpty())
   {
      sConfFile =
         QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
         CONF_FILE
      ;
   }

   TRACE << "config file " << ENC(sConfFile) << endl;

   _pSettings = new QSettings(sConfFile, QSettings::IniFormat);
}

// -----------------------------------------------------------------------------

qPBReaderConfig::~qPBReaderConfig()
{
   TRSCOPE(cfg, "qPBReaderConfig::~qPBReaderConfig");
   if (_pSettings)
   {
      delete _pSettings;
      _pSettings = 0;
   }
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetDataDir()
{
   return GetStrValue(
      DATA_DIR_KEY,
      QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
      APP_DATA
   );
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetTempDir()
{
   return GetStrValue(
      TEMP_DIR_KEY,
      QDesktopServices::storageLocation(QDesktopServices::TempLocation) +
      APP_TEMP
   );
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetMathjaxDir()
{
   return QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
          MATHJAX_DIR;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultMagnificationFactor(double idFactor)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultMagnificationFactor");
   TRACE << "Setting: " << idFactor << endl;
   Settings().setValue(ZOOM_KEY, idFactor);
}

// -----------------------------------------------------------------------------

double qPBReaderConfig::GetDefaultMagnificationFactor()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultMagnificationFactor");

   double factor = Settings().value(ZOOM_KEY, -1.).toDouble();

   if (factor <= 0.)
   {
      TRACE << "Not found, saving default value in settings" << endl;
      factor = 1.4;
      SetDefaultMagnificationFactor(factor);
   }

   TRACE << factor << endl;

   return factor;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultStandardFont(int idx)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultStandardFont");
   TRACE << "Setting: " << idx << endl;
   if (idx >= 0 && idx <= 2)
   {
      Settings().setValue(STDFONT_KEY, idx);
   }
}

// -----------------------------------------------------------------------------

int qPBReaderConfig::GetDefaultStandardFont()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultStandardFont");


   int nDefaultFont = -1;

   bool bHasDefaultFontSetting = Settings().contains(STDFONT_KEY);

   if (bHasDefaultFontSetting)
   {
      nDefaultFont = Settings().value(STDFONT_KEY).toInt();
   }

   if (!bHasDefaultFontSetting || nDefaultFont < 0 || nDefaultFont > 2)
   {
      TRACE << "Not found (or invalid), saving default value in settings" << endl;
      int nDefaultFont = 0;
      SetDefaultStandardFont(nDefaultFont);
   }

   TRACE << nDefaultFont << endl;

   return nDefaultFont;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultSerifFont(const QString & isFont)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultSerifFont");
   TRACE << VAR(isFont) << endl;
   Settings().setValue(SERIFFNT_KEY, isFont);
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetDefaultSerifFont()
{
   return GetStrValue(SERIFFNT_KEY, "Linux Libertine");
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultSansFont(const QString & isFont)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultSansFont");
   TRACE << VAR(isFont) << endl;
   Settings().setValue(SANSFNT_KEY, isFont);
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetDefaultSansFont()
{
   return GetStrValue(SANSFNT_KEY, "Liberation Sans");
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultMonoFont(const QString & isFont)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultMonoFont");
   TRACE << VAR(isFont) << endl;
   Settings().setValue(MONOFNT_KEY, isFont);
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetDefaultMonoFont()
{
   return GetStrValue(MONOFNT_KEY, "Linux Libertine Mono");
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultIgnoreFonts(bool ibIgnoreFonts)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultIgnoreFonts");
   TRACE << VAR(ibIgnoreFonts) << endl;
   Settings().setValue(IGNOREFNT_KEY, ibIgnoreFonts);
}

// -----------------------------------------------------------------------------

bool qPBReaderConfig::GetDefaultIgnoreFonts()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultIgnoreFonts");


   bool bIgnoreFonts = false;

   if (Settings().contains(IGNOREFNT_KEY))
   {
      bIgnoreFonts = Settings().value(IGNOREFNT_KEY).toBool();
   }

   else
   {
      TRACE << "Not found, saving default value in settings" << endl;
      SetDefaultIgnoreFonts(bIgnoreFonts);
   }

   TRACE << bIgnoreFonts << endl;
   return bIgnoreFonts;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultUseBookMargin(bool ibUseBookMargin)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultUseBookMargin");
   TRACE << VAR(ibUseBookMargin) << endl;
   Settings().setValue(BOOKMRGN_KEY, ibUseBookMargin);
}

// -----------------------------------------------------------------------------

bool qPBReaderConfig::GetDefaultUseBookMargin()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultUseBookMargin");


   bool bUseBookMargin = true;

   if (Settings().contains(BOOKMRGN_KEY))
   {
      bUseBookMargin = Settings().value(BOOKMRGN_KEY).toBool();
   }

   else
   {
      TRACE << "Not found, saving default value in settings" << endl;
      SetDefaultUseBookMargin(bUseBookMargin);
   }

   TRACE << bUseBookMargin << endl;
   return bUseBookMargin;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultTopMargin(int iTop)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultTopMargin");
   TRACE << VAR(iTop) << endl;
   Settings().setValue(MRGNTOP_KEY, iTop);
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultSideMargin(int iSide)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultSideMargin");
   TRACE << VAR(iSide) << endl;
   Settings().setValue(MRGNSIDE_KEY, iSide);
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultBottomMargin(int iBottom)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultBottomMargin");
   TRACE << VAR(iBottom) << endl;
   Settings().setValue(MRGNBOT_KEY, iBottom);
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::GetDefaultMargins(int & oTop, int & oSide, int & oBottom)
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultMargins");


   // default values

   int top = 4;
   int side = 18;
   int bottom = 0;

   if (Settings().contains(MRGNTOP_KEY))
   {
      top = Settings().value(MRGNTOP_KEY).toInt();
   }

   else
   {
      TRACE << "Default top margin not found, saving default value in settings" <<
            endl;
      SetDefaultTopMargin(top);
   }

   if (Settings().contains(MRGNSIDE_KEY))
   {
      side = Settings().value(MRGNSIDE_KEY).toInt();
   }

   else
   {
      TRACE << "Default side margin not found, saving default value in settings" <<
            endl;
      SetDefaultSideMargin(side);
   }

   if (Settings().contains(MRGNBOT_KEY))
   {
      bottom = Settings().value(MRGNBOT_KEY).toInt();
   }

   else
   {
      TRACE << "Default bottom margin not found, saving default value in settings" <<
            endl;
      SetDefaultBottomMargin(bottom);
   }

   oTop = top;
   oSide = side;
   oBottom = bottom;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetDefaultHyphenPatternLang(const QString & isLang)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetDefaultHyphenPatternLang");
   TRACE << VAR(isLang) << endl;
    Settings().setValue(HYPHEN_KEY, isLang);
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetDefaultHyphenPatternLang()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetDefaultHyphenPatternLang");

   QString sPatternLang = Settings().value(HYPHEN_KEY).toString();

   TRACE << ENC(sPatternLang) << endl;

   return sPatternLang;
}

// -----------------------------------------------------------------------------

void qPBReaderConfig::SetApplicationLanguage(const QString & isLang)
{
   TRSCOPE(cfg, "qPBReaderConfig::SetApplicationLanguage");
   TRACE << VAR(isLang) << endl;
    Settings().setValue(LANG_KEY, isLang);
}

// -----------------------------------------------------------------------------

QString qPBReaderConfig::GetApplicationLanguage()
{
   TRSCOPE(cfg, "qPBReaderConfig::GetApplicationLanguage");

   QString sLang;

   if (Settings().contains(LANG_KEY))
   {
      sLang = Settings().value(LANG_KEY).toString();
   }
   else
   {
      sLang = qPBReaderPlatform::LocaleName();
      SetApplicationLanguage(sLang);
   }

   return sLang;
}

// -----------------------------------------------------------------------------
