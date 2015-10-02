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

#include "qPBReaderEpubSettings.h"

// qt
#include <QStringList>

// -----------------------------------------------------------------------------

#define BOOK_PREFIX  "book/"

#define EPUB_KEY      BOOK_PREFIX "epub"
#define POS_KEY       BOOK_PREFIX "pos"
#define ZOOM_KEY      BOOK_PREFIX "zoom"
#define STDFONT_KEY   BOOK_PREFIX "standardfont"
#define SERIFFONT_KEY BOOK_PREFIX "serif"
#define SANSFONT_KEY  BOOK_PREFIX "sans"
#define MONOFONT_KEY  BOOK_PREFIX "mono"
#define IGNOREFNT_KEY BOOK_PREFIX "ignorefonts"
#define USEMRGN_KEY   BOOK_PREFIX "bookmargin"
#define MRGNTOP_KEY   BOOK_PREFIX "forcedMarginTop"
#define MRGNSIDE_KEY  BOOK_PREFIX "forcedMarginSide"
#define MRGNBOT_KEY   BOOK_PREFIX "forcedMarginBottom"
#define HYPHEN_KEY    BOOK_PREFIX "hyphenLang"

#define CURR_SUBKEY   "/current"
#define PAGES_SUBKEY  "/pagesCount"
#define TITLE_SUBKEY  "/title"
#define NCX_SUBKEY    "/ncx"

#define OPF_KEY       "opf"
#define OPFS_KEY      "opfs"
#define CURR_OPF_KEY  OPFS_KEY CURR_SUBKEY
#define SECTION_KEY   "section"

// -----------------------------------------------------------------------------

qPBReaderEpubSettings::qPBReaderEpubSettings()
{
}

// -----------------------------------------------------------------------------

qPBReaderEpubSettings::qPBReaderEpubSettings(const QString & iFileName) :
   QSettings(iFileName, QSettings::IniFormat)
{
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetArraySize(const QString & isArrayName)
{
   int nb = beginReadArray(isArrayName);
   endArray();
   return nb;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetArrayValue(const QString & isArrayName,
      const QString & isItemsName,
      int idx)
{
   QString sVal;
   int nb = beginReadArray(isArrayName);

   if (idx >= 0 && idx < nb)
   {
      setArrayIndex(idx);
      sVal = value(isItemsName).toString();
   }

   endArray();
   return sVal;
}

// -----------------------------------------------------------------------------

double qPBReaderEpubSettings::GetDoubleValue(const QString & isKey,
      double iDefault)
{
   double val = iDefault;

   if (contains(isKey))
   {
      val = value(isKey).toDouble();
   }

   return val;
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetIntValue(const QString & isKey, int iDefault)
{
   int val = iDefault;

   if (contains(isKey))
   {
      val = value(isKey).toInt();
   }

   return val;
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetNbOpf()
{
   return GetArraySize(OPFS_KEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveCurrentOpf(int n)
{
   setValue(CURR_OPF_KEY, n);
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetCurrentOpf()
{
   return GetIntValue(CURR_OPF_KEY);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetOpfFile(int n)
{
   return GetArrayValue(OPFS_KEY, OPF_KEY, n);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::GetOpf(QStringList & olsOpf)
{
   bool b = true;
   olsOpf.clear();

   int nbOpf = beginReadArray(OPFS_KEY);

   for (int idx = 0; b && idx < nbOpf; idx++)
   {
      setArrayIndex(idx);
      QString tmp = value(OPF_KEY).toString();
      b = !tmp.isEmpty();
      olsOpf.append(tmp);
   }

   endArray();

   if (!b)
   {
      olsOpf.clear();
   }

   return b;
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveOpf(const QStringList & ilsOpf)
{
   int nbOpf = ilsOpf.size();
   beginWriteArray(OPFS_KEY);

   for (int idx = 0; idx < nbOpf; idx++)
   {
      setArrayIndex(idx);
      setValue(OPF_KEY, ilsOpf[idx]);
   }

   endArray();
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetNbSectionsInOpf(int iOpfIdx)
{
   return GetArraySize(OPF_KEY + QString::number(iOpfIdx));
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetCurrentSectionInOpf(int iOpfIdx)
{
   return GetIntValue(OPF_KEY + QString::number(iOpfIdx) + CURR_SUBKEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveCurrentSectionInOpf(int iOpfIdx, int iSectionIdx)
{
   setValue(OPF_KEY + QString::number(iOpfIdx) + CURR_SUBKEY, iSectionIdx);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetSectionFileName(int iOpfIdx, int iSectionIdx)
{
   return GetArrayValue(OPF_KEY + QString::number(iOpfIdx), SECTION_KEY,
                        iSectionIdx);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveSectionsInOpf(int iOpfIdx,
      const QStringList & ilsSections)
{
   int nbSections = ilsSections.size();
   beginWriteArray(OPF_KEY + QString::number(iOpfIdx));

   for (int idx = 0; idx < nbSections; idx++)
   {
      setArrayIndex(idx);
      setValue(SECTION_KEY, ilsSections[idx]);
   }

   endArray();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SavePagesCountInOpf(int iOpfIdx,
      const QList<int> & ilnPagesCount)
{
   QString s;
   int nb = ilnPagesCount.size();

   for (int idx = 0; idx < nb; idx++)
   {
      if (idx > 0)
      {
         s += ",";
      }

      s += QString::number(ilnPagesCount[idx]);
   }

   setValue(OPF_KEY + QString::number(iOpfIdx) + PAGES_SUBKEY, s);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::GetPagesCountInOpf(int iOpfIdx,
      QList<int> & olnPagesCount)
{
   olnPagesCount.clear();
   QString s = value(OPF_KEY + QString::number(iOpfIdx) + PAGES_SUBKEY).toString();

   QStringList ls = s.split(',', QString::SkipEmptyParts);

   int nb = ls.size();

   for (int idx = 0; idx < nb; idx++)
   {
      olnPagesCount.append(ls[idx].toInt());
   }
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveTitle(int iOpfIdx, const QString & isTitle)
{
   setValue(OPF_KEY + QString::number(iOpfIdx) + TITLE_SUBKEY, isTitle);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetTitle(int iOpfIdx)
{
   return value(OPF_KEY + QString::number(iOpfIdx) + TITLE_SUBKEY).toString();
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetNCXFile(int iOpfIdx)
{
   return value(OPF_KEY + QString::number(iOpfIdx) + NCX_SUBKEY).toString();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveNCXFile(int iOpfIdx, const QString & isNCX)
{
   setValue(OPF_KEY + QString::number(iOpfIdx) + NCX_SUBKEY, isNCX);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveEpubFileName(const QString & isEpubFile)
{
   setValue(EPUB_KEY, isEpubFile);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SavePagePos(double pos)
{
   setValue(POS_KEY, pos);
}

// -----------------------------------------------------------------------------

double qPBReaderEpubSettings::GetPagePos()
{
   return GetDoubleValue(POS_KEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveMagnification(double val)
{
   setValue(ZOOM_KEY, val);
}

// -----------------------------------------------------------------------------

double qPBReaderEpubSettings::GetMagnification()
{
   return GetDoubleValue(ZOOM_KEY);
}

// -----------------------------------------------------------------------------

int qPBReaderEpubSettings::GetStandardFont()
{
   return GetIntValue(STDFONT_KEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveStandardFont(int i)
{
   setValue(STDFONT_KEY, i);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::HasStandardFont()
{
   return contains(STDFONT_KEY);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetSerifFamily()
{
   return value(SERIFFONT_KEY).toString();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::HasSerifFamily()
{
   return contains(SERIFFONT_KEY);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetSansFamily()
{
   return value(SANSFONT_KEY).toString();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::HasSansFamily()
{
   return contains(SANSFONT_KEY);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetMonoFamily()
{
   return value(MONOFONT_KEY).toString();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::HasMonoFamily()
{
   return contains(MONOFONT_KEY);
}

// -----------------------------------------------------------------------------
void qPBReaderEpubSettings::SaveSerifFamily(const QString & isFamily)
{
   setValue(SERIFFONT_KEY, isFamily);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveSansFamily(const QString & isFamily)
{
   setValue(SANSFONT_KEY, isFamily);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveMonoFamily(const QString & isFamily)
{
   setValue(MONOFONT_KEY, isFamily);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::IgnoreFontsDefined()
{
   return contains(IGNOREFNT_KEY);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::GetIgnoreFonts()
{
   return value(IGNOREFNT_KEY).toBool();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveIgnoreFonts(bool ibIgnoreFonts)
{
   setValue(IGNOREFNT_KEY, ibIgnoreFonts);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::UseBookMarginDefined()
{
   return contains(USEMRGN_KEY);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::UseBookMargin()
{
   return value(USEMRGN_KEY, true).toBool();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveUseBookMargin(bool ibUseBookMargin)
{
   setValue(USEMRGN_KEY, ibUseBookMargin);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::GetMarginsDefined()
{
   return contains(MRGNTOP_KEY) &&
          contains(MRGNSIDE_KEY) &&
          contains(MRGNBOT_KEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::GetMargins(int & oTop, int & oSide, int & oBottom)
{
   oTop = GetIntValue(MRGNTOP_KEY);
   oSide = GetIntValue(MRGNSIDE_KEY);
   oBottom = GetIntValue(MRGNBOT_KEY);
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveMargins(int iTop, int iSide, int iBottom)
{
   setValue(MRGNTOP_KEY, iTop);
   setValue(MRGNSIDE_KEY, iSide);
   setValue(MRGNBOT_KEY, iBottom);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpubSettings::HyphenPatternLangDefined()
{
   return contains(HYPHEN_KEY);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpubSettings::GetHyphenPatternLang()
{
   return value(HYPHEN_KEY).toString();
}

// -----------------------------------------------------------------------------

void qPBReaderEpubSettings::SaveHyphenPatternLang(const QString & isLang)
{
   setValue(HYPHEN_KEY, isLang);
}

// -----------------------------------------------------------------------------
