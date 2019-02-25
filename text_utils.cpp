/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cstdio>

#include "text_utils.h"
    
void draw_text(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int bg, const char * str, int lenghtToPrint)
{
  char format[20] = {};
  sprintf(format, "%%.%ds", lenghtToPrint);

  for (unsigned int i = 0; i < strlen(str); i++)
  {
    textprintf_ex(buffer, mm_font, x, y, fg, bg, format, str);
  }
}

void draw_text_shadow(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, const char * str)
{
  if (fg == 0) return;

  textprintf_ex(buffer, mm_font, x+2, y+2, 0,  -1, "%s", str);
  textprintf_ex(buffer, mm_font, x,   y,   fg, -1, "%s", str);
}

void draw_text_center_shadow(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, const char * str)
{
  textprintf_centre_ex(buffer, mm_font, x+2, y+2, 0,  -1, "%s", str);
  textprintf_centre_ex(buffer, mm_font, x,   y,   fg, -1, "%s", str);
}

void draw_number(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int number, int zeros)
{
  char mask[80] = {};
  sprintf(mask, "%%0%dd", zeros);

  textprintf_ex(buffer, mm_font, x + 2, y + 2, 0,  -1, mask, number);
  textprintf_ex(buffer, mm_font, x,     y,     fg, -1, mask, number);
}

void draw_number_center(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int number, int zeros)
{
  char mask[80] = {};
  sprintf(mask, "%%0%dd", zeros);

  textprintf_centre_ex(buffer, mm_font, x+2, y+2, 0,  -1, mask, number);
  textprintf_centre_ex(buffer, mm_font, x,   y,   fg, -1, mask, number);
}
