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

#include "qPBReaderJSLoader.h"

// qt
#include <QFile>

// project
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderJSLoader::qPBReaderJSLoader(QWebPage * p) :
   _p(p)
{
   TRSCOPE(js, "qPBReaderJSLoader::qPBReaderJSLoader");

   // hard-coded module list, keep in synch with aliases in qrc file

   _lNames.append("jquery");
   _lNames.append("jquery_scrollTo");
   _lNames.append("bookmarks");
   _lNames.append("referencing");
   _lNames.append("images");
   _lNames.append("hyphenation");
   _lNames.append("Hyphenator");
   _lNames.append("utils");
   _lNames.append("cfi");
   _lNames.append("indexing");
   _lNames.append("paged");
   _lNames.append("full_screen");
   _lNames.append("mathjax");
   _lNames.append("extract");
   _lNames.append("qPBReader");

   // load resource

   for (int idx = 0; idx < _lNames.size(); idx++)
   {
      QByteArray ba;
      LoadResource(_lNames[idx], ba);
      _lSrc.append(QString::fromUtf8(ba.data(), ba.size()));
      TRACE << "Loaded " << ENC(_lNames[idx]) << ", length=" << _lSrc[idx].size() <<
            endl;
   }

}

// -----------------------------------------------------------------------------

qPBReaderJSLoader::~qPBReaderJSLoader()
{
   TRSCOPE(js, "qPBReaderJSLoader::~qPBReaderJSLoader");
}

// -----------------------------------------------------------------------------

void qPBReaderJSLoader::Load(const QString & isHyphenLang)
{
   TRSCOPE(js, "qPBReaderJSLoader::Load");

   if (_p)
   {
      for (int idx = 0; idx < _lNames.size(); idx++)
      {
         TRACE << "injecting " << ENC(_lNames[idx]) <<  " (" << _lSrc[idx].length() <<
               ")" << endl;
         _p->mainFrame()->evaluateJavaScript(_lSrc[idx]);
      }

      // if hyphenation is required

      if (!isHyphenLang.isEmpty())
      {
         // if pattern lang has changed

         if (isHyphenLang != _sLang)
         {
            // load it from resources

            TRACE << "Loading pattern script" << ENC(isHyphenLang) << endl;
            QByteArray ba;
            LoadResource("patterns/" + isHyphenLang, ba);

            // store lang and corresponding script

            _sLang = isHyphenLang;
            _sPattern = QString::fromUtf8(ba.data(), ba.size());

            TRACE << "Loaded " << ENC(isHyphenLang) << ", length=" << _sLang.size() << endl;
         }
         else
         {
            TRACE << "Pattern script" << ENC(_sLang) << "already loaded" << endl;
         }

         // and load it into page

         TRACE << "injecting hyphens pattern script" << endl;
         _p->mainFrame()->evaluateJavaScript(_sPattern);
      }
   }
}

// -----------------------------------------------------------------------------

void qPBReaderJSLoader::LoadResource(const QString & isResource, QByteArray & oData)
{
   QFile file;
   file.setFileName(":/" + isResource);
   file.open(QIODevice::ReadOnly);
   oData = file.readAll();
   file.close();
}

// -----------------------------------------------------------------------------
