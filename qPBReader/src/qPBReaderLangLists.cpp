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

#include "qPBReaderLangLists.h"

#include <QStringList>

// -----------------------------------------------------------------------------

const QStringList & qPBReaderLangLists::GetAvailableApplicationLanguages()
{
   static QStringList ls;
   if (ls.size() == 0)
   {
      ls.append("en");
      ls.append("fr");
   }
   return ls;
}

// -----------------------------------------------------------------------------

const QStringList & qPBReaderLangLists::GetAvailableHyphenPatterns()
{
   static QStringList ls;
   if (ls.size() == 0)
   {
      ls.append("be");
      ls.append("bn");
      ls.append("ca");
      ls.append("cs");
      ls.append("da");
      ls.append("de");
      ls.append("el-monoton");
      ls.append("el-polyton");
      ls.append("en-gb");
      ls.append("en-us");
      ls.append("eo");
      ls.append("es");
      ls.append("et");
      ls.append("fi");
      ls.append("fr");
      ls.append("grc");
      ls.append("gu");
      ls.append("hi");
      ls.append("hu");
      ls.append("hy");
      ls.append("it");
      ls.append("kn");
      ls.append("la");
      ls.append("lt");
      ls.append("lv");
      ls.append("ml");
      ls.append("nb-no");
      ls.append("nl");
      ls.append("or");
      ls.append("pa");
      ls.append("pl");
      ls.append("pt");
      ls.append("ro");
      ls.append("ru");
      ls.append("sk");
      ls.append("sl");
      ls.append("sr-cyrl");
      ls.append("sr-latn");
      ls.append("sv");
      ls.append("ta");
      ls.append("te");
      ls.append("tr");
      ls.append("uk");
   }
   return ls;
}

// -----------------------------------------------------------------------------
