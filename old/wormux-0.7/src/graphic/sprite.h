/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Sprite:     Simple sprite management
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 *****************************************************************************/

#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL.h>
#include <vector>
#include "spriteframe.h"
#include "spritecache.h"
#include "spriteanimation.h"
#include "include/base.h"

typedef enum {
  top_left, 
  top_center, 
  top_right, 
  left_center,
  center,
  right_center,
  bottom_left,
  bottom_center,
  bottom_right
} Rotation_HotSpot;

class Sprite
{
public:
  SpriteCache cache;
  SpriteAnimation animation;
	
public:
  explicit Sprite();
  explicit Sprite( Surface surface);
  Sprite(const Sprite &other);

  void Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y);
  Surface GetSurface();
   
  // Frame number
  unsigned int GetCurrentFrame() const;
  void SetCurrentFrame( unsigned int frame_no);    
  unsigned int GetFrameCount();

  // Size
  unsigned int GetWidth() const;
  unsigned int GetHeight() const;
  Point2i GetSize() const;
  void GetScaleFactors( float &scale_x, float &scale_y);
  void SetSize(unsigned int w, unsigned int h);
  void SetSize(const Point2i &size);
  void Scale( float scale_x, float scale_y);
  void ScaleSize(int width, int height);
  void ScaleSize(Point2i size);

  // Rotation
  void SetRotation_deg( float angle_deg);
  void SetRotation_HotSpot( Rotation_HotSpot rhs) {rot_hotspot = rhs;};
  
  SpriteFrame& operator[] (unsigned int frame_no);
  const SpriteFrame& operator[] (unsigned int frame_no) const;
  const SpriteFrame& GetCurrentFrameObject() const;

  // Prepare animation
  void AddFrame( const Surface& surf, unsigned int delay = 100);
  void SetFrameSpeed(unsigned int nv_fs);

  // Animation
  void Start();
  void Update();
  void Finish();
  bool IsFinished() const;

  // Alpha
  void SetAlpha( float alpha); // Can't be combined with per pixel alpha
  float GetAlpha();

  // Cache
  void EnableRotationCache(unsigned int cache_size);
  void EnableFlippingCache();

  // Show flag
  void Show();
  void Hide();
     
  // Draw
  void Blit(Surface &dest, uint pox_x, uint pos_y);
  void Blit(Surface &dest, const Point2i &pos);
  void Blit(Surface &dest, const Rectanglei &srcRect, const Point2i &destPos);
  void Blit(Surface &dest, int pox_x, int pos_y, int src_x, int src_y, uint w, uint h);
  void Draw(const Point2i &pos);

private:
   Surface current_surface;
   bool show;

   // Frames
   unsigned int current_frame;
   int frame_width_pix,frame_height_pix;
   std::vector<SpriteFrame> frames;
   
   // Gfx
   float alpha;
   float scale_x,scale_y;
   float rotation_deg;
   Rotation_HotSpot rot_hotspot;

private:
   void Constructor();
   void RefreshSurface();
   void Calculate_Rotation_Offset(int & rot_x, int & rot_y, Surface& tmp_surface);
};

#endif /* _SPRITE_H */
