/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Arme bazooka : projette une roquette avec un angle et une force donn�.
 *****************************************************************************/

#include "bazooka.h"
#include "explosion.h"
#include "../game/config.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"

RoquetteBazooka::RoquetteBazooka(ExplosiveWeaponConfig& cfg,
                                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("rocket", cfg,p_launcher), smoke_engine(20)
{
  explode_colliding_character = true;
}

void RoquetteBazooka::Refresh()
{
  WeaponProjectile::Refresh();

  double angle = GetSpeedAngle() *180/M_PI;
  image->SetRotation_deg( angle);
  smoke_engine.AddPeriodic(Point2i(GetX() + GetWidth() / 2,
                                   GetY() + GetHeight()/ 2), particle_DARK_SMOKE, false, -1, 2.0);
}

void RoquetteBazooka::SignalOutOfMap()
{ 
  GameMessages::GetInstance()->Add (_("The rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

Bazooka::Bazooka() :
  WeaponLauncher(WEAPON_BAZOOKA, "bazooka", new ExplosiveWeaponConfig())
{  
  m_name = _("Bazooka");
  ReloadLauncher();
}

WeaponProjectile * Bazooka::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new RoquetteBazooka(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}
