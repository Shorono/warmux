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
 * Label in GUI.
 *****************************************************************************/

#include "label.h"

Label::Label (const std::string &label, const Rectanglei &rect, Font& _font,
	      const Color& color, bool _center, bool _shadowed) 
  : font_color(color)
{
  position = rect.GetPosition();
  size = rect.GetSize();
  font = &_font;
  center = _center;
  shadowed = _shadowed;
  hidden = false;
  txt_label = new Text(label, font_color, &_font, shadowed);
  txt_label->SetMaxWidth(GetSizeX());
  size.y = txt_label->GetHeight();
}

Label::~Label()
{
  delete txt_label;
}

void Label::Draw(const Point2i &mousePosition, Surface& surf) const
{
  if (!hidden) 
    {
      if (!center)
	txt_label->DrawTopLeft(position);
      else
	txt_label->DrawCenterTop(position.x + size.x/2, position.y);
    }
}

void Label::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());
  size.y = txt_label->GetHeight();
}

void Label::SetText(const std::string &new_txt)
{
  need_redrawing = true;
  delete txt_label;
  txt_label = new Text(new_txt, font_color, font, shadowed);
  txt_label->SetMaxWidth(GetSizeX());
}

const std::string& Label::GetText() const
{
  return txt_label->GetText();
}

void Label::SetVisible(bool visible)
{
  if (hidden == visible) {
    hidden = !visible;
    need_redrawing = true;
  }
}
