/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <cstdio>

#include "gameovermenu.h"
#include "text_utils.h"

#include "scenesoundmanager.h"
#include "sfx.h"
#include "soundpack.h"

#include "globals.h"
#include "globalgamestate.h"

bool GameoverMenu::choice(BITMAP *buffer)
{
  int bgcolor = makecol(0, 112, 236);
  int white   = makecol(255, 255, 255);
  
  bool gameover = false;

  bool isKeyUp = false;

  SceneSoundManager gameover_music(mm_soundpack::sounds[mm_soundpack::GAMEOVER_MUSIC]);
  gameover_music.play(0, false);

  while (!key[KEY_ENTER])
  {
    if (key[KEY_P] && isKeyUp == false)
    {
      gameover = !gameover;

      isKeyUp = true;
    }
    else if (isKeyUp == true && (!key[KEY_P]))
    {
      isKeyUp = false;
    }

    clear_to_color(buffer, bgcolor);
    textout_centre_ex(buffer, Font::mm_font, "GAME OVER", SCREEN_W/2, 145, white, -1);

    textprintf_centre_ex(buffer, Font::mm_font, SCREEN_W / 2, 170, white, -1, "%07d", GlobalGameState::bonus_points);

    char seta[2];
    seta[0] = '9' + 5;
    seta[1] = 0;
    int ypos[] = { 293, 323 };
    draw_text_shadow(buffer, Font::mm_font, SCREEN_W / 2 - 105, ypos[gameover], white, seta);

    textout_ex(buffer, Font::mm_font, "CONTINUE", SCREEN_W / 2 - 80, 290, white, -1);
    textout_ex(buffer, Font::mm_font, "STAGE SELECT", SCREEN_W / 2 - 80, 320, white, -1);

    Vsync::Sync();
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }
  while (key[KEY_ENTER]);

  return gameover;
}
