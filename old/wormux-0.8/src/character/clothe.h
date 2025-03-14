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
 *****************************************************************************/

#ifndef CLOTHE_H
#define CLOTHE_H
#include <map>
#include <vector>

class Member;
typedef struct _xmlNode xmlNode;

class Clothe
{
public:
  std::string name;
  std::vector<Member*> layers;

  ~Clothe();
  Clothe(xmlNode* xml, std::map<std::string, Member*>& members_lst);
  Clothe(Clothe* c, std::map<std::string, Member*>& members_lst);
};

#endif //CLOTHE_H
