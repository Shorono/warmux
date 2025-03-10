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
 * Affiche un message dans le jeu, puis pose une question dans le jeu ou
 * attend au moins la pression d'une touche.
 *****************************************************************************/

#ifndef QUESTION_H
#define QUESTION_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include <string>
#include <list>
#include <SDL_events.h>

//-----------------------------------------------------------------------------

class Question
{
public:
  // Message to display
  std::string message;

  // A choice = a key return a value
  typedef struct choix_t
  {
    int m_key;
    int m_val;
    choix_t (int key, int value) 
    { m_key = key; m_val = value; }
  } choice_t;

  // Choices list
  std::list<choice_t> choices;
  typedef std::list<choix_t>::iterator choice_iterator;

  // Default choice used when another key is pressed
  struct s_default_choice
  {
    bool active;
    int value;
  } default_choice;

 private:
  int TreatsKey (SDL_Event &event);
  void Draw();

 public:
  Question();
  void Set(const std::string &message, 
	   bool default_active, 
	   int default_value);
  int AskQuestion ();
};

//-----------------------------------------------------------------------------
#endif
