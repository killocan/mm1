/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <stdio.h>

#include "defines.h"
#include "globals.h"
#include "scenesoundmanager.h"
#include "intro.h"
#include "menu.h"
#include "globalgamestate.h"
#include "soundpack.h"

FONT              * Font::mm_font           = NULL;
BITMAP            * Buffer::buffer          = NULL;
SceneSoundManager * Sounds::mm_sounds       = NULL;
void (*Vsync::Sync)()                       = NULL;
unsigned long       Clock::clockTicks       = 0;
Camera            * GlobalCamera::mm_camera = NULL;
std::list<Character *> TemporaryCharacterList::mm_tempCharacterLst;
bool GameplayGlobals::bHoldLogic            = false;
bool GameplayGlobals::bWeaponUpdateRunning  = false;

volatile int fps       = 0;
volatile int fps_count = 0;
volatile int seconds   = 0;
static void check_fps()
{
  fps        += fps_count;
  fps_count  = 0;
  ++seconds;
}
END_OF_STATIC_FUNCTION(check_fps);

void do_nothing()
{
  return;
}

//Check frame rate with vsync on, if it drops bellow 50fps, vsync is turned off
//since the sincronization will slow down the engine. This may cause blitting
//artifacts on the final image, but its better than slow down on gameplay.
//This appears to be an (allegro+video driver) bug, anyway i try to "work around it".
void calibrate_refresh_rate()
{
  static const int SAMPLE_SIZE = 2;

  LOCK_VARIABLE(fps);
  LOCK_VARIABLE(fps_count);
  LOCK_VARIABLE(seconds);
  LOCK_FUNCTION(check_fps);
  install_int(check_fps, 1000);

  while(true)
  {
    if (seconds == SAMPLE_SIZE) break;

    clear_bitmap(Buffer::buffer);
    textprintf_ex(Buffer::buffer, font, 1, 1, makecol(255,255,255), -1, "Calibrating... [%d]", seconds);
    vsync();
    blit(Buffer::buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);

    ++fps_count;
  }

  remove_int(check_fps);

  float average_fps = ((float)fps)/((float)seconds);
  if (average_fps < 50.0f)
  {
    //fprintf(stderr,"sync off\n");
    Vsync::Sync = do_nothing;
  }
}

int main()
{
  allegro_init();

  install_keyboard();
  install_timer();
  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, mm_graphs_defs::UTIL_W, mm_graphs_defs::UTIL_H, 0, 0);

  END_OF_FUNCTION(GlobalGameState::update_game_logic_tick);
  LOCK_VARIABLE(GlobalGameState::game_logic_tick);
  LOCK_FUNCTION(GlobalGameState::update_game_logic_tick);
  install_int_ex(GlobalGameState::update_game_logic_tick, BPS_TO_TIMER(60));

  Buffer::buffer = create_bitmap(mm_graphs_defs::UTIL_W, mm_graphs_defs::UTIL_H);
  Font::mm_font  = load_font("game_data/font.pcx", NULL, NULL);
  Sounds::mm_sounds = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::SFX_SOUNDS]);

  Vsync::Sync = vsync;
  //calibrate_refresh_rate();
  
  clear(Buffer::buffer);

  //intro();
  menu();

  if (Sounds::mm_sounds) delete Sounds::mm_sounds;
  if (Font::mm_font)     destroy_font(Font::mm_font);
  if (Buffer::buffer)    destroy_bitmap(Buffer::buffer);

  return 0;
}
END_OF_MAIN()
