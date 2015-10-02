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

#include "qPBReaderEpub.h"

// qt
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QUrl>
#include <QXmlQuery>

// project
#include "qPBReaderConfig.h"
#include "qPBReaderEpubOpf.h"
#include "qPBReaderEpubSettings.h"
#include "qPBReaderTracer.h"
#include "qPBReaderTOCGenerator.h"
#include "qPBReaderUnzip.h"
#include "qPBReaderUtils.h"

// -----------------------------------------------------------------------------

#define CONTAINER   "META-INF/container.xml"
#define CONTAINERNS "urn:oasis:names:tc:opendocument:xmlns:container"

#define OEBPS_TYPE  "application/oebps-package+xml"

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderEpub::qPBReaderEpub(const QString & isEpubFile) :
   _sFile(isEpubFile),
   _pSettings(0),
   _pCurrentOpf(new qPBReaderEpubOpf()),
   _nActiveOpf(-1)
{
   TRSCOPE(epub, "qPBReaderEpub::qPBReaderEpub");
}

// -----------------------------------------------------------------------------

qPBReaderEpub::~qPBReaderEpub()
{
   TRSCOPE(epub, "qPBReaderEpub::~qPBReaderEpub");

   Clean();

   if (_pCurrentOpf)
   {
      delete _pCurrentOpf;
      _pCurrentOpf = 0;
   }
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::Open()
{
   TRSCOPE(epub, "qPBReaderEpub::Open");

   Clean();

   // compute MD5 to get config file and temporary directory

   _sMD5 = qPBReaderUtils::MD5(_sFile);

   bool bOk = !_sMD5.isEmpty();

   if (bOk)
   {
      _sTempDir = qPBReaderConfig::GetTempDir() + "/" + _sMD5;
      TRACE << "temp dir " << ENC(_sTempDir) << endl;

      bOk = InitSettings(qPBReaderConfig::GetDataDir() + "/" + _sMD5 + ".cfg");
   }

   // extract epub file

   if (bOk && qgetenv("qPBReaderDisableEpubExtraction").isEmpty())
   {
      bOk = qPBReaderUnzip::ExtractAllTo(_sFile, _sTempDir);
   }

   // analyze epub. Won't be completely performed if settings are present.

   if (bOk)
   {
      bOk = AnalyzeDocStructure();
   }

   // keep the input file name in config, useful when data
   // will be checked for entries to removed
   // will avoid deleting file if epub has been moved and reopened
   // since first open

   if (bOk)
   {
      QFile f(_sFile);
      _pSettings->SaveEpubFileName(QFileInfo(f).absoluteFilePath());
   }

   // don't stay partially initialized in case of error

   if (!bOk)
   {
      Clean();
   }

   TRACE << TRANAME << " " << VAR(bOk) << endl;

   return bOk;
}

// -----------------------------------------------------------------------------

int qPBReaderEpub::GetNbOpf() const
{
   int nb = 0;

   if (_pSettings)
   {
      nb = _pSettings->GetNbOpf();
   }

   return nb;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::SetCurrentOpf(int nOpfNum)
{
   // if valid index

   bool b = nOpfNum >= 0 && nOpfNum < GetNbOpf();

   // switch opf

   if (b)
   {
      b = _pCurrentOpf->Set(
             _pSettings,
             nOpfNum,
             qPBReaderUtils::CleanPath(
                _sTempDir + "/" + _pSettings->GetOpfFile(nOpfNum)
             )
          );
   }

   // use index and persist it

   if (b)
   {
      _nActiveOpf = nOpfNum;
      _pSettings->SaveCurrentOpf(_nActiveOpf);
   }

   return b;
}

// -----------------------------------------------------------------------------

int qPBReaderEpub::GetCurrentOpf() const
{
   return _nActiveOpf;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::GetCurrentOpfFileName(QString & osFileName)
{
   osFileName.clear();

   bool b = true;

   osFileName = _pCurrentOpf->GetOpfFileName();

   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetCurrentOpfFileName()
{
   QString sFileName;
   GetCurrentOpfFileName(sFileName);
   return sFileName;
}

// -----------------------------------------------------------------------------

int qPBReaderEpub::GetNbSections() const
{
   int nb = 0;

   if (_pSettings)
   {
      nb = _pSettings->GetNbSectionsInOpf(_nActiveOpf);
   }

   return nb;
}

// -----------------------------------------------------------------------------

int qPBReaderEpub::GetCurrentSectionIndex()
{
   return _pCurrentOpf->GetCurrentSectionIndex();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::GetSectionFiles(QStringList & ols)
{
   ols.clear();

   int nb = GetNbSections();

   bool b = true;

   for (int idx = 0; b && idx < nb; idx++)
   {
      QString s;
      b = GetSectionFile(idx, s);
      ols.append(s);
   }

   if (!b)
   {
      ols.clear();
   }

   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetCurrentSectionFile()
{
   return qPBReaderUtils::CleanPath(
      _pCurrentOpf->GetOpfBaseDir() + "/" + _pCurrentOpf->GetCurrentSectionFile()
   );
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::GetSectionFile(int inSectionIndex, QString & osFile)
{
   bool b = _pCurrentOpf->GetSectionFileName(inSectionIndex, osFile);
   osFile = qPBReaderUtils::CleanPath(
      _pCurrentOpf->GetOpfBaseDir() + "/" + osFile
   );
   return b;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetSectionFile(int inSectionIndex)
{
   QString s;
   GetSectionFile(inSectionIndex, s);
   return s;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::PreviousSection()
{
   return _pCurrentOpf->PreviousSection();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::NextSection()
{
   return _pCurrentOpf->NextSection();
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetNCX()
{
   return qPBReaderUtils::CleanPath(
      _pCurrentOpf->GetOpfBaseDir() + "/" + _pCurrentOpf->GetNCX()
   );
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetTitle()
{
   return _pCurrentOpf->GetTitle();
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::GetHTMLToc(QString & osHTMLToc)
{
   TRSCOPE(epub, "qPBReaderEpub::GetHTMLToc");
   bool b = true;

   QString sNCX = GetNCX();

   // links inside ncx are relative to the ncx, so to make it easy, create
   // the html toc where the ncx is

   osHTMLToc = QFileInfo(sNCX).dir().path();

   b = !osHTMLToc.isEmpty();

   if (b)
   {
      osHTMLToc += "/TOC_OPF" + QString::number(GetCurrentOpf()) + ".xhtml";
   }

   // generate if not already done

   if (b && !QFile(osHTMLToc).exists())
   {
      b = qPBReaderTOCGenerator::GenerateHTMLTOC(sNCX, osHTMLToc);
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::SwitchToSectionByFileName(const QString & isFile)
{
   return _pCurrentOpf->SetCurrentSection(isFile);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SavePagesCount(const QList<int> & ilnPagesCount)
{
   _pCurrentOpf->SavePagesCount(ilnPagesCount);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::GetPagesCount(QList<int> & olnPagesCount)
{
   _pCurrentOpf->GetPagesCount(olnPagesCount);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SavePos(double val)
{
   _pSettings->SavePagePos(val);
}

// -----------------------------------------------------------------------------

double qPBReaderEpub::GetPos()
{
   double pos = _pSettings->GetPagePos();

   if (pos < 0.)
   {
      pos = 0.;
      SavePos(pos);
   }

   else if (pos > 1.0)
   {
      pos = 1.;
      SavePos(pos);
   }

   return pos;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveMagnification(double val)
{
   _pSettings->SaveMagnification(val);
}

// -----------------------------------------------------------------------------

double qPBReaderEpub::GetMagnification()
{
   double factor = _pSettings->GetMagnification();

   if (factor <= 0.)
   {
      factor = qPBReaderConfig::GetDefaultMagnificationFactor();
      SaveMagnification(factor);
   }

   return factor;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveStandardFont(int i)
{
   _pSettings->SaveStandardFont(i);
}

// -----------------------------------------------------------------------------

int qPBReaderEpub::GetStandardFont()
{
   int i = 0;

   if (!_pSettings->HasStandardFont())
   {
      i = qPBReaderConfig::GetDefaultStandardFont();
      SaveStandardFont(i);
   }

   else
   {
      i = _pSettings->GetStandardFont();
   }

   return i;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetSerifFamily()
{
   QString sFamily;

   if (!_pSettings->HasSerifFamily())
   {
      sFamily = qPBReaderConfig::GetDefaultSerifFont();
      SaveSerifFamily(sFamily);
   }
   else
   {
      sFamily = _pSettings->GetSerifFamily();
   }

   return sFamily;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetSansFamily()
{
   QString sFamily;

   if (!_pSettings->HasSansFamily())
   {
      sFamily = qPBReaderConfig::GetDefaultSansFont();
      SaveSansFamily(sFamily);
   }
   else
   {
      sFamily = _pSettings->GetSansFamily();
   }

   return sFamily;
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetMonoFamily()
{
   QString sFamily;

   if (!_pSettings->HasMonoFamily())
   {
      sFamily = qPBReaderConfig::GetDefaultMonoFont();
      SaveMonoFamily(sFamily);
   }
   else
   {
      sFamily = _pSettings->GetMonoFamily();
   }

   return sFamily;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveSerifFamily(const QString & isFamily)
{
   return _pSettings->SaveSerifFamily(isFamily);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveSansFamily(const QString & isFamily)
{
   return _pSettings->SaveSansFamily(isFamily);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveMonoFamily(const QString & isFamily)
{
   return _pSettings->SaveMonoFamily(isFamily);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::GetIgnoreFonts()
{
   bool bIgnoreFonts = false;

   if (!_pSettings->IgnoreFontsDefined())
   {
      bIgnoreFonts = qPBReaderConfig::GetDefaultIgnoreFonts();
      SaveIgnoreFonts(bIgnoreFonts);
   }
   else
   {
      bIgnoreFonts = _pSettings->GetIgnoreFonts();
   }

   return bIgnoreFonts;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveIgnoreFonts(bool ibIgnoreFonts)
{
   _pSettings->SaveIgnoreFonts(ibIgnoreFonts);
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::UseBookMargin()
{
   bool bUseBookMargin = true;

   if (!_pSettings->UseBookMarginDefined())
   {
      bUseBookMargin = qPBReaderConfig::GetDefaultUseBookMargin();
      _pSettings->SaveUseBookMargin(bUseBookMargin);
   }

   else
   {
      bUseBookMargin = _pSettings->UseBookMargin();
   }

   return bUseBookMargin;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveUseBookMargin(bool ibUseBookMargin)
{
   _pSettings->SaveUseBookMargin(ibUseBookMargin);
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::GetMargins(int & oTop, int & oSide, int & oBottom)
{
   if (!_pSettings->GetMarginsDefined())
   {
      qPBReaderConfig::GetDefaultMargins(oTop, oSide, oBottom);
      _pSettings->SaveMargins(oTop, oSide, oBottom);
   }

   else
   {
      _pSettings->GetMargins(oTop, oSide, oBottom);
   }
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveMargins(int iTop, int iSide, int iBottom)
{
   _pSettings->SaveMargins(iTop, iSide, iBottom);
}

// -----------------------------------------------------------------------------

QString qPBReaderEpub::GetHyphenPatternLang()
{
   QString sLang;
   if (!_pSettings->HyphenPatternLangDefined())
   {
      sLang = qPBReaderConfig::GetDefaultHyphenPatternLang();
      SaveHyphenPatternLang(sLang);
   }
   else
   {
      sLang = _pSettings->GetHyphenPatternLang();
   }
   return sLang;
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::SaveHyphenPatternLang(const QString & isLang)
{
   _pSettings->SaveHyphenPatternLang(isLang);
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

qPBReaderEpub::qPBReaderEpub() :
   _pSettings(0),
   _pCurrentOpf(0)
{
}

// -----------------------------------------------------------------------------

void qPBReaderEpub::Clean()
{
   // remove temporary files

   if (!_sTempDir.isEmpty() && qgetenv("qPBReaderLeaveTempEpub").isEmpty())
   {
      qPBReaderUtils::Rmdir(_sTempDir);
   }

   // forget

   _sMD5.clear();
   _sTempDir.clear();
   InitSettings();
   _pCurrentOpf->Clear();
   _nActiveOpf = -1;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::InitSettings(const QString & isFileName)
{
   TRSCOPE(epub, "qPBReaderEpub::InitSettings");
   if (_pSettings)
   {
      delete _pSettings;
      _pSettings = 0;
   }

   if (!isFileName.isEmpty())
   {
      TRACE << "book settings " << VAR(isFileName) << endl;
      _pSettings = new qPBReaderEpubSettings(isFileName);
   }

   return _pSettings != 0;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::AnalyzeDocStructure()
{
   TRSCOPE(epub, "qPBReaderEpub::AnalyzeDocStructure");

   // get opfs

   QStringList lsOpf;
   bool b = RetrieveOEB(lsOpf);

   for (int idx = 0; b && idx < lsOpf.size(); idx++)
   {
      // switch to opf. this will initialize stuff if not already done.

      b = _pCurrentOpf->Set(_pSettings,
                            idx,
                            qPBReaderUtils::CleanPath(_sTempDir + "/" + lsOpf[idx]));
   }

   _pCurrentOpf->Clear();

   // retrieve last used opf from settings, or use first one

   if (b)
   {
      int nb = _pSettings->GetCurrentOpf();

      if (nb < 0 || nb >= lsOpf.size())
      {
         nb = 0;
      }

      b = SetCurrentOpf(nb);
   }

   TRACE << b << endl;
   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::RetrieveOEB(QStringList & olsOpf)
{
   TRSCOPE(epub, "qPBReaderEpub::RetrieveOEB");

   // try settings

   bool b = _pSettings->GetOpf(olsOpf);

   // or analyze container

   if (b && !olsOpf.size())
   {
      TRACE << "Reading container" << endl;

      // create query from file

      QXmlQuery qry;
      b = qry.setFocus(QUrl(_sTempDir + "/" CONTAINER));
      TRACE << "qry.setFocus(...): " << b << endl;

      // prepare query
      // retrieve full-path from container/rootfiles/root elements where media-type=OEBPS_TYPE

      if (b)
      {
         qry.setQuery("declare default element namespace \"" CONTAINERNS "\";"
                      "/container/rootfiles/rootfile[lower-case(@media-type)='" OEBPS_TYPE
                      "']/@full-path/string()");
         b = qry.isValid();
         TRACE << "qry.isValid(): " << b << endl;
      }

      // execute it

      QStringList lsTmp;

      if (b)
      {
         b = qry.evaluateTo(&lsTmp) && lsTmp.size() > 0;
         TRACE << "evaluation " << b << " nbopf=" << lsTmp.size() << endl;
      }

      // get and make results persist

      if (b)
      {
         _pSettings->SaveOpf(lsTmp);
         olsOpf = lsTmp;
      }
   }

   if (!b)
   {
      olsOpf.clear();
   }

   else if (TRACEON)
   {
      for (int idx = 0; idx < olsOpf.size(); idx++)
      {
         TRACE << ENC(olsOpf[idx]) << endl;
      }
   }

   return b;
}

// -----------------------------------------------------------------------------

bool qPBReaderEpub::RetrieveCSS(QStringList & olsCssFiles)
{
   return _pCurrentOpf->GetCSSFiles(olsCssFiles);
}

// -----------------------------------------------------------------------------
