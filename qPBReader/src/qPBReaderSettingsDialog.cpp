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

#include "qPBReaderSettingsDialog.h"

// project
#include "qPBReaderTracer.h"
#include "qPBReaderVerticalWidget.h"
#include "qPBReaderViewer.h"

// -----------------------------------------------------------------------------

qPBReaderSettingsDialog::qPBReaderSettingsDialog(qPBReaderDocView * ipView,
      qPBReaderViewer * parent) :
   qPBReaderDialog(parent, tr("Settings"))
{
   TRSCOPE(viewer, "qPBReaderSettingsDialog::qPBReaderSettingsDialog");

   qPBReaderVerticalWidget * pVert = new qPBReaderVerticalWidget(this);

   pVert->AddButton(tr("Text layout"),
                    parent, SLOT(TextLayout()),
                    ":textsettings.bmp");
   pVert->AddButton(tr("Font options"),
                    parent, SLOT(FontOptions()),
                    ":fonts.bmp");
#ifdef Q_OS_POCKETBOOK
   pVert->AddButton(tr("Orientation"),
                    parent,
                    SLOT(Orientation()), ":rotate.bmp");
#endif
   pVert->AddButton(tr("Compute page count"),
                    parent, SLOT(ComputePagesCount()),
                    ":pages.bmp");
   pVert->AddButton(tr("Table of contents"),
                    parent, SLOT(TableOfContents()),
                    ":toc.bmp");
   if (!qgetenv("qPBReaderDev").isEmpty())
   {
      pVert->AddButton(tr("Test"),
                       parent, SLOT(Test()));
   }
   pVert->AddButton(tr("Exit"),
                    parent, SLOT(Exit()),
                    ":exit_small.bmp");

   addWidget(pVert);
}

// -----------------------------------------------------------------------------

qPBReaderSettingsDialog::~qPBReaderSettingsDialog()
{
   TRSCOPE(viewer, "qPBReaderSettingsDialog::~qPBReaderSettingsDialog");
}

// -----------------------------------------------------------------------------
