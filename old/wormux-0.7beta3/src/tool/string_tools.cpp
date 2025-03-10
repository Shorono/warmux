/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Refresh des cha�nes de caract�re (string).
 *****************************************************************************/

#include "../tool/string_tools.h"
//-----------------------------------------------------------------------------
#include <sstream>
//-----------------------------------------------------------------------------

ulong occurence_caractere (const std::string &txt, char x)
{
  ulong n=0;
  long pos = txt.find (x, 0);
  while (0 <= pos)
  {
    n++;
    pos = txt.find (x, pos+1);
  }
  return n;
}

//-----------------------------------------------------------------------------

bool str2long (const std::string &txt, long &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
//  return ss;
  return true;
}

//-----------------------------------------------------------------------------

bool str2int (const std::string &txt, int &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
//  return ss;
  return true;
}

//-----------------------------------------------------------------------------

bool str2double (const std::string &txt, double &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
//  return ss;
  return true;
}

//-----------------------------------------------------------------------------

std::string double2str (double x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

//-----------------------------------------------------------------------------

std::string long2str (long x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

//-----------------------------------------------------------------------------

std::string ulong2str (ulong x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

//-----------------------------------------------------------------------------
