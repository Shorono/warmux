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
 ******************************************************************************
 * Polecat : send a polecat to the enemy. Close character get sick with the mefitic odor.
 *****************************************************************************/

#ifndef POLECAT_H
#define POLECAT_H

#include "weapon/weapon_launcher.h"
#include <WARMUX_base.h>

class Polecat;

class PolecatLauncher : public WeaponLauncher
{
private:
  Polecat *current_polecat;
  uint polecat_death_time;

public:
  PolecatLauncher();

  virtual void SignalEndOfProjectile();
  virtual void SignalProjectileCollision() { };
  virtual void SignalProjectileDrowning() { };

  bool IsOnCooldownFromShot() const;
  bool IsReady() const;

  void StopShooting();

  virtual bool IsPreventingLRMovement();
  virtual bool IsPreventingJumps();
  virtual bool IsPreventingWeaponAngleChanges();

  void UpdateTranslationStrings();
  std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
protected:
  virtual bool p_Shoot();
  virtual void Refresh();
  WeaponProjectile * GetProjectileInstance();
};

#endif /* POLECAT_H */
