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
 * Weapon dynamite : When fired, explode after a short laps of time. Then make a
 * big hole, eject character and made them lost energy.
 * Like a dynamite after all :)
 *****************************************************************************/

#ifndef DYNAMITE_H
#define DYNAMITE_H

#include "launcher.h"
#include "graphic/sprite.h"
#include "include/base.h"
#include "character/character.h"

class Dynamite;

class DynamiteStick : public WeaponProjectile
{
  int channel;

  public:
    DynamiteStick(ExplosiveWeaponConfig& cfg,
                  WeaponLauncher * p_launcher);

    void Shoot(double strength);
    void Refresh();

  protected:
    void ShootSound();
    void SignalExplosion();
    void SignalOutOfMap();
    void SignalDrowning();
};


// L'arme dynamite
class Dynamite : public WeaponLauncher
{
  protected:
    bool p_Shoot();
  protected:
    WeaponProjectile * GetProjectileInstance();
  public:
    Dynamite();
    DECLARE_GETWEAPONSTRING();
};
#endif /* DYNAMITE_H */
