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

#include "qPBReaderDoc.h"

// std
#include <assert.h>

// qt
#include <QFontDatabase>

// project
#include "qPBReaderConfig.h"
#include "qPBReaderJSLoader.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

bool convert(const QString & is, QList<int> & oln, const QString & iSep = ",");
bool convert(const QList<int> & iln, QString & os, const QString & iSep,
             const QString & iBeg,
             const QString & iEnd);

QStringList qPBReaderDoc::_lsAddedFonts;
QList<int> qPBReaderDoc::_lnAddedFontsId;

// -----------------------------------------------------------------------------
//  public
// -----------------------------------------------------------------------------

qPBReaderDoc::qPBReaderDoc(QObject * ipParent) :
   QWebPage(ipParent),
   _pLoader(0),
   _bJSLoaded(false),
   _pp(this),
   _bUseBookMargin(true),
   _marginTop(20),
   _marginSide(40),
   _marginBottom(20),
   _bIgnoreFonts(false)
{
   TRSCOPE(doc, "qPBReaderDoc::qPBReaderDoc");

   //setNetworkAccessManager(0);
   networkAccessManager()->setNetworkAccessible(
      QNetworkAccessManager::NotAccessible);

   // javascript loader

   _pLoader = new qPBReaderJSLoader(this);


   // to get LinkClicked notifications

   setLinkDelegationPolicy(QWebPage::DelegateAllLinks);


   // web page settings

   QWebSettings::setObjectCacheCapacities(0, 0, 0);

   QWebSettings * pSettings = settings();

   if (pSettings)
   {
#ifndef Q_OS_POCKETBOOK
      // for javascript debugging, get the inspector on right click on PC.

      pSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

#endif
      pSettings->setAttribute(QWebSettings::JavaEnabled, false);
      pSettings->setAttribute(QWebSettings::PluginsEnabled, false);
      pSettings->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
      pSettings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false);
      pSettings->setAttribute(QWebSettings::LinksIncludedInFocusChain, true);

      // this one is important, if not set, paged display javascript
      // will completely fuck up if zoom factor is not 1.

      pSettings->setAttribute(QWebSettings::ZoomTextOnly, true);


      // user stylesheet must be provided as an url
      // so copy resource into temporary file
      // the temporary file will be removed at destruction

      QFile fres(":qPBReaderUserStyleSheet.css");
      fres.open(QFile::ReadOnly);
      _fUserStyleSheet.open();
      _fUserStyleSheet.write(fres.readAll());
      _fUserStyleSheet.close();
      TRACE << "user stylesheet " << ENC(_fUserStyleSheet.fileName()) << endl;
      pSettings->setUserStyleSheetUrl(QUrl::fromLocalFile(
                                         _fUserStyleSheet.fileName()));
   }

   // trigger javascript injection

   connect(
      mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
      this,        SLOT(AddWindowObjects())
   );

   // hide scroll bars

   mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
   mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}

// -----------------------------------------------------------------------------

qPBReaderDoc::~qPBReaderDoc()
{
   TRSCOPE(doc, "qPBReaderDoc::~qPBReaderDoc");

   if (_pLoader)
   {
      delete _pLoader;
      _pLoader = 0;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::LoadJSLibraries()
{
   TRSCOPE(doc, "qPBReaderDoc::LoadJSLibraries");

   if (!_bJSLoaded)
   {
      // load scripts

      _pLoader->Load(_sHyphenLang);

      _bJSLoaded = true;

      EvalJS("window.calibre_utils.setup_epub_reading_system("
             "   \"qPBReader\", \"0.1\", \"paginated\", [\"dom-manipulation\", \"layout-changes\", \"mouse-events\", \"keyboard-events\"]"
             ")"
      );

      // tell mathjax loader where to find mathjax

      EvalJS("window.mathjax.base = '" + qPBReaderConfig::GetMathjaxDir() + "'");

      EvalJS("py_bridge.mark_element.connect(window.calibre_extract.mark)");
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::AfterLoad()
{
   TRSCOPE(doc, "qPBReaderDoc::AfterLoad");

   EvalJS("window.paged_display.read_document_margins()");
   SetBottomPadding(0);
   EvalJS("setup_image_scaling_handlers()");
   Hyphenate();
   EvalJS("full_screen.save_margins()");
   SwitchToPagedMode();
   EvalJS("window.mathjax.check_for_math()");
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::ProcessFonts()
{
   TRSCOPE(css, "qPBReaderDoc::ProcessFonts");

   if (_bIgnoreFonts)
   {
      int nbCss = GetNumberOfStyleSheets();
      TRACE << "Got " << nbCss << " stylesheets" << endl;

      for (int idx = 0; idx < nbCss; idx++)
      {
         // for each rule

         int nbRules = GetNumberOfStyleSheetRules(idx);
         TRACE << "Got " << nbRules << " rules for stylesheet #" << idx << endl;

         for (int jdx = 0; jdx < nbRules; jdx++)
         {
            // if font-face rule

            if (IsFontFaceRule(idx, jdx))
            {
               RemoveCssRule(idx, jdx);
               nbRules--;
               jdx--;
            }
            else if (IsStyleRule(idx, jdx))
            {
               RemoveCssStyleProperty(idx, jdx, "font-family");
            }
         }
      }
   }
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
   else
   {
      BigUglyCSSFontFaceHack();
   }
#endif
}

// -----------------------------------------------------------------------------

QList<int> qPBReaderDoc::ColumnBoundaries()
{
   TRSCOPE(doc, "qPBReaderDoc::ColumnBoundaries");

   int inColumnBoundaries = 1;
   assert(inColumnBoundaries == 0);

   EvalJS("py_bridge.value = paged_display.column_boundaries()");

   QList<int> ln;

   if (_bJSLoaded)
   {
      convert(bridge_value.toString(), ln);

      if (ln.size() != 2)
      {
         TRACE << "Got a list with " << ln.size() << " elements" << endl;
         ln.clear();
         ln.append(0);
         ln.append(1);
      }
   }

   else
   {
      ln.append(0);
      ln.append(1);
   }

   return ln;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::ScrollBy(int dx, int dy)
{
   int inScrollBy = 1;
   assert(inScrollBy == 0);

   mainFrame()->scroll(dx, dy);
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::ScrollTo(int x, int y)
{
   TRSCOPE(doc, "qPBReaderDoc::ScrollTo");

   TRACE << VAR(x) << VAR(y) << endl;

   mainFrame()->setScrollPosition(QPoint(x, y));
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::JumpToAnchor(const QString & is)
{
   if (_bJSLoaded)
   {
      EvalJS("window.paged_display.jump_to_anchor(\"" + is + "\")");
   }
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::AtBottom()
{
   int inAtBottom = 0;
   assert(inAtBottom == 1);

   return (height() - ypos()) <= window_height();
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::AtTop()
{
   int inAtTop = 0;
   assert(inAtTop == 1);

   return ypos() <= 0;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::ypos()
{
   return mainFrame()->scrollPosition().y();
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::window_height()
{
   TRSCOPE(doc, "qPBReaderDoc::window_height");

   int val = EvalJSInt("window.innerHeight");

   TRACE << "window.innerHeight=" << val << endl;

   return val;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::window_width()
{
   TRSCOPE(doc, "qPBReaderDoc::window_width");

   int val = EvalJSInt("window.innerWidth");

   TRACE << "window.innerWidth=" << val << endl;

   return val;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::xpos()
{
   return mainFrame()->scrollPosition().x();
}

// -----------------------------------------------------------------------------

double qPBReaderDoc::GetScrollFraction()
{
   double val = 0.;

   if (_bJSLoaded)
   {
      val = EvalJSDouble("window.paged_display.current_pos()");
   }

   else
   {
      int whyamihere = 0;
      assert(whyamihere == 1);
      int div = height() - window_height();

      if (div != 0)
      {
         val = fabsf((double)ypos() / (double)div);
      }
   }

   return val;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetScrollFraction(double val)
{
   TRSCOPE(doc, "qPBReaderDoc::SetScrollFraction");

   if (_bJSLoaded)
   {
      TRACE << "Scrolling to " << val << endl;
      EvalJS("paged_display.scroll_to_pos(" + QString::number(val) + ")");
   }
}

// -----------------------------------------------------------------------------

double qPBReaderDoc::hscrollFraction()
{
   double val = 0.;
   int w = width();

   if (w != 0)
   {
      val = (double)xpos() / (double)w;
   }

   return val;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::height()
{
   int height = mainFrame()->contentsSize().height();

   if (height < 0)
   {
      TRSCOPE(doc, "qPBReaderDoc::height hack");
      int jheight = EvalJSInt("document.body.offsetHeight");
      TRACE << VAR(jheight) << endl;

      if (jheight > 0)
      {
         height = jheight;
      }
   }

   return height;
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::width()
{
   return mainFrame()->contentsSize().width();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetBottomPadding(int val)
{
   TRSCOPE(doc, "qPBReaderDoc::SetBottomPadding");

   TRACE << VAR(val) << endl;

   QSize s;

   if (val == 0)
   {
      s = QSize(-1, -1);
   }

   else
   {
      s = QSize(viewportSize().width(), height() + val);
   }

   setPreferredContentsSize(s);
}

// -----------------------------------------------------------------------------

QString qPBReaderDoc::ExtractNode()
{
   return EvalJSString("window.calibre_extract.extract()");
}

// -----------------------------------------------------------------------------

QVariant qPBReaderDoc::EvalJS(const QString & iSrc)
{
   TRSCOPE(doc, "qPBReaderDoc::EvalJS");

   TRACE << ENC(iSrc) << endl;

   return mainFrame()->evaluateJavaScript(iSrc);
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::EvalJSInt(const QString & iSrc)
{
   return EvalJS(iSrc).toInt();
}

// -----------------------------------------------------------------------------

double qPBReaderDoc::EvalJSDouble(const QString & iSrc)
{
   return EvalJS(iSrc).toDouble();
}

// -----------------------------------------------------------------------------

float qPBReaderDoc::EvalJSFloat(const QString & iSrc)
{
   return EvalJS(iSrc).toFloat();
}

// -----------------------------------------------------------------------------

qreal qPBReaderDoc::EvalJSReal(const QString & iSrc)
{
   return EvalJS(iSrc).toReal();
}

// -----------------------------------------------------------------------------

QString qPBReaderDoc::EvalJSString(const QString & iSrc)
{
   return EvalJS(iSrc).toString();
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::EvalJSBool(const QString & iSrc)
{
   return EvalJS(iSrc).toBool();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SavePagePos()
{
   _pp.Save();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::RestorePagePos()
{
   _pp.Restore();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::UpdateContentsSizeForPagedMode()
{
   TRSCOPE(doc, "qPBReaderDoc::UpdateContentsSizeForPagedMode");

   if (_bJSLoaded)
   {
      int side_margin = EvalJSInt("window.paged_display.layout(false)");
      QSize sz = mainFrame()->contentsSize();
      int scroll_width = EvalJSInt("document.body.scrollWidth");

      if (scroll_width > window_width())
      {
         sz.setWidth(scroll_width + side_margin);
         setPreferredContentsSize(sz);
      }

      EvalJS("window.paged_display.fit_images()");
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetFonts(int i,
                            const QString & isSerifFamily,
                            const QString & isSansFamily,
                            const QString & isMonoFamily)
{
   TRSCOPE(doc, "qPBReaderDoc::SetFonts");

   QWebSettings * pSettings = settings();

   if (!isSerifFamily.isEmpty())
   {
      pSettings->setFontFamily(QWebSettings::SerifFont, isSerifFamily);
   }

   if (!isSansFamily.isEmpty())
   {
      pSettings->setFontFamily(QWebSettings::SansSerifFont, isSansFamily);
   }

   if (!isMonoFamily.isEmpty())
   {
      pSettings->setFontFamily(QWebSettings::FixedFont, isMonoFamily);
   }

   QWebSettings::FontFamily ff = QWebSettings::SerifFont;

   if (i == 1)
   {
      ff = QWebSettings::SansSerifFont;
   }

   else if (i == 2)
   {
      ff = QWebSettings::FixedFont;
   }

   pSettings->setFontFamily(
      QWebSettings::StandardFont,
      pSettings->fontFamily(ff)
   );
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::GetFonts(QString & osStandardFamily,
                            QString & osSerifFamily,
                            QString & osSansFamily,
                            QString & osMonoFamily)
{
   QWebSettings * pSettings = settings();
   osStandardFamily = pSettings->fontFamily(QWebSettings::StandardFont);
   osSerifFamily = pSettings->fontFamily(QWebSettings::SerifFont);
   osSansFamily = pSettings->fontFamily(QWebSettings::SansSerifFont);
   osMonoFamily = pSettings->fontFamily(QWebSettings::FixedFont);
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetCurrentPage()
{
   int pos = 0;
   if (_bJSLoaded)
   {
      pos = EvalJSInt(
         "window.paged_display.column_boundaries()[0] + 1"
      );
   }
   return pos;
}

// -----------------------------------------------------------------------------
//  public slots
// -----------------------------------------------------------------------------

void qPBReaderDoc::AddWindowObjects()
{
   TRSCOPE(doc, "qPBReaderDoc::AddWindowObjects");

   // prepare page for js interop

   if (_pLoader)
   {
      // expose this object as "py_bridge" for calibre's javascript scripts

      mainFrame()->addToJavaScriptWindowObject("py_bridge", this);

      // plug that to our _pass_json_value property

      EvalJS(
         "Object.defineProperty(py_bridge, 'value', {\n"
         "   get : function() { return JSON.parse(this._pass_json_value); },\n"
         "   set : function(val) { this._pass_json_value = JSON.stringify(val); }\n"
         "});"
      );

      // and reset loaded flag

      _bJSLoaded = false;
   }
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::debug(const QString & iMsg)
{
   TRSCOPE(js, "-------------------------------------qPBReaderDoc::debug-------------------------------------");

   TRACE << iMsg << endl;
}

// -----------------------------------------------------------------------------
//  protected
// -----------------------------------------------------------------------------

void qPBReaderDoc::javaScriptConsoleMessage(const QString & message,
      int lineNumber, const QString & sourceID)
{
   TRSCOPE(js, "-------------------------------------qPBReaderDoc::javaScriptConsoleMessage-------------------------------------");

   TRACE << lineNumber << " " << sourceID << endl
         << message << endl;
}

// -----------------------------------------------------------------------------
//  private
// -----------------------------------------------------------------------------

QVariant qPBReaderDoc::_pass_json_value_getter()
{
   TRSCOPE(doc, "qPBReaderDoc::_pass_json_value_getter");

   TRACE << "Sending " << ENC(bridge_value.toString()) << endl;

   return bridge_value;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::_pass_json_value_setter(const QVariant & is)
{
   TRSCOPE(doc, "qPBReaderDoc::_pass_json_value_setter");

   TRACE << "Received " << ENC(is.toString()) << endl;

   bridge_value = is;
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SwitchToPagedMode()
{
   TRSCOPE(doc, "qPBReaderDoc::SwitchToPagedMode");

   QString sMargin = _bUseBookMargin ? "true" : "false";

   EvalJS(
      "window.paged_display.use_document_margins = " + sMargin + ";\n"
      "window.paged_display.set_geometry(1, " +
      QString::number(_marginTop) + ", " +
      QString::number(_marginSide) + ", " +
      QString::number(_marginBottom) + ");\n"
   );

   UpdateContentsSizeForPagedMode();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::Hyphenate()
{
   TRSCOPE(doc, "qPBReaderDoc::Hyphenate");

   if (!_sHyphenLang.isEmpty())
   {
      EvalJS("do_hyphenation(\"" + _sHyphenLang + "\")");
   }
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetNumberOfStyleSheets()
{
   return mainFrame()->evaluateJavaScript("document.styleSheets.length").toInt();
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetNumberOfStyleSheetRules(int inStyleSheetNum)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);

   return mainFrame()->evaluateJavaScript(
             "document.styleSheets[" + sStyleSheetNum + "].rules.length"
          ).toInt();
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::IsA(int inStyleSheetNum, int inRuleNum,
                       const QString & isType)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   return mainFrame()->evaluateJavaScript(
             "document.styleSheets[" + sStyleSheetNum + "]"
             ".rules[" + sRuleNum + "] instanceof " + isType
          ).toBool();
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::IsFontFaceRule(int inStyleSheetNum, int inRuleNum)
{
   return IsA(inStyleSheetNum, inRuleNum, "CSSFontFaceRule");
}

// -----------------------------------------------------------------------------

bool qPBReaderDoc::IsStyleRule(int inStyleSheetNum, int inRuleNum)
{
   return IsA(inStyleSheetNum, inRuleNum, "CSSStyleRule");
}

// -----------------------------------------------------------------------------

QString qPBReaderDoc::GetRuleStylePropertyValue(int inStyleSheetNum,
      int inRuleNum,
      const QString & isProp)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   return mainFrame()->evaluateJavaScript(
             "val = document.styleSheets[" + sStyleSheetNum + "]"
             ".rules[" + sRuleNum + "]"
             ".style.getPropertyValue(\"" + isProp + "\")"
          ).toString();
}

// -----------------------------------------------------------------------------

int qPBReaderDoc::GetRuleStylePropertySize(int inStyleSheetNum,
      int inRuleNum,
      const QString & isProp)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   return mainFrame()->evaluateJavaScript(
             "val = document.styleSheets[" + sStyleSheetNum + "]"
             ".rules[" + sRuleNum + "]"
             ".style.getPropertyCSSValue(\"" + isProp + "\");\n"
             "if (val) val.length; else 0;"
          ).toInt();
}

// -----------------------------------------------------------------------------

QString qPBReaderDoc::GetRuleStylePropertyValue(int inStyleSheetNum,
      int inRuleNum,
      int inValueNum,
      const QString & isProp)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);
   QString sValueNum = QString::number(inValueNum);

   return mainFrame()->evaluateJavaScript(
             "document.styleSheets[" + sStyleSheetNum + "]"
             ".rules[" + sRuleNum + "]"
             ".style.getPropertyCSSValue(\"" + isProp + "\")[" + sValueNum + "]"
             ".cssText"
          ).toString();
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::SetCssStyleProperty(int inStyleSheetNum,
                                       int inRuleNum,
                                       const QString & isProp,
                                       const QString & isVal)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   mainFrame()->evaluateJavaScript(
      "document.styleSheets[" + sStyleSheetNum + "]"
      ".rules[" + sRuleNum + "]"
      ".style.setProperty(\"" + isProp + "\", \"" + isVal + "\")"
   );
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::RemoveCssStyleProperty(int inStyleSheetNum,
                                          int inRuleNum,
                                          const QString & isProp)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   mainFrame()->evaluateJavaScript(
      "document.styleSheets[" + sStyleSheetNum + "]"
      ".rules[" + sRuleNum + "]"
      ".style.removeProperty(\"" + isProp + "\")"
   );
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::RemoveCssRule(int inStyleSheetNum, int inRuleNum)
{
   QString sStyleSheetNum = QString::number(inStyleSheetNum);
   QString sRuleNum = QString::number(inRuleNum);

   mainFrame()->evaluateJavaScript(
      "document.styleSheets[" + sStyleSheetNum + "]"
      ".deleteRule(" + sRuleNum + ")"
   );
}

// -----------------------------------------------------------------------------

void qPBReaderDoc::BigUglyCSSFontFaceHack()
{
   TRSCOPE(css, "qPBReaderDoc::BigUglyCSSFontFaceHack");

   // clean up everything before starting
   // first remove what we added from Qt font database

   {
      TRACE << "****** CLEANUP ******" << endl;
      bool b = QFontDatabase::removeAllApplicationFonts();
      TRACE << "QFontDatabase::removeAllApplicationFonts()=" << b << endl;
      _lsAddedFonts.clear();
      _lnAddedFontsId.clear();
      TRACE << "****** DONE ******" << endl;
   }



   // holders for font-families names to replace if css-defined value
   // differs from font internal value

   QStringList lsCssFontFamily, lsRealFontFamily;


   // check style sheet for @font-face rules
   // extract uri and font-family

   // for each loaded stylesheet

   int nbCss = GetNumberOfStyleSheets();
   TRACE << "Got " << nbCss << " stylesheets" << endl;

   for (int idx = 0; idx < nbCss; idx++)
   {
      // for each rule

      int nbRules = GetNumberOfStyleSheetRules(idx);
      TRACE << "Got " << nbRules << " rules for stylesheet #" << idx << endl;

      for (int jdx = 0; jdx < nbRules; jdx++)
      {
         // if font-face rule

         bool bIsFontFaceRule = IsFontFaceRule(idx, jdx);

         TRACE << jdx << " " << VAR(bIsFontFaceRule) << endl;

         if (bIsFontFaceRule)
         {
            // get font-family

            QString sFamily = GetRuleStylePropertyValue(idx, jdx, "font-family");
            QString sFamilyUnquoted = sFamily;

            if (sFamilyUnquoted.length() > 1 && sFamilyUnquoted.startsWith("'") &&
                  sFamilyUnquoted.endsWith("'"))
            {
               sFamilyUnquoted = sFamilyUnquoted.mid(1, sFamilyUnquoted.length() - 2);
            }

            // get font uri

            QString sUri = GetRuleStylePropertyValue(idx, jdx, "src");

            if (sUri.length() > 4 && sUri.startsWith("url(") && sUri.endsWith(")"))
            {
               sUri = sUri.mid(4, sUri.length() - 5);
            }

            TRACE << "   " << ENC(sFamily) << " " << ENC(sUri) << endl;

            // load font into Qt font database and/or remove rule

            if (!sFamily.isEmpty() && !sUri.isEmpty() && QUrl(sUri).isLocalFile())
            {
               bool bRemoveRule = false;

               // font not already added

               int nPos = _lsAddedFonts.indexOf(sUri);

               if (nPos == -1)
               {
                  int x = QFontDatabase::addApplicationFont(QUrl(sUri).path());
                  TRACE << "Got " << x << endl;

                  // font is added, store path and font id, compute pos it is used later
                  if (x >= 0)
                  {
                     bRemoveRule = true;
                     _lsAddedFonts.append(sUri);
                     _lnAddedFontsId.append(x);
                     nPos = _lsAddedFonts.size() - 1;
                  }

                  else
                  {
                     TRACE << "A problem occured while loading font into runtime database" << endl;
                  }
               }

               // font already in database

               else
               {
                  TRACE << "Font file already added to database" << endl;
                  bRemoveRule = true;
               }

               // remove css rule

               if (bRemoveRule)
               {
                  TRACE << "Removing rule #" << jdx << " of stylesheet #" << idx << endl;
                  RemoveCssRule(idx, jdx);
                  nbRules--;
                  jdx--;

                  // now check if css rules need to be updated with real font-family

                  QStringList lsRealFamilies = QFontDatabase::applicationFontFamilies(
                                                  _lnAddedFontsId[nPos]);
                  int nFamilyPos = lsRealFamilies.indexOf(sFamilyUnquoted);
                  TRACE << ENC(sFamilyUnquoted) << " inside real font-defined font-families is #"
                        << nFamilyPos << endl;

                  // font-face font-family does not match real font-family,
                  // and we have not yet planned to replace that family

                  if (lsRealFamilies.size() && nFamilyPos == -1 &&
                        lsCssFontFamily.indexOf(sFamily) == -1)
                  {
                     // stack values for replacement

                     lsCssFontFamily.append(sFamily);
                     lsRealFontFamily.append(lsRealFamilies[0]);
                  }
               }
            }
         }
      }
   }


   // if required, reparse the stylesheet for patching rules where
   // font-family matches removed @font-face rules and where actual font-family
   // differs from the one defined in css...

   if (lsCssFontFamily.size())
   {
      TRACE << "There might be font-family values to fix" << endl;

      // so there is something to replace...

      // for each stylesheet

      int nbCss = GetNumberOfStyleSheets();

      for (int idx = 0; idx < nbCss; idx++)
      {
         // and for each rule

         int nbRules = GetNumberOfStyleSheetRules(idx);

         for (int jdx = 0; jdx < nbRules; jdx++)
         {
            // if style rule

            if (IsStyleRule(idx, jdx))
            {
               // get number of font-face values from style

               int nbFam = GetRuleStylePropertySize(idx, jdx, "font-family");
               TRACE << "Got " << nbFam << " families for stylesheet #" << idx << " rule #" <<
                     jdx << endl;

               // read values one by one, and replace on the fly

               QStringList lsNewFontFamilyValues;
               bool bHaveToReplace = false;

               // for each font-face value defined in rule style

               for (int k = 0; k < nbFam; k++)
               {
                  // get value

                  QString val = GetRuleStylePropertyValue(idx, jdx, k, "font-family");
                  TRACE << "   " << k << " " << ENC(val) << endl;

                  // check if present in list of values to replace

                  int nPos = lsCssFontFamily.indexOf(val);

                  // nope, keep

                  if (nPos == -1)
                  {
                     lsNewFontFamilyValues.append(val);
                  }

                  // use real font-defined value

                  else
                  {
                     lsNewFontFamilyValues.append("'" + lsRealFontFamily[nPos] + "'");
                     bHaveToReplace = true;
                  }
               }

               // have to replace

               if (bHaveToReplace)
               {
                  // build new value

                  QString sNewVal;

                  for (int k = 0; k < lsNewFontFamilyValues.size(); k++)
                  {
                     if (k > 0)
                     {
                        sNewVal += ", ";
                     }

                     sNewVal += lsNewFontFamilyValues[k];
                  }

                  TRACE << "   Replacement value " << ENC(sNewVal) << endl;

                  // and replace ("ouf !")

                  SetCssStyleProperty(idx, jdx, "font-family", sNewVal);
               }
            }
         }
      }
   }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool convert(const QString & is, QList<int> & oln, const QString & iSep)
{
   int i_am_in_convert_tolist = 0;
   assert(i_am_in_convert_tolist == 1);

   oln.clear();
   QStringList tmp = is.split(iSep);

   for (int idx = 0; idx < tmp.size(); idx++)
   {
      oln.append(tmp[idx].toInt());
   }

   return true;
}

// -----------------------------------------------------------------------------

bool convert(const QList<int> & iln, QString & os, const QString & iSep,
             const QString & iBeg,
             const QString & iEnd)
{
   int i_am_in_convert_fromlist = 0;
   assert(i_am_in_convert_fromlist == 1);

   os = iBeg;
   int nb = iln.size();

   for (int idx = 0; idx < nb; idx++)
   {
      os += QString::number(iln[idx]);

      if (idx + 1 < nb)
      {
         os += iSep;
      }
   }

   os += iEnd;
   return true;
}

// -----------------------------------------------------------------------------
