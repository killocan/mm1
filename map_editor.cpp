/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

// F - Não mostra as informações sobre o tile (Numero da ação associada)
// G - Não mostra o grid
// Space - Não mostra os tiles, somente as ações
// O - Mostra offsetX dentro do tile atual (seta offset ao colocar ação)
// WSAD - Move uma tela por vez
// Left/Right/Up/Down - Move um tile por vez
// I - Mostra a ação no mapa para referencia (não considera o offset, usar tecla 'O' junto)
// Left SHIFT - Salva
// B - Liga flag de foreground.
// M - Mostra as ações sobre o mapa.
// CTRL+C + (MOUSE) - Seta pontos de inicio e fim para copia
// CTRL+V + (MOUSE) - Efetua copia partindo do ponto atual.
// CTRL+X - Desfaz seleção.

#include <allegro.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>

//NES RES: 256x240 MY RES: 512x480

const int  TILE_SIZE = 32;
const int  UTIL_W    = 512;
const int  UTIL_H    = 480;
const int  UTIL_H_EX = 420;
const int  TS_X_TLS  = 15;
const int  TILES_X   = UTIL_W/TILE_SIZE;
const int  TILES_Y   = UTIL_H/TILE_SIZE;

typedef struct 
{
  unsigned char tile_number;
  unsigned char action;
  int xOffset;
  bool isForeground;
} MAP_INFO;

MAP_INFO **map;

int x_desloc = 0;
int cur_tile = 0;
int cur_act  = 0;

int map_drawx, map_drawy;
int max_x, max_y;
unsigned char default_tile;

BITMAP * tileset;

struct point_t
{
  int x,y;
};

struct tiles_t
{
  std::vector<BITMAP*> tile_img;
  std::vector<point_t> xy_pos;
};
tiles_t tiles;

struct actions_t
{
BITMAP ** tile_img;
point_t * coords;
};
actions_t actions;

// Selection Variables
int cur_copy_point = 0;
point_t pt_ini_point;
point_t pt_end_point;
bool draw_selection = false;
bool take_shot = false;
MAP_INFO * selection_data = NULL;
BITMAP * selection_preview;

void map_save(char * filename)
{
  FILE * fp = fopen(filename, "wb+");

  fwrite(&max_x, sizeof(int), 1, fp);
  fwrite(&max_y, sizeof(int), 1, fp);
  fwrite(&default_tile, sizeof(unsigned char), 1, fp);

  for (int i = 0; i < max_y; i++)
  {
    fwrite(&map[i][0], sizeof(MAP_INFO), max_x, fp);
  }

  fclose(fp);
}

int map_load(FILE * fp)
{
  int i;

  fread(&max_x, sizeof(int), 1, fp);
  fread(&max_y, sizeof(int), 1, fp);
  fread(&default_tile, sizeof(unsigned char), 1, fp);

  map = (MAP_INFO**) malloc(sizeof(MAP_INFO*) * max_y);
  for(int i = 0; i < max_y; i++)
  {
    map[i] = (MAP_INFO*) malloc(sizeof(MAP_INFO) * max_x);
    fread(&map[i][0], sizeof(MAP_INFO), max_x, fp);
  }

  fclose(fp);

  return 0;
}

void tile_draw(BITMAP * bmp, int tile_number, int x, int y)
{
  draw_sprite(bmp, tiles.tile_img[tile_number], x, y);
}

void map_draw(BITMAP * bmp, int map_drawx, int map_drawy, bool draw_actions, bool ignoreVoid)
{
  int i, j;
  int mapx, mapy;
  int map_xoff, map_yoff;
  int color[] = {makecol(30,30,30), makecol(255,0,0), makecol(255,255,255)};

  mapx = map_drawx / TILE_SIZE;
  mapy = map_drawy / TILE_SIZE;

  map_xoff = map_drawx & (TILE_SIZE-1);
  map_yoff = map_drawy & (TILE_SIZE-1);

  int x,y;
  int xdisp = mapx+TILES_X==max_x ? 0 : 1;
  int ydisp = mapy+TILES_Y==max_y ? 0 : 1;
  for(i = 0; i < TILES_Y+ydisp; i++) 
  {
    for(j = 0; j < TILES_X+xdisp; j++)
    {
      x = j * TILE_SIZE - map_xoff;
      y = i * TILE_SIZE - map_yoff;

      tile_draw(bmp, map[mapy + i][mapx + j].tile_number, x, y);
      if (ignoreVoid == true)
      {
        if (map[mapy + i][mapx + j].action != 0)
        {
          x+=map[mapy + i][mapx + j].xOffset;
          draw_sprite(bmp, actions.tile_img[map[mapy + i][mapx + j].action], x, y);
        }
      }
      else if (draw_actions == true)
      {
        draw_sprite(bmp, actions.tile_img[map[mapy + i][mapx + j].action], x, y);
      }

      if (key[KEY_F] == 0 && take_shot == false)
      {
        switch (map[mapy + i][mapx + j].action)
        {
          case 0:
            textprintf_ex(bmp, font, x, y, color[2], color[0], "%02d", map[mapy + i][mapx + j].action);
          break;
          case 1:
            textprintf_ex(bmp, font, x, y, color[2], color[1], "%02d", map[mapy + i][mapx + j].action);
          break;
          default:
            textprintf_ex(bmp, font, x, y, 0, color[2], "%02d", map[mapy + i][mapx + j].action);
          break;
        }

        if (map[mapy + i][mapx + j].isForeground)
        {
          textprintf_ex(bmp, font, x+24, y, color[2], color[1], "F");
        }
      }
    }
  }
}

void draw_tilesbar(BITMAP * bmp, int tiles_num, int y)
{
  rectfill(bmp, 0,y,512,y+UTIL_H_EX,makecol(30,40,100));
  
  int x = 8;
  for (int i = 0, j = 0; i < tiles_num; i++)
  {
    tiles.xy_pos[i].x = x+1;
    tiles.xy_pos[i].y = y+8;
    draw_sprite(bmp, tiles.tile_img[i], tiles.xy_pos[i].x, tiles.xy_pos[i].y);
    x += 33;

    j++;
    if (j == TS_X_TLS)
    {
      j = 0;
      y+=33;
      x =8;
    }
  }

  x = tiles.xy_pos[cur_tile].x;
  y = tiles.xy_pos[cur_tile].y;

  rect(bmp, x,  y,  x+32,y+32, makecol(255,10,10));
  rect(bmp, x-1,y-1,x+33,y+33, makecol(255,255,255));
}

void draw_actionsbar(BITMAP *bmp, int act_num, int x)
{
  rectfill(bmp, x, 0, x+200, SCREEN_H, makecol(255,255,255));  
  rectfill(bmp, x+4, 4, x+196, SCREEN_H-4, makecol(0,50,50));

  for (int i = 0; i < act_num; i++)
  {
    draw_sprite(bmp, actions.tile_img[i], x + actions.coords[i].x, actions.coords[i].y);
    if (cur_act == i) 
    {
      rect(bmp, (x + actions.coords[i].x)-2, 
                actions.coords[i].y-2, 
                (x + actions.coords[i].x)+33,
                actions.coords[i].y+33,
                makecol(255,50,50));
    }
  }

  if (mouse_x < UTIL_W && mouse_y < UTIL_H)
  {
    int mx = (mouse_x+map_drawx)/TILE_SIZE;
    int my = (mouse_y+map_drawy)/TILE_SIZE;
    char tmp[80];
    int bg,fg;
    bg = makecol(255,255,255);
    fg = makecol(0,50,200);
    sprintf(tmp, "mouse_x: [%02d]", mouse_x);
    textprintf_ex(bmp, font, x+20, SCREEN_H-228, fg, bg, "%-20s", tmp);
    sprintf(tmp, "mouse_y: [%02d]", mouse_y);
    textprintf_ex(bmp, font, x+20, SCREEN_H-220, fg, bg, "%-20s", tmp);
    sprintf(tmp, "mapx: [%02d]", mx);
    textprintf_ex(bmp, font, x+20, SCREEN_H-212, fg, bg, "%-20s", tmp);
    sprintf(tmp, "mapy: [%02d]", my);
    textprintf_ex(bmp, font, x+20, SCREEN_H-204, fg, bg, "%-20s", tmp);
  }

  if (key[KEY_O])
    textprintf_ex(bmp, font, x+20, SCREEN_H-50, makecol(0,50,200),makecol(255,255,255), "OFFSET: %02d", mouse_x%TILE_SIZE);

  if (draw_selection)
    textprintf_ex(bmp, font, x+8, SCREEN_H-80, makecol(255,0,0),0, "SELECTION ON-SCROLL OFF");
}
 
void handle_tilebar(int tiles_num)
{
  if (mouse_b & 1)
  {
    if (mouse_y >= 480)
    {
      int x = mouse_x;
      int y = mouse_y;
      for (int i = 0; i < tiles_num; i++)
      {
        if ((x >= tiles.xy_pos[i].x)    &&
            (x <= tiles.xy_pos[i].x+33) &&
            (y >= tiles.xy_pos[i].y)    &&
            (y <= tiles.xy_pos[i].y+33))
            { cur_tile = i; break; }
      }
    }
  }
}

void handle_actbar(int act_num)
{
  if (mouse_b & 2)
  {
    if (mouse_x >= 512)
    {
      int x = mouse_x;
      int y = mouse_y;
      for (int i = 0; i < act_num; i++)
      {
        if (x >= actions.coords[i].x+512 && 
            x <= actions.coords[i].x+512+40 && 
            y >= actions.coords[i].y && 
            y <= actions.coords[i].y+40) 
           {
             cur_act = i; 
             break; 
           }
      }
    }
  }
}

void handle_click(int x, int y, int button)
{
  x = x/TILE_SIZE;
  y = y/TILE_SIZE;
  
  x += (map_drawx>>5);
  y += (map_drawy>>5);
  
  switch(button)
  {
    case 1:
    {
      if (!key[KEY_LCONTROL])
      {
        map[y][x].tile_number = cur_tile;
      }
      else
      {
        if (key[KEY_C])
        {
          if (cur_copy_point == 0)
          {
            if (selection_data != NULL)
              free(selection_data);

            clear_bitmap(selection_preview);
            selection_data = NULL;

            pt_ini_point.x = x;
            pt_ini_point.y = y;
            cur_copy_point = 1;
            draw_selection = false;
          }
          else
          {
            pt_end_point.x = x;
            pt_end_point.y = y;
            if (pt_end_point.x < pt_ini_point.x || pt_end_point.y < pt_ini_point.y)
            {
              point_t temp = pt_ini_point;
              pt_ini_point = pt_end_point;
              pt_end_point = temp;
            }
            take_shot = true;
            draw_selection = true;
          }
          while(key[KEY_C]);
        }
        else if (key[KEY_V])
        {
          point_t pt_data_len;
          pt_data_len.x = (pt_end_point.x - pt_ini_point.x) + 1;
          pt_data_len.y = (pt_end_point.y - pt_ini_point.y) + 1;

          if (selection_data == NULL)
          {
            selection_data = (MAP_INFO*) malloc(sizeof(MAP_INFO) * (pt_data_len.x*pt_data_len.y));

            for (int ys = 0; ys < pt_data_len.y; ++ys)
            {
              for (int xs = 0; xs < pt_data_len.x; ++xs)
              {
                selection_data[ys*(pt_data_len.x)+xs] = map[pt_ini_point.y+ys][pt_ini_point.x+xs];
              }
            }
          }

          for (int ys = 0; ys < pt_data_len.y; ++ys)
            {
              for (int xs = 0; xs < pt_data_len.x; ++xs)
              {
                map[y+ys][x+xs] = selection_data[ys*(pt_data_len.x)+xs];
              }
            }

          draw_selection = false;
          cur_copy_point = 0;

          while(key[KEY_V]);
        }
      }
    }
    break;
    case 2:
    {
      map[y][x].action = cur_act;
      if (key[KEY_O])
      {
        int offset = mouse_x%TILE_SIZE;
        map[y][x].xOffset = offset;
      }
      else
      {
        map[y][x].xOffset = 0;
      }

      if (key[KEY_B])
      {
        map[y][x].isForeground = true;
      }
      else
      {
        map[y][x].isForeground = false;
      }        
    }
    break;
  }
}

void load_actions(char * path, int num_actions)
{
  actions.tile_img = (BITMAP **) malloc(sizeof(BITMAP*)*num_actions);
  actions.coords = new point_t[num_actions];
  int x=20,y=10;

  char img_name[80];
  for (int i = 0; i < num_actions; i++)
  {
    sprintf(img_name, "%s/%02d.bmp",path,i);

    actions.tile_img[i] = load_bitmap(img_name, NULL);
    actions.coords[i].x = x;
    actions.coords[i].y = y;

    x+=40;
    if (x >= 180) { x=20; y+=40; }
  }
}

void load_tiles(char * dir, int & tiles_num)
{
  char tile_name[80];
  sprintf(tile_name, "%s/tileset.bmp",dir);
  tileset = load_bitmap(tile_name, NULL);

  int w,h;

  w = tileset->w;
  h = tileset->h;

  for (int y = 2; y < (h-TILE_SIZE); y+=34)
  {
    for (int x = 2; x < (w-TILE_SIZE); x+=34)
    {
      tiles.tile_img.push_back(create_sub_bitmap(tileset, x, y, TILE_SIZE, TILE_SIZE));
      point_t xy = {0,0};
      tiles.xy_pos.push_back(xy);
    }
  }

  tiles_num = tiles.tile_img.size();
}

void draw_grid(BITMAP * bmp)
{
  int color = makecol(255,255,255);
  for (int i = 0; i < SCREEN_W; i+=TILE_SIZE)
    vline(bmp, i, 0, SCREEN_H, color);
    
  for (int i = 0; i < SCREEN_H; i+=TILE_SIZE)
    hline(bmp, 0, i, SCREEN_W, color);
}

int main(int argc, char *argv[])
{
  BITMAP * buffer;
  int tiles_num = 0;
  int act_num = 45;
  FILE *fp;

  map_drawx=map_drawy=0;

  allegro_init();
  install_mouse();
  install_keyboard();
  install_timer();

  set_color_depth(32);  
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, UTIL_W+200, UTIL_H+UTIL_H_EX, 0, 0);

  buffer            = create_bitmap(SCREEN_W, SCREEN_H);
  selection_preview = create_bitmap(185, 150);

  clear_bitmap(selection_preview);
 
  char filename[255]={};
  char dirpath[255] = {};
  if (argc > 1)
  {
    sprintf(filename, "%s/stage.dat", argv[1]);
    sprintf(dirpath, "%s", argv[1]);
  }
  else
  {
    if (file_select_ex("Load megaman map", dirpath, "d", 80, 640, 480) == 0)
    {
      exit(-1);
    }

    sprintf(filename, "%s/stage.dat", dirpath);
  }

  if ((fp = fopen(filename, "rb")) != NULL)
  {
    map_load(fp);
  }
  else
  {
    if (argc < 5)
    {
      exit(-1);
    }

    max_x = atoi(argv[2]) * TILES_X;
    max_y = atoi(argv[3]) * TILES_Y;
    if (max_x == 0 || max_y == 0)
    {
      exit(-1);
    }
    default_tile = atoi(argv[4]);

    map = (MAP_INFO**) malloc(sizeof(MAP_INFO*) * max_y);
    for(int i = 0; i < max_y; i++)
      map[i] = (MAP_INFO*) malloc(sizeof(MAP_INFO) * max_x);

    for(int i = 0; i < max_y; i++)
      for (int j = 0; j < max_x; j++)
      {
        map[i][j].tile_number = default_tile;
        map[i][j].action = 0;
        map[i][j].xOffset = 0;
        map[i][j].isForeground = false;
      }
  }

  load_actions((char*)"./actions", act_num);
  load_tiles(dirpath, tiles_num);
  
  int stepx = TILE_SIZE*TILES_X;
  int stepy = TILE_SIZE*TILES_Y;
  bool draw_actions = false;
  bool ignoreVoid   = false;
  while(!key[KEY_ESC])
  {
    handle_tilebar(tiles_num);
    handle_actbar(act_num);

    if (mouse_x < UTIL_W && mouse_y < UTIL_H)
    {
      if (mouse_b&1) handle_click(mouse_x, mouse_y, 1);
      if (mouse_b&2) handle_click(mouse_x, mouse_y, 2);
    }
    
    if (key[KEY_RIGHT] && draw_selection == false)
    { 
      if ((map_drawx/TILE_SIZE) < (max_x-TILES_X)) 
      {
        map_drawx+=TILE_SIZE;
      }
    }
    else if (key[KEY_LEFT] && draw_selection == false) 
    {
      if (map_drawx > 0) 
      { 
        map_drawx-=TILE_SIZE;
      } 
    }

    if (key[KEY_UP] && draw_selection == false)
    { 
      if (map_drawy > 0)
      { 
        map_drawy-=TILE_SIZE;
      } 
    }
    else if (key[KEY_DOWN] && draw_selection == false)
    {
      if ((map_drawy/TILE_SIZE) < (max_y-TILES_Y))
      {
        map_drawy+=TILE_SIZE;
      }
    }

    if (key[KEY_D] && draw_selection == false)
    {
      map_drawx+=stepx;
      if (map_drawx/TILE_SIZE >= (max_x-TILES_X))
        map_drawx = (max_x-TILES_X)*TILE_SIZE;

      while(key[KEY_D]);
    }
    else if (key[KEY_A] && draw_selection == false)
    {
      map_drawx-=stepx;
      if (map_drawx < 0)
        map_drawx=0;

      while(key[KEY_A]);
    }

    if (key[KEY_W] && draw_selection == false)
    { 
      map_drawy-=stepy;
      if (map_drawy < 0)
        map_drawy=0;

      while(key[KEY_W]);
    }
    else if (key[KEY_S] && draw_selection == false)
    {
      map_drawy+=stepy;
      if (map_drawy/TILE_SIZE >= (max_y-TILES_Y))
        map_drawy = (max_y-TILES_Y)*TILE_SIZE;

      while(key[KEY_S]);
    }

    if (key[KEY_LSHIFT]) 
    {
      rectfill(screen, SCREEN_W / 2 - 200, SCREEN_H / 2 - 50, SCREEN_W / 2 + 200, SCREEN_H / 2 + 50, makecol(255, 255, 255));
      textout_centre_ex(screen, font, "SAVING...", SCREEN_W / 2, SCREEN_H / 2, 0, -1);
      map_save(filename);
      while(key[KEY_LSHIFT]);
    }

    if (key[KEY_SPACE])
    {
      draw_actions = true;
    }
    else draw_actions = false;

    if (key[KEY_M])
      ignoreVoid = true;
    else
      ignoreVoid = false;

    if (key[KEY_X])
    {
      cur_copy_point = 0;
      draw_selection = false;
    }

    map_draw(buffer, map_drawx, map_drawy, draw_actions, ignoreVoid);

    if (!key[KEY_G]) 
    {
      if (take_shot == false)
        draw_grid(buffer);
    }
    draw_tilesbar(buffer, tiles_num, UTIL_H);
    draw_actionsbar(buffer, act_num, UTIL_W);

    if (key[KEY_I])
    {
      int xpos, ypos;
      xpos = mouse_x / TILE_SIZE * TILE_SIZE;
      if (key[KEY_O]) xpos += mouse_x%TILE_SIZE;
      ypos = mouse_y / TILE_SIZE * TILE_SIZE;
      draw_sprite(buffer,actions.tile_img[cur_act], xpos, ypos);
    }
    else if (take_shot == false)
    {
      circlefill(buffer, mouse_x, mouse_y, 5, 0);
      circlefill(buffer, mouse_x, mouse_y, 3,  makecol(255,255,255));
    }

    if (draw_selection == true)
    {
      int xini,yini,xend,yend;

      xini = (pt_ini_point.x<<5) - map_drawx;
      yini = (pt_ini_point.y<<5) - map_drawy;
      xend = ((pt_end_point.x<<5)+TILE_SIZE) - map_drawx;
      yend = ((pt_end_point.y<<5)+TILE_SIZE) - map_drawy;

      if (take_shot == true)
      {
        stretch_blit(buffer, selection_preview, xini, yini, xend-xini,yend-yini, 
                                                0,0, selection_preview->w, selection_preview->h);
        take_shot = false;
      }
      rect(buffer, SCREEN_W-194, SCREEN_H-392, 
                   SCREEN_W-194+selection_preview->w+3, SCREEN_H-392+selection_preview->h+3, 
                   makecol(255,255,255));

      blit (selection_preview, buffer, 0,0, SCREEN_W-192, SCREEN_H-390, selection_preview->w, selection_preview->h);

      set_trans_blender(128, 128, 128, 128);
      drawing_mode(DRAW_MODE_TRANS,NULL, 0,0);
      rectfill(buffer, xini, yini, xend, yend, makecol(30,30,255));
      drawing_mode(DRAW_MODE_SOLID,NULL, 0,0);
      rect(buffer, xini, yini, xend, yend, 0);
      rect(buffer, xini+1, yini+1, xend-1, yend-1, 0);
    }

    vsync();
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_bitmap(buffer);
  }

  return 0;
}
END_OF_MAIN();
