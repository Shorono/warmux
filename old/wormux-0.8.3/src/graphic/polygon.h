/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Polygon class. Store point of a polygon and handle affine transformation
 *****************************************************************************/

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "tool/point.h"
#include "tool/rectangle.h"

// Forward declarations
class Color;
class AffineTransform2D;
class Surface;
class Sprite;

/** Use to draw the polygon */
class PolygonBuffer {
  /* if you need that, implement it (correctly)*/
  PolygonBuffer(const PolygonBuffer&);
  PolygonBuffer operator=(const PolygonBuffer&);
  /*********************************************/

 public:
  int16_t * vx;
  int16_t * vy;
  int buffer_size;
  int array_size;
  PolygonBuffer();
  ~PolygonBuffer();
  int GetSize() const;
  void SetSize(const int size);
};

/** Store information about a item (sprite) of the polygon */
class PolygonItem {

  /* if you need that, implement it (correctly)*/
  PolygonItem( PolygonItem&);
  PolygonItem operator=( PolygonItem&);
  /*********************************************/

 public:
  typedef enum { LEFT, H_CENTERED, RIGHT } H_align;
  typedef enum { TOP,  V_CENTERED, BOTTOM } V_align;
 protected:
  Point2d position;
  Point2d transformed_position;
  Sprite * item;
  H_align h_align;
  V_align v_align;
 protected:
  virtual Point2i GetOffsetAlignment() const;
 public:
  PolygonItem();
  PolygonItem(PolygonItem * item);
  PolygonItem(const Sprite * sprite, const Point2d & pos, H_align h_a = H_CENTERED, V_align v_a = V_CENTERED);
  virtual ~PolygonItem();
  void SetPosition(const Point2d & pos);
  void SetAlignment(H_align h_a = H_CENTERED, V_align v_a = V_CENTERED);
  H_align GetHAlign() const { return h_align; }
  V_align GetVAlign() const { return v_align; }
  Point2d & GetPosition();
  Point2d & GetTransformedPosition();
  Point2i GetIntTransformedPosition() const;
  virtual bool Contains(const Point2d & p) const;
  void SetSprite(Sprite * sprite);
  Sprite * GetSprite();
  virtual void ApplyTransformation(const AffineTransform2D & trans);
  virtual void Draw(Surface * dest);
};

/** Store information about a simple shape */
class Polygon {
 private:
  void Init();

 protected:
  bool is_closed;
  Surface * texture;
  Color * plane_color;
  Color * border_color;
  Point2d max;
  Point2d min;
  // Original shape
  std::vector<Point2d> original_shape;
  std::vector<Point2d> transformed_shape;
  // Vector of icons
  std::vector<PolygonItem *> items;
  // Shape position after an affine transformation
  PolygonBuffer * shape_buffer;
 private:
  Polygon operator=(const Polygon&);
 public:
  Polygon();
  Polygon(const std::vector<Point2d>& shape);
  Polygon(Polygon & poly);
  virtual ~Polygon();
  // Point handling
  void AddPoint(const Point2d & p);
  void InsertPoint(int index, const Point2d & p);
  void DeletePoint(int index);
  virtual void ApplyTransformation(const AffineTransform2D & trans, bool save_transformation = false);
  virtual void ResetTransformation();
  void SaveTransformation(const AffineTransform2D & trans);

  // Test
  bool IsInsidePolygon(const Point2d & point) const;
  bool IsOverlapping(const Polygon & poly) const;
  bool IsClockWise() const;

  // Use to randomize a construction
  Point2d GetRandomUpperPoint();
  int GetRandomPointIndex();

  // Interpolation handling
  void AddBezierCurve(const Point2d& anchor1, const Point2d& control1,
                      const Point2d& control2, const Point2d& anchor2,
                      const int num_steps = 20, const bool add_first_point = true,
                      const bool add_last_point = true);
  void AddRandomCurve(const Point2d& start, const Point2d& end,
                      const double x_random_offset, const double y_random_offset,
                      const int num_steps, const bool add_first_point = true,
                      const bool add_last_point = true);
  Polygon * GetBezierInterpolation(double smooth_value = 1.0, int num_steps = 20, double rand = 0.0);
  void Expand(double expand_value);

  // Size information
  double GetWidth() const;
  double GetHeight() const;
  Point2d GetSize() const;
  Point2i GetIntSize() const;
  int GetNbOfPoint() const;
  Point2d GetMin() const;
  Point2i GetIntMin() const;
  Point2d GetMax() const;
  Point2i GetIntMax() const;
  virtual Rectanglei GetRectangleToRefresh() const;

  // Buffer of transformed point
  PolygonBuffer * GetPolygonBuffer();

  // Type of the polygon
  bool IsTextured() const;
  bool IsPlaneColor() const;
  bool IsBordered() const;
  bool IsClosed() const;
  // Set type to Open
  void SetOpen();
  void SetClosed();

  // Texture handling
  Surface * GetTexture();
  void SetTexture(Surface * texture_surface);
  // Color handling
  void SetBorderColor(const Color & color);
  void SetPlaneColor(const Color & color);
  const Color & GetBorderColor() const;
  const Color & GetPlaneColor() const;

  // Drawing
  virtual void Draw(Surface * dest);
  virtual void DrawOnScreen();

  // Item management
  void AddItem(const Sprite * sprite, const Point2d & pos,
               PolygonItem::H_align h_a = PolygonItem::H_CENTERED,
               PolygonItem::V_align v_a = PolygonItem::V_CENTERED);
  virtual void AddItem(PolygonItem * item);
  void DelItem(int index);
  std::vector<PolygonItem *> GetItem() const;
  void ClearItem(bool free_mem = true);
};


class DecoratedBox : public Polygon
{
 public :
  enum Style {STYLE_ROUNDED, STYLE_SQUARE};

  DecoratedBox(double width, double height);
  ~DecoratedBox();
  virtual void Draw(Surface * dest);
  virtual void ApplyTransformation(const AffineTransform2D & trans, bool save_transformation);
  virtual void AddItem(PolygonItem * item);
  virtual void ResetTransformation();
  void SetPosition(double x, double y);
  void SetStyle(Style style);

 private :
  Point2d max_refresh;
  Point2d min_refresh;
  Point2d original_max;
  Point2d original_min;
  Surface *m_border;
  Style m_style;

  void GenerateBorder(Surface & source);



};

#endif /* POLYGON_H */
