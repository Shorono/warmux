/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Warmux Team.
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
 ******************************************************************************/

#ifndef WARMUX_SERVER_LOGFILE_H
#define WARMUX_SERVER_LOGFILE_H

#include <string>
#include <stdio.h>

class LogFile
{
 protected:
  std::string suffix_filename;
  FILE* fd;

  // may be useful to write a file header
  virtual void AtOpen() {};

  // may be useful to write a file footer
  virtual void AtClose() {};

 public:
  LogFile(const std::string &suffix);
  ~LogFile();

  void OpenFile();
  void CloseFile();
  void Rotate();
};

#endif
