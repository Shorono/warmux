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
 * Ninja-rope: les skins se balancent au bout d'une corde pour se balader sur le terrain
 *****************************************************************************/

#include "ninja_rope.h"
#include <math.h>
#include "explosion.h"
#include "../game/config.h"
#include "../game/game.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../include/app.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../sound/jukebox.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"

const int DT_MVT  = 15 ; //delta_t bitween 2 up/down/left/right mvt
const int DST_MIN = 6 ;  //dst_minimal bitween 2 nodes
const uint MAX_ROPE_LEN = 450 ; // Max rope length in pixels
const uint ROPE_DRAW_SPEED = 12 ; // Pixel per 1/100 second.
const int ROPE_PUSH_FORCE = 10;

bool find_first_contact_point (int x1, int y1, double angle, int length,
			       int skip, int &cx, int &cy)
{
  double x, y, x_step, y_step ;
  int x2, y2 ;

  x_step = cos(angle) ;
  y_step = sin(angle) ;

  x1 += (int)(skip * x_step) ;
  y1 += (int)(skip * y_step) ;

  x = (double)x1 ;
  y = (double)y1 ;
  cx = x1 ;
  cy = y1 ;

  length -= skip;

  x2 = x1 + (int)(length * cos(angle));
  y2 = y1 + (int)(length * sin(angle));

  while(!world.EstHorsMondeXY(cx, cy) &&
	(length > 0))
    {
      if (!world.EstDansVide(cx, cy))
	return true ;

      x += x_step ;
      y += y_step ;
      cx = (int)round(x) ;
      cy = (int)round(y) ;
      length-- ;
    }

  return false ;
}

NinjaRope::NinjaRope() : Weapon(WEAPON_NINJA_ROPE, "ninjarope", new WeaponConfig())
{
  m_name = _("Ninjarope");
  override_keys = true ;
  use_unit_on_first_shoot = false;

  m_hook_sprite = resource_manager.LoadSprite(weapons_res_profile,"ninjahook");
  m_hook_sprite->EnableRotationCache(32);
  m_node_sprite = resource_manager.LoadSprite(weapons_res_profile,"ninjanode");

  m_is_active = false;
  m_attaching = false;
  m_rope_attached = false;
  go_left = false ;
  go_right = false ;
  delta_len = 0 ;
}

bool NinjaRope::p_Shoot()
{
  last_broken_node_angle = 100;

  last_node = 0 ;
  m_attaching = true;
  m_launch_time = Time::GetInstance()->Read() ;
  m_initial_angle = ActiveCharacter().GetFiringAngle();
  last_mvt=Time::GetInstance()->Read();
  return true ;
}

void NinjaRope::TryAttachRope()
{
  int x, y;
  uint length;
  uint delta_time = Time::GetInstance()->Read() - m_launch_time;
  double angle ;

  // The rope is being launching. Increase the rope length and check
  // collisions.

  Point2i handPos = ActiveCharacter().GetHandPosition();
  x = handPos.x;
  y = handPos.y;

  length = ROPE_DRAW_SPEED * delta_time / 10;
  if (length > MAX_ROPE_LEN)
    {
      // Hum the roe is too short !
      m_attaching = false;
      m_is_active = false;
      return ;
    }

  angle = m_initial_angle;

  if (find_first_contact_point(x, y, angle, length, 4,
			       m_fixation_x, m_fixation_y))
    {
      m_attaching = false;

      int dx, dy;

      // The rope reaches the fixation point. Let's fix it !

      dx = x - ActiveCharacter().GetX() ;
      dy = y - ActiveCharacter().GetY() ;

      ActiveCharacter().SetPhysFixationPointXY(
					       m_fixation_x / PIXEL_PER_METER,
					       m_fixation_y / PIXEL_PER_METER,
					       (double)dx / PIXEL_PER_METER,
					       (double)dy / PIXEL_PER_METER);

      rope_node[0].x = m_fixation_x ;
      rope_node[0].y = m_fixation_y ;

      ActiveCharacter().ChangePhysRopeSize (-10.0 / PIXEL_PER_METER);
      m_hooked_time = Time::GetInstance()->Read();
      ActiveCharacter().SetMovement("ninja-rope");

     ActiveCharacter().SetFiringAngle(-M_PI / 3);

    }
  else
    {
      rope_node[0].x = x + (int)(length * cos(angle));
      rope_node[0].y = y + (int)(length * sin(angle));
    }
}

void NinjaRope::UnattachRope()
{
  ActiveCharacter().UnsetPhysFixationPoint() ;
  last_node = 0;
}

bool NinjaRope::TryAddNode(int CurrentSense)
{
  int dx, dy, lg, cx, cy;
  Point2d V;
  bool AddNode = false ;
  double angle, rope_angle;

  Point2i handPos = ActiveCharacter().GetHandPosition();

  // Compute distance between hands and rope fixation point.

  V.x = handPos.x - m_fixation_x;
  V.y = handPos.y - m_fixation_y;
  angle = V.ComputeAngle();
  lg = (int)V.Norm();

  if (lg < DST_MIN)
    return false;

  // Check if the rope collide something

  if (find_first_contact_point(m_fixation_x, m_fixation_y, angle, lg, 4,cx,cy))
    {
      rope_angle = ActiveCharacter().GetRopeAngle() ;

      if ( (last_broken_node_sense * CurrentSense > 0) &&
	   (fabs(last_broken_node_angle - rope_angle) < 0.1))
	return false ;

      // The rope has collided something...
      // Add a node on the rope and change the fixation point.

      dx = handPos.x - ActiveCharacter().GetX();
      dy = handPos.y - ActiveCharacter().GetY();

      ActiveCharacter().SetPhysFixationPointXY(cx / PIXEL_PER_METER,
					       cy / PIXEL_PER_METER,
					       (double)dx / PIXEL_PER_METER,
					       (double)dy / PIXEL_PER_METER);

      m_fixation_x = cx ;
      m_fixation_y = cy ;
      last_node++ ;
      rope_node[last_node].x = m_fixation_x ;
      rope_node[last_node].y = m_fixation_y ;
      rope_node[last_node].angle = rope_angle ;
      rope_node[last_node].sense = CurrentSense ;

      AddNode = true ;
    }

  return AddNode ;
}

bool NinjaRope::TryBreakNode(int CurrentSense)
{
  double CurrentAngle, NodeAngle ;
  int NodeSense ;
  double AngularSpeed ;
  bool BreakNode = false ;
  int dx, dy ;

  // Check if we can break a node.

  NodeSense = rope_node[last_node].sense ;
  NodeAngle = rope_node[last_node].angle ;
  AngularSpeed = ActiveCharacter().GetAngularSpeed() ;
  CurrentAngle = ActiveCharacter().GetRopeAngle() ;

  if ( (last_node != 0) &&              // We cannot break the initial node.
       (NodeSense * CurrentSense < 0) ) // Cannot break a node if we are in the
                                        // same sense of the node.
    {
      if ( (CurrentAngle > 0) &&
	   (AngularSpeed > 0) &&
	   (CurrentAngle > NodeAngle))
	BreakNode = true ;

      if ( (CurrentAngle > 0) &&
	   (AngularSpeed < 0) &&
	   (CurrentAngle < NodeAngle))
	BreakNode = true ;

      if ( (CurrentAngle < 0) &&
	   (AngularSpeed > 0) &&
	   (CurrentAngle > NodeAngle))
	BreakNode = true ;

      if ( (CurrentAngle < 0) &&
	   (AngularSpeed < 0) &&
	   (CurrentAngle < NodeAngle))
	BreakNode = true ;
    }

  // We can break the current node... Let's do it !

  if (BreakNode)
    {
      last_broken_node_angle = CurrentAngle ;
      last_broken_node_sense = CurrentSense ;

      last_node-- ;

      m_fixation_x = rope_node[last_node].x ;
      m_fixation_y = rope_node[last_node].y ;

      Point2i handPos = ActiveCharacter().GetHandPosition();
      dx = handPos.x - ActiveCharacter().GetX();
      dy = handPos.y - ActiveCharacter().GetY();

      ActiveCharacter().SetPhysFixationPointXY(m_fixation_x / PIXEL_PER_METER,
					       m_fixation_y / PIXEL_PER_METER,
					       (double)dx / PIXEL_PER_METER,
					       (double)dy / PIXEL_PER_METER);

    }

  return BreakNode ;
}

void NinjaRope::NotifyMove(bool collision)
{
  bool AddNode = false ;
  double AngularSpeed ;
  int CurrentSense ;

  if (!m_is_active)
    return ;

  // Check if the character collide something.

  if (collision)
    {
      // Yes there has been a collision.
      if (delta_len != 0)
	{
	  // The character tryed to change the rope size.
	  // There has been a collision, so we cancel the rope length change.
	  ActiveCharacter().ChangePhysRopeSize (-delta_len);
	  delta_len = 0 ;
	}
      return ;
    }

  AngularSpeed = ActiveCharacter().GetAngularSpeed() ;
  CurrentSense = (int)(AngularSpeed / fabs(AngularSpeed)) ;

  // While there is nodes to add, we add !
  while (TryAddNode(CurrentSense))
    AddNode = true ;

  // If we have created nodes, we exit to avoid breaking what we
  // have just done !
  if (AddNode)
    return ;

  // While there is nodes to break, we break !
  while (TryBreakNode(CurrentSense)) ;
}

void NinjaRope::Refresh()
{
  if (!m_is_active)
    return ;

  ActiveCharacter().UpdatePosition();
}

void NinjaRope::GoUp()
{
  if(Time::GetInstance()->Read()<last_mvt+DT_MVT)
    return;
  last_mvt = Time::GetInstance()->Read();

  delta_len = -0.1 ;
  ActiveCharacter().ChangePhysRopeSize (delta_len);
  ActiveCharacter().UpdatePosition();
  delta_len = 0 ;
}

void NinjaRope::GoDown()
{
  if(Time::GetInstance()->Read()<last_mvt+DT_MVT)
    return;
  last_mvt = Time::GetInstance()->Read();

  if (ActiveCharacter().GetRopeLength() >= MAX_ROPE_LEN / PIXEL_PER_METER)
    return;

  delta_len = 0.1 ;
  ActiveCharacter().ChangePhysRopeSize (delta_len) ;
  ActiveCharacter().UpdatePosition() ;
  delta_len = 0 ;
}

void NinjaRope::GoRight()
{
  go_right = true ;
  ActiveCharacter().SetExternForce(ROPE_PUSH_FORCE,0);
  ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
}

void NinjaRope::StopRight()
{
  go_right = false ;

  if (go_left || go_right)
    return ;

  ActiveCharacter().SetExternForce(0,0);
}

void NinjaRope::GoLeft()
{
  go_left = true ;
  ActiveCharacter().SetExternForce(-ROPE_PUSH_FORCE,0);
  ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
}

void NinjaRope::StopLeft()
{
  go_left = false ;

  if (go_left || go_right)
    return ;

  ActiveCharacter().SetExternForce(0,0);
}

void NinjaRope::Draw()
{
  int i, x, y;
  double angle, prev_angle;

  struct CL_Quad {Sint16 x1,x2,x3,x4,y1,y2,y3,y4;} quad;

  if (!m_is_active)
  {
    Weapon::Draw();
    return ;
  }

  if (m_attaching)
    {
      TryAttachRope();
      if (!m_is_active)
	      return ;
      if(m_attaching)
        angle = m_initial_angle + M_PI/2;
      else
        angle = ActiveCharacter().GetRopeAngle();
    }
  else
    angle = ActiveCharacter().GetRopeAngle();
  prev_angle = angle;


  // Draw the rope.

  Point2i handPos = ActiveCharacter().GetHandPosition();
  x = handPos.x;
  y = handPos.y;

  quad.x1 = (int)round((double)x - 2 * cos(angle));
  quad.y1 = (int)round((double)y + 2 * sin(angle));
  quad.x2 = (int)round((double)x + 2 * cos(angle));
  quad.y2 = (int)round((double)y - 2 * sin(angle));

  for (i = last_node ; i >= 0; i--)
    {
      quad.x3 = (int)round((double)rope_node[i].x + 2 * cos(angle));
      quad.y3 = (int)round((double)rope_node[i].y - 2 * sin(angle));
      quad.x4 = (int)round((double)rope_node[i].x - 2 * cos(angle));
      quad.y4 = (int)round((double)rope_node[i].y + 2 * sin(angle));

      float dx = sin(angle) * (float)m_node_sprite->GetHeight();
      float dy = cos(angle) * (float)m_node_sprite->GetHeight();
      int step = 0;
      int size = (quad.x1-quad.x4) * (quad.x1-quad.x4)
                +(quad.y1-quad.y4) * (quad.y1-quad.y4);
      size -= m_node_sprite->GetHeight();
      while( (step*dx*step*dx)+(step*dy*step*dy) < size )
      {
        if(m_attaching)
          m_node_sprite->Draw(
				  Point2i(
				  quad.x1 + (int)((float) step * dx),
				  quad.y1 - (int)((float) step * dy)) );
        else
          m_node_sprite->Draw( Point2i(
					  quad.x4 + (int)((float) step * dx),
                      quad.y4 + (int)((float) step * dy)) );
        step++;
      }
      quad.x1 = quad.x4 ;
      quad.y1 = quad.y4 ;
      quad.x2 = quad.x3 ;
      quad.y2 = quad.y3 ;
      prev_angle = angle;
      angle = rope_node[i].angle ;

    }

  m_hook_sprite->SetRotation_rad(-prev_angle);
  m_hook_sprite->Draw( Point2i(rope_node[0].x, rope_node[0].y)
		  - m_hook_sprite->GetSize()/2);
}

void NinjaRope::p_Deselect()
{
  m_is_active = false;
  ActiveCharacter().Show();
  ActiveCharacter().SetExternForce(0,0);
  ActiveCharacter().UnsetPhysFixationPoint() ;
}

void NinjaRope::HandleKeyEvent(Action::Action_t action, Keyboard::Key_Event_t event_type)
{
  switch (action) {
    case Action::ACTION_UP:
      if (event_type != Keyboard::KEY_RELEASED)
	GoUp();
      break ;

    case Action::ACTION_DOWN:
      if (event_type != Keyboard::KEY_RELEASED)
	GoDown();
      break ;

    case Action::ACTION_MOVE_LEFT:
      if (event_type == Keyboard::KEY_PRESSED)
	GoLeft();
      else
	if (event_type == Keyboard::KEY_RELEASED)
	  StopLeft();
      break ;

    case Action::ACTION_MOVE_RIGHT:
      if (event_type == Keyboard::KEY_PRESSED)
	GoRight();
      else
	if (event_type == Keyboard::KEY_RELEASED)
	  StopRight();
      break ;

    case Action::ACTION_SHOOT:
      if (event_type == Keyboard::KEY_PRESSED)
	UseAmmoUnit();
      break ;

    default:
      break ;
  } ;
}

void NinjaRope::SignalTurnEnd()
{
  p_Deselect();
}

EmptyWeaponConfig& NinjaRope::cfg()
{
  return static_cast<EmptyWeaponConfig&>(*extra_params);
}
