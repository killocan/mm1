/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once
    
void draw_text(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int bg, const char * str, int lenghtToPrint);
void draw_text_shadow(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, const char * str);
void draw_text_center_shadow(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, const char * str);

void draw_number(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int number, int zeros);
void draw_number_center(BITMAP * buffer, FONT * mm_font, int x, int y, int fg, int number, int zeros);
