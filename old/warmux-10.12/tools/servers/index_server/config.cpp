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

#include <iostream>
#include <string>
#include <fstream>
#include "config.h"

Config config;

Config::Config() : ServerConfig(true)
{}

void Config::Load(const std::string & config_file)
{
  ServerConfig::Load(config_file);
  SetDefault("port", 9997);
  SetDefault("working_dir", "warmux_log/");
  SetDefault("chroot", true);
  SetDefault("chroot_gid", 500);
  SetDefault("chroot_uid", 500);
  SetDefault("connexion_max", -2);
  SetDefault("local", false);
  SetDefault("verbose", true);
  Display();
}
