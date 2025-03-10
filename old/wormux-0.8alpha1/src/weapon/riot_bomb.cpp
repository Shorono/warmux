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
 * riot bomb : fire a rocket that doesn't make any damage but makes a big
 * hole with angle and force
 *****************************************************************************/

#include "riot_bomb.h"
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

RoquetteRiotBomb::RoquetteRiotBomb(ExplosiveWeaponConfig& cfg,
                                   WeaponLauncher * p_launcher) :
  WeaponProjectile ("riot_rocket", cfg, p_launcher)
{  
  explode_colliding_character = true;
}

void RoquetteRiotBomb::Refresh()
{
  WeaponProjectile::Refresh();

  double angle = GetSpeedAngle() *180/M_PI;
  image->SetRotation_deg( angle);
}

void RoquetteRiotBomb::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();
}

void RoquetteRiotBomb::DoExplosion()
{
  Point2i pos = GetCenter();
  ApplyExplosion (pos, cfg, "weapon/explosion", false, ParticleEngine::LittleESmoke);
}
//-----------------------------------------------------------------------------

RiotBomb::RiotBomb() :
  WeaponLauncher(WEAPON_RIOT_BOMB, "riot_bomb", new ExplosiveWeaponConfig())
{  
  m_name = _("Riot Bomb");
  ReloadLauncher();
}

WeaponProjectile * RiotBomb::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new RoquetteRiotBomb(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}
