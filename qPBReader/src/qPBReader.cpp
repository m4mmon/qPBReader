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

#include "qPBReader.h"

// qt
#include <QtGui>

// project
#include "qPBReaderConfig.h"
#include "qPBReaderConfigDialog.h"
#include "qPBReaderPlatform.h"
#include "qPBReaderViewer.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

int main(int argc, char ** argv)
{
   TRSCOPE(app, "main");
   qPBReaderPlatform::InitializeEnv();

   qPBReader app(argc, argv);

   int rc = 0;
   if (app.InitialChecks(argc, argv))
   {
      rc = app.exec();
   }
   return rc;
}

// -----------------------------------------------------------------------------

qPBReader::qPBReader(int & argc, char ** argv) :
   QApplication(argc, argv),
   _bGlobalConfMode(false),
   _pViewer(0),
   _pConfigDialog(0)
{
   TRSCOPE(app, "qPBReader::qPBReader");

   if (TRACEON)
   {
      TRACE << "Current dir " << ENC(QDir::currentPath()) << endl;
      TRACE << "argc=" << argc << endl;

      for (int idx = 0; idx < argc; idx++)
      {
         TRACE << idx << ": " << ENC(argv[idx]) << endl;
      }
   }

   QString sLangFile = QCoreApplication::applicationFilePath() +
                       "_" +
                       qPBReaderConfig::GetApplicationLanguage();
   TRACE << VAR(sLangFile) << endl;
   _translator.load(sLangFile);
   installTranslator(&_translator);
}

// -----------------------------------------------------------------------------

bool qPBReader::InitialChecks(int argc, char ** argv)
{
   TRSCOPE(app, "qPBReader::InitialChecks");

   bool b = argc == 2;

   if (!b)
   {
      qPBReaderPlatform::MessageTimeOut(
         "qPBReader", tr("Bad program invokation"), 4000
      );
   }

   QString sArg;
   if (b)
   {
      sArg = QString::fromUtf8(argv[1]);
      _bGlobalConfMode = sArg == "-config";
   }

   if (b && !_bGlobalConfMode)
   {
      QFile f(sArg);
      b = f.exists();
      if (!b)
      {
         TRACE << "File does not exist" << endl;
         qPBReaderPlatform::MessageTimeOut(
            "qPBReader", sArg + tr(" not found"), 4000
         );
      }
      else
      {
         _sEbook = QFileInfo(f).absoluteFilePath();
         f.close();
         b = !qPBReaderPlatform::CheckForExisting(_sEbook);
         if (!b)
         {
            TRACE << "Process using this book already exist, exiting" << endl;
         }
      }
   }

   return b;
}

// -----------------------------------------------------------------------------

int qPBReader::exec()
{
   TRSCOPE(app, "qPBReader::exec");

   int rc = 0;

   connect(this, SIGNAL(aboutToQuit()),
           this, SLOT(End()));

   setOrganizationName("m4mmon");
   setApplicationName("qPBReader");
   setApplicationVersion("0.1");

   // application stylesheet

   {
      QFile f(":qPBReader.css");
      f.open(QFile::ReadOnly);
      QString css = QLatin1String(f.readAll());
      setStyleSheet(css);
   }

   // configuration dialog

   if (_bGlobalConfMode)
   {
      _pConfigDialog = new qPBReaderConfigDialog();
      _pConfigDialog->show();
      return QApplication::exec();
   }

   // or instantiate viewer

   else
   {
      _pViewer = new qPBReaderViewer(_sEbook);
      _pViewer->show();

      // and launch

      if (_pViewer->Start())
      {

         TRACE << "go!" << endl;
         return QApplication::exec();
      }
      else
      {
         qPBReaderPlatform::MessageTimeOut(
            "qPBReader", tr("Could not open ") + _sEbook, 4000
         );
         rc = -1;
      }
   }

   return rc;
}

// -----------------------------------------------------------------------------

void qPBReader::End()
{
   TRSCOPE(app, "qPBReader::End");

   if (_pViewer)
   {
      delete _pViewer;
      _pViewer = 0;
   }

   if (_pConfigDialog)
   {
      delete _pConfigDialog;
      _pConfigDialog = 0;
   }
}

// -----------------------------------------------------------------------------
