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
 * Function used to format string.
 * Eg. : Format("Hello %s", "world") returns "Hello World".
 *****************************************************************************/

#ifndef FORMAT_H
#define FORMAT_H

#include "include/base.h"
#include <string>
#include <libintl.h>

#define _(X) gettext(X)

std::string Format (const char *format, ...);
void InitI18N(const std::string &dir);
void I18N_SetDir(const std::string &dir);

#endif
