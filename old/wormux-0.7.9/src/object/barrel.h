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
 * Petrol barrels
 *****************************************************************************/

#ifndef BARREL_H
#define BARREL_H

#include "physical_obj.h"
#include "../graphic/sprite.h"


class PetrolBarrel : public PhysicalObj
{
  Sprite* img;
  public:
    PetrolBarrel();
    ~PetrolBarrel();
    void Draw();
    void Refresh();
  protected:
    void SignalGhostState(bool was_already_dead);
};

#endif /* BARREL_H */
