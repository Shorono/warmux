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
 *  Starting file. (the 'main' function is here.)
 *****************************************************************************/

#include <getopt.h>
#ifndef WIN32
# include <signal.h>
#endif
#include <SDL.h>
#include <WARMUX_singleton.h>
#include "game/config.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "map/map.h"
#include "menu/benchmark_menu.h"
#include "menu/credits_menu.h"
#include "menu/game_menu.h"
#include "menu/help_menu.h"
#include "menu/main_menu.h"
#include "menu/network_connection_menu.h"
#include "menu/options_menu.h"
#include "network/randomsync.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "tool/stats.h"
#ifdef MAEMO
#include "maemo/osso.h"
#include "menu/pause_menu.h"
#endif
#ifdef WMX_LOG
# include "include/debugmasks.h"
#endif

static MainMenu::menu_item choice = MainMenu::NONE;
static bool skip_menu = false;
static NetworkConnectionMenu::network_menu_action_t net_action = NetworkConnectionMenu::NET_NOTHING;

AppWarmux *AppWarmux::singleton = NULL;

AppWarmux *AppWarmux::GetInstance()
{
  if (!singleton) {
    singleton = new AppWarmux();
  }
  return singleton;
}

AppWarmux::AppWarmux():
  video(new Video()),
  menu(NULL)
{
  RandomLocal().InitRandom();
  std::cout << "[ " << _("Run game") << " ]" << std::endl;
}

AppWarmux::~AppWarmux()
{
  delete video;
  ParticleEngine::FreeMem();
  Font::ReleaseInstances();
  BaseSingleton::ReleaseSingletons();
  singleton = NULL;
}

int AppWarmux::Main(void)
{
  bool quit = false;

  DisplayLoadingPicture();

  // Now that we are displaying a kind of 'please wait', do preload sounds
  JukeBox::GetInstance()->Init();

#ifdef HAVE_LIBCURL
  OptionMenu::CheckUpdates();
#endif

  Action_Handler_Init();

  do {
    if (choice == MainMenu::NONE) {
      MainMenu main_menu;
      SetCurrentMenu(&main_menu);
      choice = main_menu.Run();
    }

    ActionHandler::GetInstance()->Flush();

    switch (choice) {
      case MainMenu::PLAY:
      {
        GameMenu game_menu;
        SetCurrentMenu(&game_menu);
        game_menu.Run(skip_menu);
        break;
      }
      case MainMenu::NETWORK:
      {
        NetworkConnectionMenu network_connection_menu(net_action);
        SetCurrentMenu(&network_connection_menu);
        network_connection_menu.Run(skip_menu);
        break;
      }
      case MainMenu::OPTIONS:
      {
        OptionMenu options_menu;
        SetCurrentMenu(&options_menu);
        options_menu.Run();
        break;
      }
      case MainMenu::BENCHMARK:
      {
        BenchmarkMenu bench_menu;
        SetCurrentMenu(&bench_menu);
        bench_menu.Run();
        break;
      }
      case MainMenu::HELP:
      {
        HelpMenu help_menu;
        SetCurrentMenu(&help_menu);
        help_menu.Run();
        break;
      }
      case MainMenu::CREDITS:
      {
        CreditsMenu credits_menu;
        SetCurrentMenu(&credits_menu);
        credits_menu.Run();
        break;
      }
      case MainMenu::QUIT:
        quit = true;
        break;
      default:
        break;
    }
    SetCurrentMenu(NULL);
    choice = MainMenu::NONE;
    skip_menu = false;
    net_action = NetworkConnectionMenu::NET_NOTHING;
  }
  while (!quit);

  End();

  return 0;
}

void AppWarmux::DisplayLoadingPicture()
{
  Config *config = Config::GetInstance();

  std::string txt_version =
    _("Version") + std::string(" ") + Constants::WARMUX_VERSION;
  std::string filename = config->GetDataDir() + "menu" PATH_SEPARATOR "background_loading.jpg";

  Surface surfaceLoading(filename.c_str());
  Sprite loading_image(surfaceLoading);

  loading_image.ScaleSize(video->window.GetSize());
  loading_image.Blit(video->window, 0, 0);

  Time::GetInstance()->Reset();

  Text text1(_("Warmux launching..."), white_color,
             Font::FONT_HUGE, Font::FONT_BOLD, true);
  Text text2(txt_version, white_color, Font::FONT_HUGE, Font::FONT_BOLD,
             true);

  Point2i windowCenter = video->window.GetSize() / 2;

  text1.DrawCenter(windowCenter);
  text2.DrawCenter(windowCenter
                   + Point2i(0, Font::GetInstance(Font::FONT_HUGE, Font::FONT_BOLD)->GetHeight() + 20));

  video->window.Flip();
}

void AppWarmux::RefreshDisplay()
{
  if (Game::IsRunning()) {
    if (Game::GetCurrentMenu()) {
      Game::GetCurrentMenu()->RedrawMenu();
      return;
    } else {
      GetWorld().DrawSky(true);
      GetWorld().Draw(true);
    }
  }
  else if (GetCurrentMenu()) {
    GetCurrentMenu()->RedrawMenu();
  }
}

void AppWarmux::DisplayError(const std::string &msg)
{
  std::cerr << msg << std::endl;

  if (Game::IsRunning()) {
    if (Game::GetCurrentMenu()) {
      Game::GetCurrentMenu()->DisplayError(msg);
    }
    // nothing to do
  } else if (singleton->GetCurrentMenu()) {
    singleton->GetCurrentMenu()->DisplayError(msg);
  }
}

void AppWarmux::ReceiveMsgCallback(const std::string& msg, const Color& color)
{
  if (Game::IsRunning()) {
    if (Game::GetCurrentMenu()) {
      // Drop message, we should be paused anyway
    } else {
      // Add message to chat session in Game
      Game::GetInstance()->chatsession.NewMessage(msg, color);
    }
  } else if (GetCurrentMenu()) {
    GetCurrentMenu()->ReceiveMsgCallback(msg, color);
  }
}

void AppWarmux::End() const
{
  std::cout << std::endl << "[ " << _("Quit Warmux") << " ]" << std::endl;

  /* FIXME calling Config->Save here sucks: it nothing was ever done, it loads
   * the whole stuff just before exiting... This should be moved, but where? */
  Config::GetInstance()->Save();

  JukeBox::GetInstance()->End();

#ifdef ENABLE_STATS
  SaveStatToXML("stats.xml");
#endif
  std::cout << "o " << _("If you found a bug or have a feature request "
                    "send us an email (in english, please):")
    << " " << Constants::EMAIL << std::endl;
}

bool AppWarmux::CheckInactive(SDL_Event& evnt)
{
#ifdef MAEMO
  bool pause_all = true;
  Osso::Process();
  if (evnt.type==SDL_ACTIVEEVENT) {
#else

# ifdef ANDROID
  bool pause_all = true;
# else
  bool pause_all = false;
# endif

  if (evnt.type==SDL_ACTIVEEVENT && evnt.active.state&SDL_APPACTIVE) {
#endif
    if (Network::IsConnected()) {
      switch (evnt.active.gain) {
      case 0: JukeBox::GetInstance()->Pause(pause_all); return true;
      case 1: JukeBox::GetInstance()->Resume(pause_all); return true;
      default: break;
      }
    }
    else if (evnt.active.gain == 0) {
#ifdef HAVE_HANDHELD
      JukeBox::GetInstance()->CloseDevice();
#else
      JukeBox::GetInstance()->Pause();
#endif
      Time::GetInstance()->SetWaitingForUser(true);
      while (SDL_WaitEvent(&evnt)) {
#ifdef MAEMO
  Osso::Process();
#endif
        if (evnt.type == SDL_QUIT) AppWarmux::EmergencyExit();
        if (evnt.type == SDL_ACTIVEEVENT && evnt.active.gain == 1) {
#ifdef HAVE_HANDHELD
    JukeBox::GetInstance()->OpenDevice();
    JukeBox::GetInstance()->NextMusic();
#else
          JukeBox::GetInstance()->Resume();
#endif
          Time::GetInstance()->SetWaitingForUser(false);
          break;
        }
      }
      return true;
    }
  }
  return false;
}

void AppWarmux::EmergencyExit()
{
  delete AppWarmux::GetInstance();
  exit(EXIT_SUCCESS);
}

void DisplayWelcomeMessage()
{
  std::cout << "=== " << _("Warmux version ") << Constants::WARMUX_VERSION << std::endl;
  std::cout << "=== " << _("Authors:") << ' ';
  for (std::vector < std::string >::iterator it = Constants::GetInstance()->AUTHORS.begin(),
       fin = Constants::GetInstance()->AUTHORS.end(); it != fin; ++it)
    {
      if (it != Constants::GetInstance()->AUTHORS.begin())
        std::cout << ", ";
      std::cout << *it;
    }
  std::cout << std::endl
    << "=== " << _("Website: ") << Constants::WEB_SITE << std::endl
    << std::endl;

  // print the disclaimer
  std::cout << "Warmux version " << Constants::WARMUX_VERSION
    << ", Copyright (C) 2001-2011 Warmux Team" << std::endl
    << "Warmux comes with ABSOLUTELY NO WARRANTY." << std::endl
    << "This is free software and you are welcome to redistribute it" << std::endl
    << "under certain conditions." << std::endl << std::endl
    << "Read the file COPYING for details." << std::endl << std::endl;

#ifdef DEBUG
  std::cout << "This program was compiled in DEBUG mode (development version)"
       << std::endl << std::endl;
#endif
}

void PrintUsage(const char* cmd_name)
{
  printf("usage: \n");
  printf("%s -h|--help : show this help\n", cmd_name);
  printf("%s -v|--version : show the version\n", cmd_name);
  printf("%s -r|--reset-config : reset the configuration to default\n", cmd_name);
  printf("%s [-p|--play] [-g|--game-mode <game_mode>]"
         " [-s|--server] [-c|--client [ip]]\n"
         " [-i|--index-server] [ip/hostname of index server]]\n"
#ifdef WMX_LOG
         " [-d|--debug <debug_masks>|all]\n"
#endif
         , cmd_name);
#ifdef WMX_LOG
  printf("\nWith :\n");
  printf(" <debug_masks> ::= { %s }\n", used_debug_masks.c_str());
#endif
}

void ParseArgs(int argc, char * argv[])
{
  int c;
  int option_index = 0;
  struct option long_options[] = {
    {"unrandom",   no_argument,       NULL, 'u'},
    {"force-refresh", no_argument,    NULL, 'f'},
    {"help",       no_argument,       NULL, 'h'},
    {"blitz",      no_argument,       NULL, 'b'},
    {"version",    no_argument,       NULL, 'v'},
    {"play",       no_argument,       NULL, 'p'},
    {"client",     optional_argument, NULL, 'c'},
    {"server",     no_argument,       NULL, 's'},
    {"index-server", optional_argument, NULL, 'i'},
    {"game-mode",  required_argument, NULL, 'g'},
    {"debug",      required_argument, NULL, 'd'},
    {"reset-config", no_argument,     NULL, 'r'},
    {NULL,         no_argument,       NULL,  0 }
  };

  while ((c = getopt_long (argc, argv, "ufhbvpc::i::sg:d:",
                           long_options, &option_index)) != -1) {
    switch (c) {
    case 'u':
      RandomSync().UnRandom();
      RandomLocal().UnRandom();
      break;
    case 'f':
      extern bool force_refresh;
      force_refresh = true;
      break;
    case 'h':
      PrintUsage(argv[0]);
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      DisplayWelcomeMessage();
      exit(EXIT_SUCCESS);
      break;
    case 'p':
      choice = MainMenu::PLAY;
      skip_menu = true;
      break;
    case 'c':
      choice = MainMenu::NETWORK;
      net_action = NetworkConnectionMenu::NET_CONNECT;
      if (optarg)
        {
          Config::GetInstance()->SetNetworkClientHost(optarg);
        }
      skip_menu = true;
      break;
    case 'd':
#ifdef WMX_LOG
      printf("Debug: %s\n", optarg);
      AddDebugMode(optarg);
#else
      fprintf(stderr, "Option -d is not available. Warmux has not been compiled with debug/logging option.\n");
#endif
      break;
    case 's':
      choice = MainMenu::NETWORK;
      net_action = NetworkConnectionMenu::NET_HOST;
      skip_menu = true;
      break;
    case 'i':
      {
        std::string index_server_address;
        if (optarg) index_server_address = optarg;
        else index_server_address = "127.0.0.1";
        printf("Using %s as address for index server. This option must be used only for debugging.\n",
               index_server_address.c_str());
        IndexServer::GetInstance()->SetAddress(index_server_address.c_str());
      }
      break;
    case 'g':
      printf("Game-mode: %s\n", optarg);
      Config::GetInstance()->SetGameMode(optarg);
      break;
    case 'r':
      {
        bool r;
        r = Config::GetInstance()->RemovePersonalConfigFile();
        if (!r)
          exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
      }
      break;

    case '?': /* returns by getopt if option was invalid */
      PrintUsage(argv[0]);
      exit(EXIT_FAILURE);
      break;

    default:
      fprintf(stderr, "Sorry, it seems that option '-%c' is not implemented!\n", c);
      ASSERT(false);
      exit(EXIT_FAILURE);
      break;
    }
  }
}

extern "C" int main(int argc, char *argv[])
{
#ifndef WIN32
  signal(SIGPIPE, SIG_IGN);
#endif

#ifdef GEKKO
  // External library libfat, but let's not bother including its header
  extern void fatInitDefault(void);
  fatInitDefault();
  freopen("sd:/stdout.txt", "wt", stdout);
  freopen("sd:/stderr.txt", "wt", stderr);
  fprintf(stdout, "Trying to force opening\n");
#endif

#ifdef __SYMBIAN32__
  freopen("warmux_stdout.txt", "w", stdout);
  freopen("warmux_stderr.txt", "w", stderr);
#endif

  /* FIXME calling Config::GetInstance here means that there is no need of
   * singleton for Config but simply a global variable. This may look stange
   * but the whole system (directories, translation etc...) is needed, even for
   * the ParseArgs and DisplayWelcomeMessage functions. */
  Config::GetInstance(); // init config first, because it initializes i18n

  ParseArgs(argc, argv);

  DisplayWelcomeMessage();

  if (SDL_Init(SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "Sorry, couldn't initialize SDL'!\n");
    exit(EXIT_FAILURE);
  }

#ifdef MAEMO
  Osso::Init();
#endif

  AppWarmux::GetInstance()->Main();
  delete AppWarmux::GetInstance();
  SDL_Quit();
  exit(EXIT_SUCCESS);
}
