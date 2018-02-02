/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <stdio.h>

#include "text_utils.h"
    
void draw_text(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int bg, const char * str, int lenghtToPrint)
{
  //static const int charLen = text_length(mm_font, "A");
  char format[20] = {};
  sprintf(format, "%%.%ds", lenghtToPrint);

  for (unsigned int i = 0; i < strlen(str); i++)
  {
    textprintf_ex(buffer, mm_font, x, y, fg, bg, format, str);
    //x+=charLen;
  }
}

void draw_text_shadow(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, const char * str)
{
  if (fg == 0) return;

  textprintf_ex(buffer, mm_font, x+2, y+2, 0,  -1, "%s", str);
  textprintf_ex(buffer, mm_font, x,   y,   fg, -1, "%s", str);
}

void draw_number(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int bg, int number)
{
  //int rndNumber = (rand()%10);
 // #define oht 100*1000

  //if (number >= oht)
  //{
  //  rndNumber = (rndNumber*oht) + (number%oht);
  textprintf_ex(buffer, mm_font, x + 2, y + 2, 0, bg, "%d  ", number);
  textprintf_ex(buffer, mm_font, x, y, fg, -1, "%d  ", number);
 // }
 // else
 // {
  //  textprintf_ex(buffer, mm_font, x + 2, y + 2, 0, bg, "%d", number);
  //  textprintf_ex(buffer, mm_font, x, y, fg, -1, "%d", number);

    //textprintf_ex(buffer, mm_font, x + 2, y + 2, 0, bg, "%d", rndNumber);
    //textprintf_ex(buffer, mm_font, x, y, fg, -1, "%d", rndNumber);
  //}
}
