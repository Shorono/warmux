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
 *  Refresh du vent
 *****************************************************************************/

#ifndef WIND_H
#define WIND_H

#include <SDL.h>
#include <list>
#include "../game/config.h"
#include "../graphic/sprite.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../tool/xml_document.h"

class WindParticle : public PhysicalObj
{
public:
  Sprite *sprite;

public:
  WindParticle(std::string& xml_file);
  inline WindParticle(const WindParticle & aparticle):
    PhysicalObj(aparticle)
    {
      assert(aparticle.sprite);
      sprite = new Sprite(*aparticle.sprite);
    };
  ~WindParticle() { delete sprite; };
  void Resize(double size);
  void Draw();
  void Refresh();
};

class Wind
{
  long m_val, m_nv_val;
  uint m_last_move;
  uint m_last_part_mvt;

public:
  std::list<WindParticle> particles;
  typedef std::list<WindParticle>::iterator iterator;

public:
  Wind();
  double GetStrength() const;
  void ChooseRandomVal();
  void SetVal (long val);
  void Refresh();
  void Reset();
  void DrawParticles();
  void RandomizeParticlesPos(); // Put particles randomly on the screen
};

extern Wind wind;
#endif

