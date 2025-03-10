/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Results menu
 *****************************************************************************/

#include "menu/results_menu.h"
#include <algorithm>  //std::sort

#include "character/character.h"
#include "character/damage_stats.h"
#include "game/game_time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/figure_widget.h"
#include "gui/graph_canvas.h"
#include "gui/label.h"
#include "gui/scroll_box.h"
#include "gui/null_widget.h"
#include "gui/tabs.h"
#include "gui/talk_box.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "sound/jukebox.h"
#include "team/results.h"
#include "team/team.h"
#include <WARMUX_debug.h>
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"

#define DEF_BORDER      8
#define DEF_SIZE       32
#define LINE_THICKNESS  2

#define GRAPH_BORDER        20
#define GRAPH_START_Y       400

static const Point2i BorderSize(DEF_BORDER, DEF_BORDER);
static const Point2i DefSize(DEF_SIZE, DEF_SIZE);

class ResultBox : public HBox
{
  Label  *category;
  Label  *character;
  Label  *score;
  Widget *img;
  std::string score_str;

  void SetWidgets(const std::string& type, const char* buffer, const Character* player)
  {
    margin = DEF_BORDER;
    border = BorderSize;

    Font::font_size_t font = Font::FONT_SMALL;

    category = new Label(type, W_UNDEF, font, Font::FONT_BOLD);
    AddWidget(category);
    character = new Label((player) ? player->GetName() : _("Nobody!"),
                          W_UNDEF, font, Font::FONT_BOLD);
    AddWidget(character);

    score_str = buffer;
    score = new Label(score_str, 30, font, Font::FONT_BOLD);
    AddWidget(score);

    if (player) {
      PictureWidget *team_picture = new PictureWidget(DefSize);
      team_picture->SetSurface(player->GetTeam().GetFlag());
      AddWidget(team_picture);
      img = team_picture;
    } else {
      img = new NullWidget(DefSize);
      AddWidget(img);
    }
  }

public:
  ResultBox(uint size, const std::string& type)
    : HBox(size, false, false, false)
  {
    SetWidgets(type, "?", NULL);
    Widget::SetBackgroundColor(transparent_color);
  }
  ResultBox(uint size, const std::string& type, uint score, const Character* player)
    : HBox(size, false, false, false)
  {
    char buffer[16];
    snprintf(buffer, 16, "%i", score);
    SetWidgets(type, buffer, player);
  }
  ResultBox(uint size, const std::string& type, float score, const Character* player)
    : HBox(size, false, false, false)
  {
    char buffer[16];
    snprintf(buffer, 16, "%.1f", score);
    SetWidgets(type, buffer, player);
  }
  void Pack()
  {
    int width = size.x - (4*margin + img->GetSizeX() + score->GetSizeX());
    category->SetSizeX(width/2);
    character->SetSizeX(width/2);

    HBox::Pack();
  }
};

class ResultListBox : public ScrollBox
{
public:
  ResultListBox(const TeamResults* res, const Point2i &size)
    : ScrollBox(size)
  {
    ResultBox       *box;
    const Character *player = res->getMostViolent();

    //Most violent
    if (player)
      box = new ResultBox(size.x, _("Most violent"), player->GetDamageStats()->GetMostDamage(), player);
    else
      box = new ResultBox(size.x, _("Most violent"));
    AddWidget(box);

    //Most useful
    player = res->getMostUseful();
    if (player)
      box = new ResultBox(size.x, _("Most useful"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useful"));
    AddWidget(box);

    //Most useless
    player = res->getMostUseless();
    if (player)
      box = new ResultBox(size.x, _("Most useless"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useless"));
    AddWidget(box);

    // Biggest sold-out
    player = res->getBiggestTraitor();
    if (player)
      box = new ResultBox(size.x, _("Most sold-out"), player->GetDamageStats()->GetFriendlyFireDamage(), player);
    else
      box = new ResultBox(size.x, _("Most sold-out"));
    AddWidget(box);

    // Most clumsy
    player = res->getMostClumsy();
    if (player)
      box = new ResultBox(size.x, _("Clumsiest"), player->GetDamageStats()->GetItselfDamage(), player);
    else
      box = new ResultBox(size.x, _("Clumsiest"));
    AddWidget(box);

    // Most accurate
    player = res->getMostAccurate();
    if (player)
      box = new ResultBox(size.x, _("Most accurate"), player->GetDamageStats()->GetAccuracy(), player);
    else
      box = new ResultBox(size.x, _("Most accurate"));
    AddWidget(box);
  }
};


//=========================================================

bool compareTeamResults(const TeamResults* a, const TeamResults* b)
{
  if (a->getTeam() == NULL)
    return false;
  if (b->getTeam() == NULL)
    return true;

  const Team* team_a = a->getTeam();
  const Team* team_b = b->getTeam();

  if (team_a->NbAliveCharacter() < team_b->NbAliveCharacter())
    return false;
  else if (team_a->NbAliveCharacter() > team_b->NbAliveCharacter())
    return true;

  // Same number of alive characters, compare left energy
  if (team_a->ReadEnergy() < team_b->ReadEnergy())
    return false;
  else if (team_a->ReadEnergy() > team_b->ReadEnergy())
    return true;

  // Same energy, probably 0, compare death_time
  return (a->GetDeathTime() > b->GetDeathTime());
}

//=========================================================

static bool IsPodiumSeparate()
{
  return GetMainWindow().GetSize() >= Point2i(640, 480);
}

ResultsMenu::ResultsMenu(std::vector<TeamResults*>& v, bool disconnected)
  : Menu("menu/bg_results", vOk)
  , results(v)
  , first_team(NULL)
  , second_team(NULL)
  , third_team(NULL)
  , msg_box(NULL)
  , winner_box(NULL)
{
  Profile *res  = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  Point2i wsize = GetMainWindow().GetSize();
  bool    small = !IsPodiumSeparate();
  uint x        = wsize.GetX() * 0.02;
  uint tab_x    = small ? x : 260+16+x;
  uint y        = wsize.GetY() * 0.02;

  if (!disconnected)
    ComputeTeamsOrder();

  // Load the podium img
  podium_img = GetResourceManager().LoadImage(res, "menu/podium");
  podium_img.SetAlpha(0, 0);
  GetResourceManager().UnLoadXMLProfile(res);

  Point2i tab_size = wsize - Point2i(tab_x + x, y+actions_buttons->GetSizeY());

  // Are we in network ? yes, so display a talkbox
  if (Network::IsConnected()) {
    int talk_size = tab_size.y/4;
    msg_box = new TalkBox(Point2i(tab_size.x, talk_size), Font::FONT_SMALL, Font::FONT_BOLD);
    tab_size.y -= talk_size;
  }

  tabs = new MultiTabs(tab_size);

  // Podium, drawn as first tab
  if (second_team)
    DrawTeamOnPodium(*second_team, Point2i(20,30));

  if (third_team)
    DrawTeamOnPodium(*third_team, Point2i(98,52));

  winner_box = new VBox(240, true, true, true);
  if (first_team) {
    Font::font_size_t title = (small) ? Font::FONT_MEDIUM : Font::FONT_BIG;
    Font::font_size_t txt   = (small) ? Font::FONT_SMALL : Font::FONT_MEDIUM;
    JukeBox::GetInstance()->Play("default", "victory");

    DrawTeamOnPodium(*first_team, Point2i(60,20));

    winner_box->AddWidget(new Label(_("Winner"), 240, title, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));
    PictureWidget* winner_logo = new PictureWidget(Point2i(64, 64)/(small+1));
    winner_logo->SetSurface(first_team->GetFlag());
    winner_box->AddWidget(winner_logo);
    winner_box->AddWidget(new Label(first_team->GetName(), 240, title, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));

    std::string tmp = _("Controlled by: ") + first_team->GetPlayerName();
    winner_box->AddWidget(new Label(tmp, 240, txt, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));
  }

  podium_img.SetAlpha(SDL_SRCALPHA, 0);
  podium = new PictureWidget(podium_img);
  winner_box->AddWidget(podium);
  if (IsPodiumSeparate()) {
    winner_box->SetPosition(x, y);
    widgets.AddWidget(winner_box);
    tabs->SetPosition(tab_x, y);
  } else {
    tabs->AddNewTab("TAB_podium", _("Podium"), winner_box);
  }

  // Create tabs for each team result
  stats = new MultiTabs(tab_size - 2*BorderSize);
  stats->SetMaxVisibleTabs(1);
  for (uint i=0; i<v.size(); i++) {
    const Team* team = v[i]->getTeam();
    const std::string name = (team) ? team->GetName() : _("All teams");
    stats->AddNewTab(name, name, new ResultListBox(v[i], tab_size - 4*BorderSize));
  }
  tabs->AddNewTab("TAB_team", _("Team stats"), stats);

  // Convert TeamResults to GraphCanvas::Results
  std::vector<GraphCanvas::Result> team_results;
  for (std::vector<TeamResults*>::const_iterator it = results.begin(); it != results.end(); ++it) {
    const Team* team = (*it)->getTeam();
    if (team) {
      const EnergyList& list = team->energy.energy_list;

      uint duration = list.GetDuration();
      if (duration) {
        GraphCanvas::Result r;
        r.ymax = list.GetMaxValue();
        r.xmax = duration*0.001f;
        r.color = team->GetColor();
        r.item = &team->GetFlag();
        for (uint i=0; i<list.size(); i++)
          r.list.push_back(std::make_pair(list[i]->GetDuration()*0.001f, list[i]->GetValue()));
        team_results.push_back(r);
      }
    }
  }
  tabs->AddNewTab("TAB_canvas", _("Team graphs"),
                  new GraphCanvas(tab_size - 2*BorderSize, _("Time"), _("Energy"), team_results));

  // Final box
  VBox* tmp_box = new VBox(tab_size.x, false, false, false);
  tmp_box->SetNoBorder();
  tmp_box->AddWidget(tabs);

  if (msg_box) {
    tmp_box->AddWidget(msg_box);
  }
  tmp_box->SetPosition(tab_x, y);

  widgets.AddWidget(tmp_box);
  widgets.Pack();
}

void ResultsMenu::ComputeTeamsOrder()
{
  std::sort(results.begin(), results.end(), compareTeamResults);

  first_team = results.at(0)->getTeam();
  second_team = results.at(1)->getTeam();
  if (results.size() > 3)
    third_team = results.at(2)->getTeam();
  else
    third_team = NULL;
}

void ResultsMenu::DrawTeamOnPodium(const Team& team, const Point2i& relative_position)
{
  Point2i flag_pos = team.GetFlag().GetSize()/2;
  Point2i position = relative_position - flag_pos;

  Surface tmp(team.GetFlag());
  podium_img.MergeSurface(tmp, position);
}

void ResultsMenu::key_ok()
{
  // return was pressed while chat texbox still had focus (player wants to send his msg)
  if (msg_box != NULL && msg_box->TextHasFocus())
  {
    msg_box->SendChatMsg();
    return;
  }
  Menu::key_ok();
}

void ResultsMenu::Draw(const Point2i &/*mousePosition*/)
{
  if (Network::IsConnected()) {
    ActionHandler * action_handler = ActionHandler::GetInstance();
    action_handler->ExecFrameLessActions();
  }
}

void ResultsMenu::ReceiveMsgCallback(const std::string& msg, const Color& color)
{
  if (msg_box) {
    msg_box->NewMessage(msg, color);
  }
}
