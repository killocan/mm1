#include <cstdio>
#include <allegro.h>

BITMAP * makegrid()
{
  BITMAP * grid = create_bitmap(SCREEN_W, SCREEN_H);

  clear_to_color(grid, MASK_COLOR_32);

  for (int i = 32; i < SCREEN_W; i+=32)
  {
    vline(grid, i-1, 0, SCREEN_H, makecol(255,0,0));
  }
  for (int i = 16; i < SCREEN_H; i+=32)
  {
    hline(grid, 0, i-1, SCREEN_W, makecol(255,0,0));
  }

  return grid;
}

int main(int argc, char ** argv)
{
  BITMAP * buffer, * img, * grid;
  int x, y;
  int step = 16;

  allegro_init();
  install_timer();
  install_keyboard();

  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512,480, 0,0);

  buffer = create_bitmap(SCREEN_W, SCREEN_H);
  img    = load_bitmap(argv[1], NULL);
  fprintf(stderr,"Trying to load[%s] - Result [%p]\n", argv[1], img);
  grid   = makegrid();

  x = y = 0;

  while(!key[KEY_ESC])
  {
    if (key[KEY_A])
    {
      if (step == 16) step = 32;
      else if (step == 32) step = 16;

      while(key[KEY_A]);
    }

    if (key[KEY_RIGHT])
    {
      x+=32;
      rest(150);
    }
    else if (key[KEY_LEFT])
    {
      x-=32;
      rest(150);
    }

    if (key[KEY_UP])
    {
      y-=step;
      rest(150);
    }
    else if (key[KEY_DOWN])
    {
      y+=step;
      rest(150);
    }

    if (key[KEY_W])
    {
      y-=step;
      while(key[KEY_W]);
    }
    else if (key[KEY_S])
    {
      y+=step;
      while(key[KEY_S]);
    }

    blit(img, buffer, x,y,0,0, SCREEN_W, SCREEN_H);
    masked_blit(grid, buffer, 0,0,0,0, SCREEN_W, SCREEN_H);

    vsync();
    blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
    clear_to_color(buffer,makecol(0,255,200));
  }

  return 0;
} END_OF_MAIN()
