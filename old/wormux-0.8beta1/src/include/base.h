/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Declarations et types de base.
 *****************************************************************************/

#ifndef BASE_H
#define BASE_H
//-----------------------------------------------------------------------------

// Mode debogage ? Peut-�tre d�sactiv� en ajoutant -DRELEASE � la compilation
#ifndef DEBUG
#  define RELEASE
#endif

//-----------------------------------------------------------------------------

// Types non signes
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef unsigned char  uint8;
//typedef unsigned long uint32;
typedef signed char    sint8;
typedef signed long    sint32;

//-----------------------------------------------------------------------------

#define XSTR(x) #x
#define STR(x) XSTR(x)

//-----------------------------------------------------------------------------
#include "tool/error.h"
//-----------------------------------------------------------------------------
#endif
