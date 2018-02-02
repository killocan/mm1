/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>

//NES RES: 256x240 MY RES: 512x480

const int PLAYERHEIGHT = 60;
const int PLAYERWIDTH  = 30;
const int GRAVITATION  = 1;
const int VELMOVING    = 4;
const int VELJUMP      = 14;

const int  TILE_SIZE = 32;
const int  UTIL_W    = 512;
const int  UTIL_H    = 480;
const int  TILES_X   = UTIL_W/TILE_SIZE;
const int  TILES_Y   = UTIL_H/TILE_SIZE;

const int STAIRCASEB   = 40;
const int STAIRCASE    = 41;
const int WAYPOINT     = 42;
const int MAP_XY_BEGIN = 43;
const int CANT_SCROLL  = 37;
const int FG_TILE_DRAW = 44;

typedef struct 
{
  unsigned char tile_number;
  unsigned char action;
  int xOffset;
  bool isForeground;
} MAP_INFO;
MAP_INFO **map;

BITMAP * tileset;

struct tiles_t
{
  std::vector<BITMAP*> tile_img;
  std::vector<int> x_pos;
};
tiles_t tiles;

struct camera_t
{
 int x, y;
 int old_x, old_y;
 int w, h;
};
camera_t camera;

struct stage_t
{
  int max_x, max_y;
  int default_tile;
  bool has_fg_tiles;
  MAP_INFO **map;
};
stage_t stage;

struct sector_t
{
  bool scroll_forbid;
};
sector_t * sectors;

enum {DIR_RIGHT, DIR_LEFT, DIR_UP, DIR_DOWN};

volatile int update_eq = 0;
void update_eq_tick()
  {
  update_eq = 1;
  }
END_OF_FUNCTION(update_eq_tick)

volatile int update_scroll = 0;
void update_scroll_tick()
  {
  update_scroll = 1;
  }
END_OF_FUNCTION(update_scroll_tick)

bool colmapxy(int x, int y)
{
  if (y < 0) return false;
  return stage.map[y][x].action == 1;
}

unsigned char mapAction(int x, int y)
{
  if (y < 0) return 0;
  return stage.map[y][x].action;  
}

class Player
{
  public:
    bool collision_ver(int x, int y, int &tilecoordx);
    bool collision_hor(int x, int y, int &tilecoordy, bool going_down, int &tiletype);
    bool collision_stair(int x, int y, int &tilecoordx, bool grabing = false);
    void calcScreenCoords(const camera_t & camera);

    int x, y;
    int sx, sy;
    int h, w;
    int velx, vely;
    
    bool isFacingRight,facedown;
    bool grabstair;
    bool overstair;
    bool lockjump;
};
Player player;

void Player::calcScreenCoords(const camera_t & camera)
{
  player.sx = player.x - camera.x;
  player.sy = player.y - camera.y;
}

bool Player::collision_hor(int x, int y, int &tilecoordy, bool going_down, int &tiletype)
{
  int tilexpixels = x-(x%TILE_SIZE);
  int testend = x + w;
  int stair = 0;

  tilecoordy = y/TILE_SIZE;

  int tilecoordx = tilexpixels/TILE_SIZE;
  while(tilexpixels <= testend)
  {
    if (going_down == false)
    {
      if(mapAction(tilecoordx, tilecoordy) == 1)
        return true;
    }
    else
    {
      tiletype = mapAction(tilecoordx, tilecoordy);
      if (this->overstair == true)
      {
        if((tiletype == 1) || (tiletype == STAIRCASEB) || (tiletype == STAIRCASE))
          return true;
      }
      else
      {
        if((tiletype == 1) || (tiletype == STAIRCASEB))
          return true;
      }
    }

    tilecoordx++;
    tilexpixels+=TILE_SIZE;
  }

  return false;
}

bool Player::collision_ver(int x, int y, int &tilecoordx)
{
  int tileypixels = y-(y%TILE_SIZE);
  int testend = y + h;

  tilecoordx = x/TILE_SIZE;

  int tilecoordy = tileypixels/TILE_SIZE;
  while(tileypixels <= testend)
  {
    if(colmapxy(tilecoordx, tilecoordy))
      return true;

    tilecoordy++;
    tileypixels += TILE_SIZE;
  }

  return false;
}

bool Player::collision_stair(int x, int y, int &tilecoordx, bool grabing)
{
  bool colliding_stair = false;

  int tileypixels = y-(y%TILE_SIZE);
  int testend = y + h;

  if (grabing == true && x%TILE_SIZE > 5) return false;

  tilecoordx = x/TILE_SIZE;

  int tilecoordy = tileypixels/TILE_SIZE;
  int mapact = 0;
  while(tileypixels <= testend)
  {
    mapact = mapAction(tilecoordx, tilecoordy);
    if(mapact == STAIRCASE || mapact == STAIRCASEB)
    {
      return true;
    }

    tilecoordy++;
    tileypixels += TILE_SIZE;
  }

  return false;
}

bool checkvoid_h(int camerax, bool facingright)
{
  int y = camera.y/TILE_SIZE;
  int x;

  if (facingright) 
  {
    x = (camerax+camera.w)/TILE_SIZE;
    if (x >= stage.max_x) return true;
  }
  else
  {
    x = (camerax-TILE_SIZE)/TILE_SIZE;
    if (x < 0) return true;
  }

  int ydesl = y / TILES_Y;
  int xdesl = x / TILES_X;
  int sector = ydesl*(stage.max_y/TILES_Y)+xdesl;

  bool action = sectors[sector].scroll_forbid;

  return action;
}

void draw_player(BITMAP * bmp, Player & player)
{
  rectfill(bmp, player.sx, player.sy, player.sx+player.w, player.sy+player.h, makecol(18,18,18));
}

int  map_load(FILE * fp)
{
  int i;
  bool waypoint_found = false;
  bool map_xy_begin_found = false;
  bool fg_tile_found = false;

  fread(&stage.max_x, sizeof(int), 1, fp);
  fread(&stage.max_y, sizeof(int), 1, fp);
  fread(&stage.default_tile, sizeof(unsigned char), 1, fp);
  stage.has_fg_tiles = false;

  int sectors_num = (stage.max_x/TILES_X) * (stage.max_y/TILES_Y);
  sectors = (sector_t*) malloc(sizeof(sector_t) * sectors_num);
  memset(sectors, 0, sizeof(sector_t) * sectors_num);

  stage.map = (MAP_INFO**) malloc(sizeof(MAP_INFO*) * stage.max_y);
  for(i = 0; i < stage.max_y; i++)
  {
    stage.map[i] = (MAP_INFO*) malloc(sizeof(MAP_INFO) * stage.max_x);
    fread(&(stage.map[i][0]), sizeof(MAP_INFO), stage.max_x, fp);
  }

  for (int y = 0; y < stage.max_y; ++y)
    for (int x = 0; x < stage.max_x; ++x)
    {
      if (waypoint_found == false && stage.map[y][x].action == WAYPOINT)
      {
        player.x = x*TILE_SIZE;
        player.y = y*TILE_SIZE;

        waypoint_found = true;
      }
      else if (map_xy_begin_found == false && stage.map[y][x].action == MAP_XY_BEGIN)
      {
        camera.x = x*TILE_SIZE;
        camera.y = y*TILE_SIZE;
        map_xy_begin_found = true;
      }
      else if (stage.map[y][x].action == CANT_SCROLL)
      {
        int ydesl = y / TILES_Y;
        int xdesl = x / TILES_X;
        int sector = ydesl*(stage.max_y/TILES_Y)+xdesl;

        sectors[sector].scroll_forbid = true;
      }
      else if (fg_tile_found == false && stage.map[y][x].isForeground == true)
      {
        stage.has_fg_tiles = true;
        fg_tile_found = true;
      }
    }

  player.h = PLAYERHEIGHT;
  player.w = PLAYERWIDTH;
  player.velx = 4;
  player.vely = 0;
  player.isFacingRight  = true;
  player.facedown   = false;
  player.lockjump   = false;
  player.grabstair  = false;
  player.overstair  = false;

  camera.w = UTIL_W;
  camera.h = UTIL_H;

  fclose(fp);

  return 0;
}

inline void tile_draw(BITMAP * bmp, int tile_number, int x, int y)
{
  draw_sprite(bmp, tiles.tile_img[tile_number], x, y);
}

// bmp dest, start x, start y, only bg tiles?, draw fg or bg?
void map_draw(BITMAP * bmp, const camera_t & camera, bool bg_only=true, bool bg=true)
{
  int i, j;
  int mapx, mapy;
  int map_xoff, map_yoff;

  mapx = camera.x / TILE_SIZE;
  mapy = camera.y / TILE_SIZE;

  map_xoff = camera.x & (TILE_SIZE-1);
  map_yoff = camera.y & (TILE_SIZE-1);

  int x,y;
  int xdisp = mapx+TILES_X==stage.max_x ? 0 : 1;
  int ydisp = mapy+TILES_Y==stage.max_y ? 0 : 1;

  if (bg_only == true)
  {
    for(i = 0; i < TILES_Y+ydisp; i++)
    {
      for(j = 0; j < TILES_X+xdisp; j++)
      {
        x = j * TILE_SIZE - map_xoff;
        y = i * TILE_SIZE - map_yoff;

        tile_draw(bmp, stage.map[mapy + i][mapx + j].tile_number, x, y);
      }
    }
  }
  else
  {
    for(i = 0; i < TILES_Y+ydisp; i++) 
    {
      for(j = 0; j < TILES_X+xdisp; j++)
      {
        if ((bg == false && stage.map[mapy + i][mapx + j].isForeground == false) || 
            (bg == true  && stage.map[mapy + i][mapx + j].isForeground == true))
          continue;

        x = j * TILE_SIZE - map_xoff;
        y = i * TILE_SIZE - map_yoff;

        //tile_draw(bmp, stage.map[mapy + i][mapx + j].tile_number, x, y);
        int tilenumber = stage.map[mapy + i][mapx + j].tile_number;
        masked_blit(tiles.tile_img[tilenumber], bmp, 0,0, x,y, TILE_SIZE,TILE_SIZE);
      }
    }
  }
}

void load_tiles(char * dir, int & tiles_num)
{
  char tile_name[80];
  sprintf(tile_name, "%s/tileset.bmp",dir);
  tileset = load_bitmap(tile_name, NULL);

  int w,h;
  w = tileset->w - TILE_SIZE;
  h = tileset->h - TILE_SIZE;
  for (int y = 2; y < h; y+=TILE_SIZE+2)
  {
    for (int x = 2; x < w; x+=TILE_SIZE+2)
    {
      tiles.tile_img.push_back(create_sub_bitmap(tileset, x, y, TILE_SIZE, TILE_SIZE));
      tiles.x_pos.push_back(0);
    }
  }

  tiles_num = tiles.tile_img.size();
}

bool horz_scroll = false;
int scroll_count = 0;
int dir = 0;
int camerax, cameray;
void do_camera()
{
  camerax = player.x - UTIL_W/2;
  if (camerax < 0)
  {
    camerax = 0;
  }
  else if (camerax + camera.w >= (stage.max_x*TILE_SIZE)) 
  {
    camerax = (stage.max_x*TILE_SIZE)-camera.w;
  }

  if (horz_scroll == false)
  {
    if (player.sy > UTIL_H)
    {
      horz_scroll = true;
      dir = 1;
    }
    else if (player.sy < 0 && camera.y != 0)
    {
      horz_scroll = true;
      dir = -1;
    }
  }
  else if (update_scroll)
  {
    update_scroll = 0;
    if (scroll_count < TILES_Y)
    {
      scroll_count++;
      camera.y += (dir*TILE_SIZE);
      player.y+=(dir*2);
    }
    else
    {
      horz_scroll = false;
      scroll_count = 0;
    }
  }

  if (camera.y < 0) camera.y = 0;

  if (cameray < 0)
  {
    cameray = 0;
  }
  else if (cameray + camera.h >= (stage.max_y*TILE_SIZE))
  {
    cameray = (stage.max_y*TILE_SIZE)-camera.h;
  }

  static bool reach_max_x = false;
  if (checkvoid_h(camerax, player.isFacingRight) == false)
  {
    reach_max_x = false;
    camera.x = camerax;
  }
  else
  {
    if (reach_max_x == false) 
    {
      camera.x = camerax;
      camera.x = camera.x - camera.x%TILE_SIZE;
      reach_max_x = true;
    }
  }

  player.calcScreenCoords(camera);
}

void do_observer_camera()
{
  camera.x = camera.x/TILE_SIZE*TILE_SIZE;
  camera.y = camera.y/TILE_SIZE*TILE_SIZE;

  if (key[KEY_RIGHT])
  { 
    if ((camera.x/TILE_SIZE) < (stage.max_x-TILES_X)) 
    {
      camera.x+=TILE_SIZE;
    }
  }
  else if (key[KEY_LEFT]) 
  {
    if (camera.x > 0) 
    { 
      camera.x-=TILE_SIZE;
    } 
  }

  if (key[KEY_UP])
  { 
    if (camera.y > 0)
    { 
      camera.y-=TILE_SIZE;
    } 
  }
  else if (key[KEY_DOWN])
  {
    if ((camera.y/TILE_SIZE) < (stage.max_y-TILES_Y))
    {
      camera.y+=TILE_SIZE;
    }
  }
}

int main(int argc, char *argv[])
{
  BITMAP * buffer;
  int tiles_num = 0;
  FILE *fp;
  int mode = 1;
  bool earthquake = false;
  int eq_count = 20;

  allegro_init();
  install_mouse();
  install_keyboard();
  install_timer();

  LOCK_VARIABLE(update_eq);
  LOCK_FUNCTION(update_eq_tick);
  install_int(update_eq_tick, 50);

  LOCK_VARIABLE(update_scroll);
  LOCK_FUNCTION(update_scroll_tick);
  install_int(update_scroll_tick, 90);

  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, UTIL_W, UTIL_H, 0, 0);

  buffer = create_bitmap(SCREEN_W, SCREEN_H);

  char tmp[80]={};
  if (argc > 1)
  {
    sprintf(tmp, "%s/stage.dat", argv[1]);
  }
  else
  {
    return -1;
  }

  if ((fp = fopen(tmp, "rb")) != NULL)
  {
    map_load(fp);
  }
  else
  {
    return -1;
  }

  load_tiles(argv[1], tiles_num);
  
  player.calcScreenCoords(camera);

  while(!key[KEY_ESC])
  {
    if (mode == 0)
    {
      do_observer_camera();
    }
    else
    {
      int tilecoord = 0;
      if (horz_scroll == false)
      {
        if (key[KEY_RIGHT] && player.grabstair == false)
        {
          player.isFacingRight = true;
          if(player.collision_ver(player.x+player.velx+player.w, player.y, tilecoord))
            player.x = tilecoord*TILE_SIZE -player.w-1;
          else
            player.x += player.velx;
        }
        else if (key[KEY_LEFT] && player.grabstair == false) 
        {
          player.isFacingRight = false;
          if(player.collision_ver(player.x-player.velx, player.y, tilecoord))
            player.x = (tilecoord+1)*TILE_SIZE +1;
          else
            player.x -= player.velx;
        }
      }

      if (player.collision_stair(player.x, player.y, tilecoord))
        player.overstair = true;
      else
        player.overstair = false;

      int tiletype;

      if (horz_scroll == false)
      {
        if (key[KEY_UP])
        {
          if(player.collision_stair(player.x, player.y, tilecoord, true))
          {
            player.grabstair = true;
            player.x = tilecoord*TILE_SIZE+1;
            player.y -= 2;
            player.vely = 1;
            player.facedown = false;
          }
          else if (player.grabstair == true)
          {
            //move to tile right above the stair.
            player.y -=2;
            player.grabstair = false;
          }
        }
        else if (key[KEY_DOWN])
        {
          // TODO: correct sliding when player is not RIGHT above the stair.
          // If player is colliding with something and it's a stair tile OR if player 
          // is on the last stair step tile(with nothing bellow) then keep holding on it.
          if((player.collision_hor(player.x, player.y+player.vely+player.h, tilecoord, true, tiletype) && 
            (tiletype == STAIRCASE || tiletype == STAIRCASEB) && ((player.x % TILE_SIZE) < (TILE_SIZE>>1))) || 
            (player.overstair == true && tiletype == 0))
          {
            player.facedown = true;
            //if (player.lockjump == false) // IF Press up or down, megaman grabs the stair
            player.grabstair = true;
            player.x = player.x/TILE_SIZE*TILE_SIZE+1;
            player.y += 2;
          }
          else if (player.grabstair == true)
          {
            player.grabstair = false;
          }
        }

        if (key[KEY_SPACE])
        {
          if (player.grabstair == true)
          {
            player.grabstair = false;
          }
          else if (player.lockjump == false)
          {
            player.vely = -VELJUMP;
            player.lockjump = true; 
          }
        }
      }

      if (player.grabstair == false && horz_scroll == false)
      {
        if(player.vely < 0)
        {
          if(player.collision_hor(player.x, player.y+player.vely, tilecoord, false, tiletype))
          {
            player.y    = (tilecoord+1)*TILE_SIZE + 1;
            player.vely = 0;
          }
          else
          {
            player.facedown = false;
            player.y    += player.vely;
            player.vely += GRAVITATION;
          }
        }
        else
        {
          if(player.collision_hor(player.x, player.y+player.vely+player.h, tilecoord, !player.overstair, tiletype))
          {
            player.y = tilecoord*TILE_SIZE - player.h-1;
            // 1 so we test against the ground again 
            // in the next frame (0 would test against the ground in the next+1 frame)
            player.vely = 1; 

            if(!key[KEY_SPACE])
              player.lockjump = false;
          }
          else
          {
            player.facedown = true;
            player.y    += player.vely;
            player.vely += GRAVITATION;

            if(player.vely >= TILE_SIZE)
              player.vely = TILE_SIZE;

            player.lockjump = true;
          }
        }
      }
      
      do_camera();
    }

    if (key[KEY_ENTER])
    {
      if (mode)
      {
        camera.old_x = camera.x;
        camera.old_y = camera.y;
      }
      else
      {
        camera.x = camera.old_x;
        camera.y = camera.old_y;
      }

      mode = !mode;
      while(key[KEY_ENTER]);
    }

    if (key[KEY_A] && earthquake == false)
    {
      eq_count = 20;
      earthquake = true;
    }

    if (stage.has_fg_tiles == false)
    {
      map_draw(buffer, camera);
      if (mode == 1) draw_player(buffer, player);
    }
    else
    {
      map_draw(buffer, camera, false, true);
      if (mode == 1) draw_player(buffer, player);
      map_draw(buffer, camera, false, false);
    }

    if (earthquake == false)
    {
      vsync();
      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    else
    {
      static int eq_x=-4, eq_y=4;
      if (update_eq == 1)
      {
        static int quake_values[4] = {-5,-3,3,5};
        // Gutsman earth quake
        int old_eq = eq_x;
        while (eq_x == old_eq) eq_x = quake_values[rand()%4];
        old_eq = eq_y;
        while (eq_y == old_eq) eq_y = quake_values[rand()%4];

        --eq_count;
        if (eq_count <= 0) earthquake = false;
        update_eq = 0;
      }
      vsync();
      clear_bitmap(screen);
      blit(buffer, screen, 0, 0, eq_x<<1, eq_y<<1, SCREEN_W, SCREEN_H); 
    }
  }

  return 0;
}
END_OF_MAIN();
