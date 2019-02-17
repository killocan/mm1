/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <cstdio>

#include "gameovermenu.h"
#include "text_utils.h"

#include "scenesoundmanager.h"
#include "sfx.h"

#include "globals.h"
#include "globalgamestate.h"

bool GameoverMenu::choice(BITMAP *buffer)
{
  int bgcolor = makecol(0, 112, 236);
  int white   = makecol(255, 255, 255);
  
  bool gameover = false;

  bool isKeyDown = false, isKeyUp = false;

  while (!key[KEY_ENTER])
  {
    if (key[KEY_UP] && isKeyUp == false)
    {
      Sounds::mm_sounds->play(MENU_ARROW);

      gameover = !gameover;

      isKeyUp = true;
    }
    else if (isKeyUp == true && (!key[KEY_UP]))
    {
      isKeyUp = false;
    }

    if (key[KEY_DOWN] && isKeyDown == false)
    {
      Sounds::mm_sounds->play(MENU_ARROW);

      gameover = !gameover;

      isKeyDown = true;
    }
    else if (isKeyDown == true && (!key[KEY_DOWN]))
    {
      isKeyDown = false;
    }

    clear_to_color(buffer, bgcolor);
    draw_text_shadow(buffer, Font::mm_font, 260, 250, white, "CONTINUE");
    draw_text_shadow(buffer, Font::mm_font, 260, 270, white, "GAMEOVER");

    Vsync::Sync();
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

  return false;
}
