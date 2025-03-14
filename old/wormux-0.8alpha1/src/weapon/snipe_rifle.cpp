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
 * Snipe Rifle. Overide the Draw method in order to draw the laser beam.
 *****************************************************************************/

#include "snipe_rifle.h"
#include <sstream>
#include "explosion.h"
#include "../game/time.h"
#include "../interface/game_msg.h"
#include "../map/map.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../include/app.h"
#include "../game/game_loop.h"


const uint SNIPE_RIFLE_BEAM_START = 5;
const uint SNIPE_RIFLE_BULLET_SPEED = 20;
const uint SNIPE_RIFLE_MAX_BEAM_SIZE = 500;

SnipeBullet::SnipeBullet(ExplosiveWeaponConfig& cfg,
                     WeaponLauncher * p_launcher) :
    WeaponBullet("snipe_rifle_bullet", cfg, p_launcher)
{ 
}

void SnipeBullet::ShootSound()
{
  jukebox.Play("share","weapon/gun");
}

//-----------------------------------------------------------------------------

SnipeRifle::SnipeRifle() : WeaponLauncher(WEAPON_SNIPE_RIFLE,"snipe_rifle", new ExplosiveWeaponConfig())
{
  m_name = _("Sniper Rifle");

  last_angle = 0.0;
  targeting_something = false;
  m_laser_image = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_laser"));
  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_fire"));
  m_weapon_fire->EnableRotationCache(32);

  ReloadLauncher();
}

WeaponProjectile * SnipeRifle::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new SnipeBullet(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

bool SnipeRifle::p_Shoot()
{
  if(m_is_active)
    return false;

  m_is_active = true;
  ReloadLauncher();
  projectile->Shoot (SNIPE_RIFLE_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

// When an explosion occurs, we compute a new targeted point
void SnipeRifle::SignalProjectileGhostState()
{
  m_is_active = false;
  ReloadLauncher();
  ComputeCrossPoint(true);
}

bool SnipeRifle::ComputeCrossPoint(bool force = false)
{
  // Did the current character is moving ?
  Point2i pos = GetGunHolePosition();
  double angle = ActiveTeam().crosshair.GetAngleRad();
  if ( !force && last_rifle_pos == pos && last_angle == angle ) return targeting_something;
  else {
    last_rifle_pos=pos;
    last_angle=angle;
  }

  // Equation of movement : y = ax + b
  double a, b;
  a = sin(angle) / cos(angle);
  b = pos.y - ( a * pos.x );
  Point2i delta_pos, size, start_point;
  start_point = pos;
  uint distance = 0;
  targeting_something = false;
  // While test is not finished
  while( distance < SNIPE_RIFLE_MAX_BEAM_SIZE ){
    // going upwards ( -3pi/4 < angle <-pi/4 )
    if (angle < -0.78 && angle > -2.36){
      pos.x = (int)((pos.y-b)/a);       //Calculate x
      delta_pos.y = -1;                   //Increment y
    // going downwards ( 3pi/4 > angle > pi/4 )
    } else if (angle > 0.78 && angle < 2.36){
      pos.x = (int)((pos.y-b)/a);       //Calculate x
      delta_pos.y = 1;                    //Increment y
    // else going at right or left
    } else {
      pos.y = (int)((a*pos.x) + b);   //Calculate y
      delta_pos.x = ActiveCharacter().GetDirection();   //Increment x
    }
    // start point of the laser beam
    if ( distance < SNIPE_RIFLE_BEAM_START ) laser_beam_start = pos;

    // the point is outside the map
    if ( world.EstHorsMondeX(pos.x) || world.EstHorsMondeY(pos.y) ) break;

    // is there a collision ??
    if ( distance > 30 && !projectile->IsInVacuumXY( pos )){
      targeting_something = true;
      break;
    }
    pos += delta_pos;
    distance = (int) start_point.Distance(pos);
  }
  targeted_point=pos;
  return targeting_something;
}

// Reset crosshair when switching from a weapon to another to avoid misused
void SnipeRifle::p_Deselect()
{
  ActiveTeam().crosshair.ChangeAngleVal(0);
}

void SnipeRifle::DrawBeam()
{
  Point2i pos1 = laser_beam_start - camera.GetPosition();
  Point2i pos2 = targeted_point - camera.GetPosition();
  AppWormux::GetInstance()->video.window.AALineColor(pos1.x, pos2.x, pos1.y, pos2.y, Color(255, 0, 0, 100));

  // Set area of the screen to be redrawn:
  // Splited into little rectangles to avoid too large area of redraw
  float redraw_size = 20.0;
  float dst = laser_beam_start.Distance(targeted_point);
  Point2f pos = Point2f((float)laser_beam_start.x, (float)laser_beam_start.y);
  Point2f delta = ( Point2f((float)targeted_point.x, (float)targeted_point.y) - pos ) * redraw_size / dst;
  Point2i delta_i((int)delta.x, (int)delta.y);

  if(delta_i.x < 0) delta_i.x = - delta_i.x; // the Map::ToRedraw method doesn't support negative rectangles
  if(delta_i.y < 0) delta_i.y = - delta_i.y;
  delta_i.x += 6; // We have to increase the size of the rectangle so the corner of the rectangles overlaps
  delta_i.y += 6;

  int i = 0;
  while( (float)i * redraw_size < dst )
  {
    // float to int conversion...
    Point2i pos_i((int)pos.x, (int)pos.y);
    if(delta.x < 0.0)
    {
      pos_i.x -= delta_i.x;
      pos_i.x += 3;
    }
    else
      pos_i.x -= 3;

    if(delta.y < 0.0)
    {
      pos_i.y -= delta_i.y;
      pos_i.y += 3;
    }
    else
      pos_i.y -= 3;

    world.ToRedrawOnMap(Rectanglei( pos_i, delta_i ));
    pos += delta;
    i++;
  }
}

void SnipeRifle::Draw()
{
  WeaponLauncher::Draw();
  if( GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING || IsActive() ) return;
  ComputeCrossPoint();
  DrawBeam();
  // Draw the laser impact
  if( targeting_something ) m_laser_image->Draw(targeted_point - (m_laser_image->GetSize()/2));
}
