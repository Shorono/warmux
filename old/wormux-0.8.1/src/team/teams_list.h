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
 * Handle team list
 *****************************************************************************/

#ifndef TEAMS_LIST_H
#define TEAMS_LIST_H
//-----------------------------------------------------------------------------
#include <list>
#include <vector>
#include "include/singleton.h"
//-----------------------------------------------------------------------------

// Forward declarations
class Team;
class ConfigTeam;
class Character;

class TeamsList : public Singleton<TeamsList>
{
public:
  typedef std::list<Team *>::iterator full_iterator;
  typedef std::vector<Team *>::iterator iterator;
  std::list<Team *> full_list;
  std::vector<Team*> playing_list;

private:
  typedef std::list<uint>::iterator selection_iterator;
  std::list<uint> selection;
  std::vector<Team*>::iterator active_team;

  void LoadOneTeam (const std::string &dir, const std::string &file);
  void LoadList();

  void AddTeam(Team* the_team, int pos, const ConfigTeam& the_team_cfg, bool is_local);
  void UpdateTeam(Team* the_team, const ConfigTeam &the_team_cfg);
  void DelTeam(Team* the_team);

protected:
  friend class Singleton<TeamsList>;
  TeamsList();
  ~TeamsList();

public:
  friend TeamsList &GetTeamsList(void);
  void NextTeam();
  Team* GetNextTeam();
  Team& ActiveTeam();
  void LoadGamingData();
  void UnloadGamingData();
  void Clear();
  void RandomizeFirstPlayer();

  // Add a new team to playing, and change active team
  void AddTeam(const ConfigTeam& the_team_cfg, bool is_local, bool generate_error = true);
  void UpdateTeam(const std::string &old_team_id, const ConfigTeam& the_team_cfg);
  void DelTeam(const std::string &id);
  void SetActive(const std::string &id);
  void InitList(const std::list<ConfigTeam> &lst);
  void InitEnergy ();
  void RefreshEnergy (); //Refresh energy bar
  void RefreshSort (); //Refresh energy bar position
  void ChangeSelection (const std::list<uint>& liste);
  bool IsSelected (uint index);
  static bool IsLoaded() { return singleton != NULL; }

  // Find a team by its id or index (in full_list)
  Team* FindById (const std::string &id, int &pos);
  Team* FindByIndex (uint index);
  // Find a team by its id or index (in playing full_list)
  Team* FindPlayingById(const std::string &id, int &index);
  Team* FindPlayingByIndex(uint index);

  // returns name1+name2+name3
  std::string GetLocalHeadCommanders() const;
};

//-----------------------------------------------------------------------------

// current active team
Team& ActiveTeam();

// current active character
Character& ActiveCharacter();

//-----------------------------------------------------------------------------

bool compareTeams(const Team *a, const Team *b);

inline TeamsList &GetTeamsList(void) { return TeamsList::GetRef(); };

//-----------------------------------------------------------------------------
#endif
