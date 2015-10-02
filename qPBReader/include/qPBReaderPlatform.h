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

#ifndef _qPBReaderPlatform_h__
#define _qPBReaderPlatform_h__

class QString;
class qPBReaderViewer;

typedef int (*IVEventHandlerFn)(int, int, int);

/**
 * Platform specific services.
 */
class qPBReaderPlatform
{
   public :

      /**
       * Installs ink view event handlers.
       */
      static void SubscribeToInkViewEvents(qPBReaderViewer * ipViewer);

      /**
       * Deactivates ink view event handlers.
       */
      static void UnsubscribeToInkViewEvents();

      /**
       * Forces display update.
       */
      static void UpdateScreenFull();

      /**
       * Displays hourglass icon.
       */
      static void ShowHourGlass();

      /**
       * Hides hourglass icon.
       */
      static void HideHourGlass();

      /**
       * Configures the panel in normal or reading mode.
       */
      static void SetReadingMode(bool ibReadingMode);

      /**
       * Notifies system reading activity has begun.
       * On pocketbook, registers the application for restart and probably
       * othe stuff.
       */
      static void ReaderApplicationStarted(const QString & isBookFile);

      /**
       * Registers page count into bookshelf application.
       * Bookshelf would display a read percentage, and a "finished" icon
       * if inCurrentPage is equal to inTotal.
       */
      static void SaveBookState(const QString & isBookFile,
                                int inCurrentPage,
                                int inTotal);

      /**
       * Refreshes bottom panel.
       */
      static void UpdatePanelDisplay();

      /**
       * Displays page count in bottom panel.
       */
      static void DisplayPageNumber(const QString & isTitle,
                                    int inCurrentPage,
                                    int inTotal);

      /**
       * Displays a message in the bottom panel.
       */
      static void DisplayPanelMessage(const QString & isMessage);

      /**
       * Erase all text in the bottom panel.
       */
      static void ErasePanelMessage();

      /**
       * Displays a self-closing message box, with no buttons.
       */
      static void MessageTimeOut(const QString & isTitle,
                                 const QString & isMessage,
                                 int inTimeoutMs);

      /**
       * Initialize environment. Called before Qt application object creation.
       */
      static void InitializeEnv();

      /**
       * Check for existing task. If exists, pop it to foreground.
       */
      static bool CheckForExisting(const QString & isEbook);

      /**
       * Returns current system language.
       */
      static QString LocaleName();

   private :

      static int IVProc(int type, int parm1, int parm2);
      static int IVMainProc(int type, int parm1, int parm2);

      static qPBReaderViewer * _pViewer;
      static IVEventHandlerFn _pProc;
      static IVEventHandlerFn _pMainProc;
};

#endif
