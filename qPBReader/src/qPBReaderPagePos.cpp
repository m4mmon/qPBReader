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

#include "qPBReaderPagePos.h"

// project
#include "qPBReaderDoc.h"
#include "qPBReaderTracer.h"

// -----------------------------------------------------------------------------

qPBReaderPagePos::qPBReaderPagePos(qPBReaderDoc * ipDoc) :
   _pDoc(ipDoc),
   _dFallbackPos(-1.)
{
}

// -----------------------------------------------------------------------------

qPBReaderPagePos::~qPBReaderPagePos()
{
}

// -----------------------------------------------------------------------------

void qPBReaderPagePos::Save(bool bOverwrite)
{
   TRSCOPE(pages, "qPBReaderPagePos::Save");

   if (!bOverwrite && (!_savedPos.isEmpty() || _dFallbackPos <= 0.))
   {
      TRACE << "overwrite of a stored position denied" << endl;
   }

   else if (_pDoc)
   {
      _savedPos = _pDoc->EvalJSString("window.paged_display.current_cfi()");
      TRACE << "pos_cfi " << ENC(_savedPos) << endl;

      if (_savedPos.isEmpty())
      {
         _dFallbackPos = _pDoc->GetScrollFraction();
         TRACE << "fallback pos=" << _dFallbackPos << endl;
      }
   }
}

// -----------------------------------------------------------------------------

void qPBReaderPagePos::Restore()
{
   TRSCOPE(pages, "qPBReaderPagePos::Restore");

   if (_pDoc)
   {
      if (!_savedPos.isEmpty())
      {
         TRACE << "Using pos_cfi " << ENC(_savedPos) << endl;
         _pDoc->EvalJSString("paged_display.jump_to_cfi(\"" + _savedPos + "\")");
         _savedPos.clear();
      }

      else if (_dFallbackPos >= 0.)
      {
         TRACE << "Using fallback pos=" << _dFallbackPos << endl;
         _pDoc->SetScrollFraction(_dFallbackPos);
         _dFallbackPos = -1.;
      }
   }
}

// -----------------------------------------------------------------------------

QString qPBReaderPagePos::Get() const
{
   QString res;

   if (_pDoc)
   {
      res = _pDoc->EvalJSString("window.paged_display.current_cfi()");
   }

   return res;
}

// -----------------------------------------------------------------------------

void qPBReaderPagePos::Set(const QString & isPos) const
{
   if (_pDoc && !isPos.isEmpty())
   {
      _pDoc->EvalJSString("paged_display.jump_to_cfi(\"" + isPos + "\")");
   }
}

// -----------------------------------------------------------------------------
