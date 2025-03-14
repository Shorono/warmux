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
 * Refresh des armes.
 *****************************************************************************/

#ifndef WEAPONS_LIST_H
#define WEAPONS_LIST_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "weapon.h"
#include <list>
#include <map>
//-----------------------------------------------------------------------------

// Classe de gestion des armes
class WeaponsList
{
public:
  typedef std::list<Weapon*> weapons_list_type;
  typedef std::list<Weapon*>::iterator weapons_list_it;
  
private:
  weapons_list_type m_weapons_list;
  // **EVIL HACK**
  weapons_list_type todelete;
  // **EVIL HACK**

  // The int is used to classify weapon by sort
  std::multimap<uint, Weapon*> m_weapons_map;
  typedef std::multimap<uint, Weapon*>::value_type keybind;
  void InitAndAddToList(Weapon* arme, uint num_sort);

public:
  WeaponsList();
  ~WeaponsList();
  void Init();

  // Refresh des armes
  // Retourne true si c'est la fin d'un tour
  void Refresh();

  // Return a list of  weapons
  weapons_list_type& GetList();
  Weapon* GetWeapon(Weapon_type type);
  bool GetWeaponBySort(uint num_sort, Weapon_type &type);
};

extern WeaponsList weapons_list;

//-----------------------------------------------------------------------------
#endif
