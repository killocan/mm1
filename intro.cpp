/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>

#include "intro.h"
#include "globals.h"

static const char * INTRO_TEXT[] = {"PRESS START",
                                    " ",
                                    " ",
                                    " ",
                                    "CAPCOM U.S.A., INC.",
                                    "LICENSED BY",
                                    "NINTENDO OF AMERICA INC.",
                                    "REMAKE BY KILLOCAN"};
static const int INTRO_TEXT_LEN = 8;

static void show(BITMAP * bmp, BITMAP * logo, FONT * fnt, bool show_press)
{
  const int white = makecol(255,255,255);
  const int h = text_height(fnt);
  int y = (SCREEN_H / 2) + 10;

  draw_sprite(bmp, logo, 0, 0);

  for (int i = 0; i < INTRO_TEXT_LEN; i++)
  {
    if (show_press == false && i == 0) ;
    else
    textout_centre_ex(bmp, fnt, INTRO_TEXT[i],
                      SCREEN_W / 2, y,
                      white, 0);

    y += h+((i?1:0)*6);
  }
}

void intro()
{
  bool bShow = false;

  BITMAP * buffer  = Buffer::buffer;
  FONT   * mm_font = Font::mm_font;

  BITMAP * logo = load_bitmap("game_data/logo.bmp", NULL);

  show(buffer, logo, mm_font, true);
  Vsync::Sync();
  blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

  int key = 0;
  while(true)
  {
    clear_keybuf();
    key = readkey() >> 8;
    if (key == KEY_ENTER) break;
  }
 
  for (int i = 0; i < 0x10; i++)
  {
    show(buffer, logo, mm_font, bShow);
    Vsync::Sync();
    blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
    clear_bitmap(buffer);

    bShow = !bShow;
    rest(110);
  }

  destroy_bitmap(logo);
}
