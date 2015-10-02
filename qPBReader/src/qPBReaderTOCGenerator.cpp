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

#include "qPBReaderTOCGenerator.h"

// libxml and libxslt
#include <libxml2/libxml/parser.h>
#include <libxslt/transform.h>

// qt
#include <QFile>

// project
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

// cleans a xmlDocPtr
#define FREEXML(xmldoc) \
if (xmldoc)             \
{                       \
   xmlFreeDoc(xmldoc);  \
   xmldoc = 0;          \
}

// cleans a xsltStylesheetPtr and its xmlDocPtr source
// the source is cleaned by xsltFreeStylesheet
// so do not forget to clean source if stylesheet was not build.
#define FREEXSLT(xslt, xmlsource) \
if (xslt)                         \
{                                 \
   xsltFreeStylesheet(xslt);      \
   xslt = 0;                      \
   xmlsource = 0;                 \
}                                 \
else FREEXML(xmlsource)

// cleans a xmlChar buffer
#define FREEXMLCHAR(xmlBuff) \
if (xmlBuff)                 \
{                            \
   xmlFree(xmlBuff);         \
   xmlBuff = 0;              \
}

// -----------------------------------------------------------------------------

bool qPBReaderTOCGenerator::GenerateHTMLTOC(const QString & isNCX,
      const QString & isHTMLToc)
{
   TRSCOPE(epub, "qPBReaderTOCGenerator::GenerateHTMLTOC");

   // here we directly use libxml and libxslt. After struggling with
   // QXmlQuery, I found this page:
   // http://doc.qt.io/qt-4.8/xmlprocessing.html#xslt-2-0
   // explaining xsl:output feature was not supported.

   bool b = true;

   // read stylesheet from resource

   xmlDocPtr xmlXsltDoc = 0;

   {
      // open resource

      QFile fxsl(":ncx2html.xsl");
      b = fxsl.open(QFile::ReadOnly);

      // read it and build xslt xml doc

      if (b)
      {
         QByteArray ba;
         ba = fxsl.readAll();
         fxsl.close();

         xmlXsltDoc = xmlParseMemory(ba.data(), ba.size());
         b = xmlXsltDoc != 0;
      }

      TRACE << "xmlXsltDoc build = " << b << endl;
   }

   // build xslt object from stylesheet

   xsltStylesheetPtr xsltDoc = 0;

   if (b)
   {
      xsltDoc = xsltParseStylesheetDoc(xmlXsltDoc);
      b = xsltDoc != 0;
   }

   // read xml doc to transform

   xmlDocPtr xmlDoc = 0;

   if (b)
   {
      xmlDoc = xmlParseFile(isNCX.toUtf8().data());
      b = xmlDoc != 0;
      TRACE << "Parse ncx = " << b << endl;
   }

   // transform

   xmlDocPtr resDoc = 0;

   if (b)
   {
      const char * parms[1];
      parms[0] = 0;
      resDoc = xsltApplyStylesheet(xsltDoc, xmlDoc, parms);
      b = resDoc != 0;
      TRACE << "Apply xslt = " << b << endl;
   }

   // we do not directly write result to disk because
   // we want to apply a "translate-able" string inside result

   QString res;

   if (b)
   {
      xmlChar * pDump = 0;
      int nLen = 0;
      xmlDocDumpFormatMemory(resDoc, &pDump, &nLen, 1);
      b = nLen > 0 && pDump != 0;

      if (b)
      {
         res = QString::fromUtf8((char *)pDump, nLen);
      }

      FREEXMLCHAR(pDump);
   }

   // write result to disk

   if (b)
   {
      QFile f(isHTMLToc);
      b = f.open(QFile::WriteOnly);

      if (b)
      {
         b = f.write(res.toUtf8());
         f.close();
      }
   }

   // cleanup

   FREEXSLT(xsltDoc, xmlXsltDoc);
   FREEXML(xmlDoc);
   FREEXML(resDoc);

   return b;
}

// -----------------------------------------------------------------------------
