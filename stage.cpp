/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <cstdio>

#include "stage.h"

#include "tileactions.h"
#include "spritefiles.h"
#include "defines.h"
#include "globalgamestate.h"
#include "globals.h"

#include "blader.h"
#include "beak.h"
#include "supercutter.h"
#include "flyingshell.h"
#include "killerbullet.h"
#include "spine.h"
#include "met.h"
#include "picketman.h"
#include "sniperjoe.h"
#include "peng.h"
#include "bigeye.h"
#include "flea.h"
#include "octopusbattery.h"
#include "screwbomber.h"
#include "bombomb.h"
#include "crazyrazy.h"
#include "footholder.h"
#include "tacklefire.h"
#include "moveplatform.h"
#include "lightningwall.h"
#include "sparkle.h"
#include "firepillar.h"
#include "firebolt.h"
#include "lifechargerbig.h"
#include "lifechargerlittle.h"
#include "weaponchargerbig.h"
#include "weaponchargerlittle.h"
#include "gutsmanrock.h"
#include "newlifeitem.h"
#include "bonuspoint.h"
#include "bossdoor.h"
#include "sparkleenm.h"
#include "watcher.h"
#include "stageorb.h"
#include "hitexplosion.h"
#include "explosionlittle.h"
#include "megamanexplosion.h"
#include "magneticbeam.h"

#include "gutsman.h"
#include "fireman.h"

#include "magneticbeamhandler.h"
#include "movingflame.h"
#include "disappearingblock.h"
#include "gutsmangun.h"

static volatile int update_scroll = 0;
static void screenscroll_timer()
{
  update_scroll = 1;
}
END_OF_FUNCTION(screenscroll_timer);

Stage::Stage(std::string stage_path, Camera & camera, Player ** player)
{
  stageNumber  = 0;
  cur_waypoint = 0;
  horz_scroll  = false;
  has_fg_tiles = false;

  hasMovingPlatform = false;

  reach_max_x = false;

  scroll_count = 0;

  load(stage_path, camera, player);

  LOCK_VARIABLE(update_scroll);
  LOCK_FUNCTION(screenscroll_timer);
  install_int(screenscroll_timer, 40);

  MagneticBeamHandler::instance()->setup(this);

  finished = false;
}

Stage::~Stage()
{
  remove_int(screenscroll_timer);

  unload();
}

unsigned char Stage::tileAction(int x, int y) const
{
  if (x < 0 || y < 0) return mm_tile_actions::TILE_VOID;
  else if (x >= max_x || y >= max_y) return mm_tile_actions::TILE_VOID;

  return map[y][x].action;
}

unsigned char Stage::tileNumber(int x, int y) const
{
  if (x < 0 || y < 0) return mm_tile_actions::TILE_VOID;

  return map[y][x].tile_number;
}

unsigned char Stage::tileActionUnnormalized(int x, int y) const
{
  x /= mm_graphs_defs::TILE_SIZE;
  y /= mm_graphs_defs::TILE_SIZE;
  return map[y][x].action;
}

unsigned char Stage::tileNumberUnnormalized(int x, int y) const
{
  x /= mm_graphs_defs::TILE_SIZE;
  y /= mm_graphs_defs::TILE_SIZE;
  return map[y][x].tile_number;
}

void Stage::setTileAction(int x, int y, unsigned char action)
{
  map[y][x].action = action;
}

void Stage::setTileNumber(int x, int y, unsigned char tile_number)
{
  map[y][x].tile_number = tile_number;
}

int Stage::unload()
{
  std::vector<BITMAP*>::iterator i;
  for (i = tiles.tile_img.begin(); i != tiles.tile_img.end(); ++i)
  {
    destroy_bitmap(*i);
  }
  tiles.tile_img.clear();

  destroy_bitmap(tileset);
  tileset = NULL;

  free(sectors);
  sectors = NULL;

  for(int i = 0; i < max_y; ++i)
  {
    free(map[i]);
    map[i] = NULL;
  }

  free(map);
  map = NULL;

  std::map<int, AnimSequence *>::iterator it;
  for (it = preLoadedSprites.begin(); it != preLoadedSprites.end(); ++it)
  {
    delete it->second;
  }

  return 0;
}

void Stage::setWaypoint(Player * player)
{
  player->x = waypoints[cur_waypoint].x;
  player->y = waypoints[cur_waypoint].y;
  scroll_count = 0;
}

void Stage::defineCameraSector(int x, int y, bool state)
{
  int ydesl = y / mm_graphs_defs::TILES_Y;
  int xdesl = x / mm_graphs_defs::TILES_X;
  int sector = ydesl*(max_x/mm_graphs_defs::TILES_X)+xdesl;

  sectors[sector].scroll_forbid = state;
}

#define TEST(r, c) if (r != (size_t)c){fprintf(stderr,"READ ERROR! Stage file corrupted. Line[%d]\n", __LINE__);}
int Stage::load(const std::string & stage_path, Camera & camera, Player ** player)
{
  FILE *fp;

  std::string stage_file = stage_path + "/stage.dat";
  if ((fp = fopen(stage_file.c_str(), "rb")) == NULL)
  {
    return -1;
  }

  size_t result = fread(&max_x, sizeof(int), 1, fp);
  TEST(result, 1);
  result = fread(&max_y, sizeof(int), 1, fp);
  TEST(result, 1);
  result = fread(&default_tile, sizeof(unsigned char), 1, fp);
  TEST(result, 1);

  int sectors_num = (max_x/mm_graphs_defs::TILES_X) * (max_y/mm_graphs_defs::TILES_Y);
  sectors = (sector_t*) malloc(sizeof(sector_t) * sectors_num);
  memset(sectors, 0, sizeof(sector_t) * sectors_num);

  map = (MAP_INFO**) malloc(sizeof(MAP_INFO*) * max_y);
  for(int i = 0; i < max_y; i++)
  {
    map[i] = (MAP_INFO*) malloc(sizeof(MAP_INFO) * max_x);
    result = fread(&(map[i][0]), sizeof(MAP_INFO), max_x, fp);
    TEST(result, max_x);
  }

  result = 0;

  fclose(fp);

  for (int y = 0; y < max_y; ++y)
  {
    for (int x = 0; x < max_x; ++x)
    {
      if (map[y][x].action == mm_tile_actions::TILE_MEGAMAN_WAYPOINT)
      {
        int waypointX = x*mm_graphs_defs::TILE_SIZE+map[y][x].xOffset;
        waypoint_t waypoint = {waypointX, y*mm_graphs_defs::TILE_SIZE};
        waypoints.push_back(waypoint);
      }
      else if (map[y][x].action == mm_tile_actions::TILE_MAP_BEGIN)
      {
        camera.x = x*mm_graphs_defs::TILE_SIZE;
        camera.y = y*mm_graphs_defs::TILE_SIZE;
#ifdef DEBUG
        fprintf(stderr,"INITIAL CAMERA Y = [%d] X [%d]\n", camera.y, camera.x);
#endif
      }
      else if (map[y][x].action == mm_tile_actions::TILE_SCROLL_LIMIT)
      {
        int ydesl = y / mm_graphs_defs::TILES_Y;
        int xdesl = x / mm_graphs_defs::TILES_X;
        int sector = ydesl*(max_x/mm_graphs_defs::TILES_X)+xdesl;

        sectors[sector].scroll_forbid = true;
      }
      else if (map[y][x].action == mm_tile_actions::TILE_BOSS)
      {
        int ydesl = y / mm_graphs_defs::TILES_Y;
        int xdesl = x / mm_graphs_defs::TILES_X;
        int sector = ydesl*(max_x/mm_graphs_defs::TILES_X)+xdesl;

        sectors[sector].has_boss = true;
      }
      else if (map[y][x].isForeground == true)
      {
        int ydesl = y / mm_graphs_defs::TILES_Y;
        int xdesl = x / mm_graphs_defs::TILES_X;
        int sector = ydesl*(max_x/mm_graphs_defs::TILES_X)+xdesl;

        sectors[sector].has_fg_tiles = true;
        this->has_fg_tiles = true;
      }
      else
      {
        unsigned char tile_action = map[y][x].action;
        if ((tile_action > mm_tile_actions::TILE_DEATH) && (tile_action < mm_tile_actions::TILE_TIMER))
        {
          if (preLoadedSprites.find(tile_action) == preLoadedSprites.end())
          {
#ifdef DEBUG
            fprintf(stderr,"Preload: [%s]\n", mm_spritefiles::sprite_files[tile_action]);
#endif
            preLoadedSprites.insert(std::pair<int, AnimSequence *>
              (tile_action, new AnimSequence(mm_spritefiles::sprite_files[tile_action])));
          }
        }
      }
    }
  }

#ifdef DEBUG
  fprintf(stderr,"Preload: [megaman]\n");
#endif
  // Megaman, keep a copy for weapon color change.
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::MEGAMAN_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::MEGAMAN_SPRITES], true)));

#ifdef DEBUG
  fprintf(stderr,"Preload: [weapons]\n");
#endif

  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_SPRITES])));
  //TODO: load bassed in megaman curr avaliable weapons *for (w in weapons) if (mm.has(w)) add(sprites(w))
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_ICEMAN, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_ICEMAN])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_BOMBMAN, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_BOMBMAN])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_ELECMAN, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_ELECMAN])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_CUTMAN, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_CUTMAN])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_FIREMAN, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_FIREMAN])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::WEAPONS_MAGNETIC, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::WEAPONS_MAGNETIC])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::GUTSMAN_ROCK_FRAGMENTS_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::GUTSMAN_ROCK_FRAGMENTS_SPRITES])));
  
#ifdef DEBUG
  fprintf(stderr,"Preload: [explosion]\n");
#endif

  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::HITEXPLOSION_SPRITES,     new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::HITEXPLOSION_SPRITES])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::EXPLOSIONLITTLE_SPRITES,  new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::EXPLOSIONLITTLE_SPRITES])));
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::MEGAMANEXPLOSION_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::MEGAMANEXPLOSION_SPRITES], true)));

#ifdef DEBUG
    fprintf(stderr,"Preload: [Bonus Point]\n");
#endif
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::BONUS_POINT_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::BONUS_POINT_SPRITES])));

#ifdef DEBUG
  fprintf(stderr, "Preload: [Stage ORB]\n");
#endif
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
    (mm_spritefiles::STAGE_ORB_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::STAGE_ORB_SPRITES])));

#ifdef DEBUG
  fprintf(stderr,"Preload: [BossDoor]\n");
#endif
  preLoadedSprites.insert(std::pair<int, AnimSequence *>
      (mm_spritefiles::BOSSDOOR_SPRITES, new AnimSequence(mm_spritefiles::sprite_files[mm_spritefiles::BOSSDOOR_SPRITES])));

#ifdef DEBUG
  fprintf(stderr,"Preload OK.\n");
#endif

  *player = new Player(*this);
  Player * cur_player = m_player = *player;
  cur_player->x = waypoints[cur_waypoint].x;
  cur_player->y = waypoints[cur_waypoint].y;
  cur_player->old_x = cur_player->x;
  cur_player->old_y = cur_player->y;

#ifdef DEBUG
  fprintf(stderr,"Load weapons.\n");
#endif
  cur_player->weapons.insert(std::pair<mm_weapons::WEAPONS, int>(mm_weapons::W_MEGA_BUSTER, 100)); // +Inf :)
  if (GlobalGameState::hasPlatformGun)
  {
    cur_player->weapons.insert(std::pair<mm_weapons::WEAPONS, int>(mm_weapons::W_PLATFORM_CREATOR, 28));
  }
  for (int i = 0; i < 6; i++) // bosses.
  {
    if (GlobalGameState::enemyDefeated[i] == true)
    {
      cur_player->weapons.insert(std::pair<mm_weapons::WEAPONS, int>((mm_weapons::WEAPONS)(i + 2), 28));
    }
    else
    {
      cur_player->weapons.insert(std::pair<mm_weapons::WEAPONS, int>((mm_weapons::WEAPONS)(i + 2), 0));
    }
  }
#ifdef DEBUG
  fprintf(stderr,"Load weapons OK.\n");
#endif

  camera.w = mm_graphs_defs::UTIL_W;
  camera.h = mm_graphs_defs::UTIL_H;

  std::string tiles_file = stage_path + "/tileset.bmp";
  loadTiles(tiles_file);

  return 0;
}

void Stage::runtimeLoadCharacter(mm_spritefiles::SPRT_TYPE characterSprite)
{
  if (preLoadedSprites.count(characterSprite)==0)
  {
    preLoadedSprites.insert(std::pair<int, AnimSequence *>
      (characterSprite, new AnimSequence(mm_spritefiles::sprite_files[characterSprite])));
  }
}

BITMAP * Stage::getTile(unsigned int index)
{
  if (index < tiles.tile_img.size())
    return tiles.tile_img[index];

  return NULL;
}

int Stage::calculateHeight(float x, float w, float y)
{
  int tilecoordx, tilecoordy;

  int i = 0;
  for (;;++i)
  {
    if (genericColHor(x, w, y, tilecoordx, tilecoordy, true) == false)
      y += 1.0f;
    else
      break;
  }

  return i;
}

void Stage::createEnemies(std::vector<Character *> & characters_vec)
{
  for (int y = 0; y < max_y; ++y)
  {
    for (int x = 0; x < max_x; ++x)
    {
      unsigned char tile_action = map[y][x].action;
      Character * cur_char = NULL;
      if (((tile_action > mm_tile_actions::TILE_DEATH) && (tile_action < mm_tile_actions::TILE_TIMER)) || 
          (tile_action == mm_tile_actions::TILE_DOOR))
      {
        int xpos, ypos;
        xpos = x * mm_graphs_defs::TILE_SIZE;
        ypos = y * mm_graphs_defs::TILE_SIZE;
        xpos += map[y][x].xOffset;
        if ((cur_char = createCharacter(tile_action, xpos, ypos)) != NULL)
        {
#ifdef DEBUG
          fprintf(stderr,"Enemy add: [%s]\n", mm_spritefiles::sprite_files[tile_action]);
#endif
          characters_vec.push_back(cur_char);
        }
      }
    }
  }
}

AnimSequence * Stage::getAnimSeq(int TYPE)
{
  return preLoadedSprites[TYPE];
}

BITMAP * Stage::getTileSet()
{
  return tileset;
}

bool Stage::genericColHor(int x, int w, int y, int &tilecoordx, int &tilecoordy, bool going_down)
{
  int tiletype = 0;
  int tilexpixels = x - (x % mm_graphs_defs::TILE_SIZE);
  int testend     = x + w;

  tilecoordy = y / mm_graphs_defs::TILE_SIZE;
  tilecoordx = tilexpixels/mm_graphs_defs::TILE_SIZE;
  while(tilexpixels <= testend)
  {
    tiletype = this->tileAction(tilecoordx, tilecoordy);

    if (going_down == false)
    {
      if(tiletype == mm_tile_actions::TILE_SOLID)
        return true;
    }
    else
    {
      if((tiletype == mm_tile_actions::TILE_SOLID) ||
         (tiletype == mm_tile_actions::TILE_STAIR_BEGIN))
        return true;
    }

    ++tilecoordx;
    tilexpixels += mm_graphs_defs::TILE_SIZE;
  }

  return false;
}

bool Stage::genericColVer(int x, int y, int h, int &tilecoordx, int &tilecoordy)
{
  int tiletype = 0;
  int realFrameHeight = h;
  y = (y + (h-realFrameHeight));

  int tileypixels = y-(y%mm_graphs_defs::TILE_SIZE);
  int testend     = y + realFrameHeight;

  tilecoordx = x / mm_graphs_defs::TILE_SIZE;

  tilecoordy = tileypixels/mm_graphs_defs::TILE_SIZE;
  while(tileypixels <= testend)
  {
    tiletype = this->tileAction(tilecoordx, tilecoordy);

    if(tiletype == mm_tile_actions::TILE_SOLID)
    {
      return true;
    }

    tilecoordy++;
    tileypixels += mm_graphs_defs::TILE_SIZE;
  }

  return false;
}

void Stage::setOffset(int type, int offset)
{
  offsetMap.insert(std::pair<int,int>(type, offset));
}

int Stage::getOffset(int type)
{
  return (offsetMap[type]);
}

void Stage::draw(BITMAP * bmp, bool hasFg, bool bg_only, bool bg)
{
  int i, j;
  int mapx, mapy;
  int map_xoff, map_yoff;

  mapx = GlobalCamera::mm_camera->x / mm_graphs_defs::TILE_SIZE;
  mapy = GlobalCamera::mm_camera->y / mm_graphs_defs::TILE_SIZE;

  map_xoff = GlobalCamera::mm_camera->x & (mm_graphs_defs::TILE_SIZE-1);
  map_yoff = GlobalCamera::mm_camera->y & (mm_graphs_defs::TILE_SIZE-1);

  int x,y;
  int xdisp = mapx + mm_graphs_defs::TILES_X==max_x ? 0 : 1;
  int ydisp = mapy + mm_graphs_defs::TILES_Y==max_y ? 0 : 1;

  xdisp += mm_graphs_defs::TILES_X;
  ydisp += mm_graphs_defs::TILES_Y;

  map_yoff = -map_yoff;
  map_xoff = -map_xoff;

  MAP_INFO * map_line;

  int tile_number = 0;

#ifdef DEBUG
  int tile_action = 0;
#endif

  if (hasFg == false)
  {
    y = map_yoff;

    for(i = 0; i < ydisp; ++i)
    {
      x = map_xoff;
      map_line = &map[mapy+i][mapx];

      for(j = 0; j < (xdisp>>2); ++j)
      {
#ifdef DEBUG
        tile_action = (*(map_line)).action;
#endif
        tile_number = (*(map_line++)).tile_number;
        blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
        if (tile_action == mm_tile_actions::TILE_SOLID)
          rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
        x += mm_graphs_defs::TILE_SIZE;

#ifdef DEBUG
        tile_action = (*(map_line)).action;
#endif
        tile_number = (*(map_line++)).tile_number;
        blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
        if (tile_action == mm_tile_actions::TILE_SOLID)
          rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
        x += mm_graphs_defs::TILE_SIZE;

#ifdef DEBUG
        tile_action = (*(map_line)).action;
#endif
        tile_number = (*(map_line++)).tile_number;
        blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
        if (tile_action == mm_tile_actions::TILE_SOLID)
          rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
        x += mm_graphs_defs::TILE_SIZE;

#ifdef DEBUG
        tile_action = (*(map_line)).action;
#endif
        tile_number = (*(map_line++)).tile_number;
        blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
        if (tile_action == mm_tile_actions::TILE_SOLID)
          rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
        x += mm_graphs_defs::TILE_SIZE;
      }
      if (xdisp&1)
      {
#ifdef DEBUG
        tile_action = (*(map_line)).action;
#endif
        tile_number = (*(map_line++)).tile_number;
        blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
        if (tile_action == mm_tile_actions::TILE_SOLID)
          rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
      }

      y += mm_graphs_defs::TILE_SIZE;
    }
  }
  else
  {
    if (bg_only == true)
    {
      y = map_yoff;
      for(i = 0; i < ydisp; ++i)
      {
        x = map_xoff;
        map_line = &map[mapy+i][mapx];
        for(j = 0; j < xdisp; ++j)
        {
#ifdef DEBUG
          tile_action = (*(map_line)).action;
#endif
          tile_number = (*(map_line++)).tile_number;
          masked_blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
          if (tile_action == mm_tile_actions::TILE_SOLID)
            rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
          x += mm_graphs_defs::TILE_SIZE;
        }
        y += mm_graphs_defs::TILE_SIZE;
      }
    }
    else
    {
      y = map_yoff;
      for(i = 0; i < ydisp; ++i)
      {
        x = map_xoff;
        map_line = &map[mapy+i][mapx];
        for(j = 0; j < +xdisp; ++j)
        {
          if ((bg == false && map[mapy + i][mapx + j].isForeground == false) ||
              (bg == true  && map[mapy + i][mapx + j].isForeground == true))
          {
            x+=mm_graphs_defs::TILE_SIZE;
            ++map_line;
            continue;
          }

#ifdef DEBUG
          tile_action = (*(map_line)).action;
#endif
          tile_number = (*(map_line++)).tile_number;
          masked_blit(tiles.tile_img[tile_number], bmp, 0,0, x,y, mm_graphs_defs::TILE_SIZE,mm_graphs_defs::TILE_SIZE);
#ifdef DEBUG
          if (tile_action == mm_tile_actions::TILE_SOLID)
            rectfill(bmp, x, y, x+mm_graphs_defs::TILE_SIZE, y+6, makecol(255,0,0));
#endif
          x += mm_graphs_defs::TILE_SIZE;
        }
        y += mm_graphs_defs::TILE_SIZE;
      }
    }
  }
}

bool Stage::checkForBoss(int x, int y)
{
  int yd = y/mm_graphs_defs::TILE_SIZE;
  int xd = x/mm_graphs_defs::TILE_SIZE;
  
  int ydesl = yd / mm_graphs_defs::TILES_Y;
  int xdesl = xd / mm_graphs_defs::TILES_X;
  int sector = ydesl*(max_x/mm_graphs_defs::TILES_X)+xdesl;

  return (sectors[sector].has_boss);
}

bool Stage::cameraSectorHasFgTiles(Camera & camera, Player & player)
{
  // Try to "look ahead" to search for FG tiles.
  int camerax = player.x + player.utilX;

  if (camerax < 0)
  {
    camerax = 0;
  }
  else if (camerax + camera.w >= (this->max_x*mm_graphs_defs::TILE_SIZE)) 
  {
    camerax = (this->max_x*mm_graphs_defs::TILE_SIZE)-camera.w;
  }

  int y = camera.y/mm_graphs_defs::TILE_SIZE;
  int x = camerax/mm_graphs_defs::TILE_SIZE;
  
  int ydesl = y / mm_graphs_defs::TILES_Y;
  int xdesl = x / mm_graphs_defs::TILES_X;
  int sector = ydesl*(this->max_x/mm_graphs_defs::TILES_X)+xdesl;

  return (sectors[sector].has_fg_tiles);
}

void Stage::scrollForbid(Camera & camera)
{
  const int xline = (this->max_x/mm_graphs_defs::TILES_X);
  int y      = camera.y / mm_graphs_defs::TILE_SIZE;

  int x_rght = camera.x + camera.w;
  int x_left = camera.x;

  x_rght /= mm_graphs_defs::TILE_SIZE;
  x_left /= mm_graphs_defs::TILE_SIZE;

  if (x_rght >= this->max_x)
  {
    camera.x = (this->max_x*mm_graphs_defs::TILE_SIZE) - camera.w;
    return;
  }
  else if (x_left < 0)
  {
    camera.x = 0;
    return;
  }

  int ydesl  = y / mm_graphs_defs::TILES_Y;

  int xDesl       = x_rght / mm_graphs_defs::TILES_X;
  int rght_sector = ydesl*xline+xDesl;
  int xDeslLeft   = x_left / mm_graphs_defs::TILES_X;
  int left_sector = ydesl*xline+xDeslLeft;

  if (sectors[rght_sector].scroll_forbid == true)
  {
    camera.x = (camera.x / mm_graphs_defs::UTIL_W) * mm_graphs_defs::UTIL_W;
  }
  else if (sectors[left_sector].scroll_forbid == true)
  {
    camera.x = ((camera.x / mm_graphs_defs::UTIL_W) * mm_graphs_defs::UTIL_W) + camera.w;
  }
}

bool Stage::cameraForbiden(int x, int y)
{
  const int xline = (this->max_x / mm_graphs_defs::TILES_X);

  y /= mm_graphs_defs::TILE_SIZE;
  x /= mm_graphs_defs::TILE_SIZE;

  int yDesl = y / mm_graphs_defs::TILES_Y;
  int xDesl = x / mm_graphs_defs::TILES_X;

  int sector = yDesl * xline + xDesl;

  return sectors[sector].scroll_forbid;
}

bool Stage::doCamera(Camera & camera)
{
  static int dir = 0;
  static int scrollDelay = 0;

  int camerax = (((int)m_player->x) + mm_player_defs::HALFPLAYERWIDTH) - mm_graphs_defs::UTIL_W/2;
  if (camerax < 0)
  {
    camerax = 0;
  }
  else if (camerax + camera.w >= (this->max_x*mm_graphs_defs::TILE_SIZE)) 
  {
    camerax = (this->max_x*mm_graphs_defs::TILE_SIZE)-camera.w;
  }

  if (horz_scroll == false)
  {
    if ((m_player->sy + m_player->h/2) > mm_graphs_defs::UTIL_H)
    {
      horz_scroll = true;
      dir = 1;
    }
    else if ((m_player->sy + m_player->h/2 - 1) < 0 && camera.y != 0)
    {
      //#warning TODO: VER COM CUIDADO!
      // Only scroll UP if megaman is using the stair.
      if (m_player->grabstair == true &&
          (m_player->getCurAnimSeqState() == Player::ONSTAIR || m_player->getCurAnimSeqState() == Player::UPDOWNSTAIR))
      {
        horz_scroll = true;
        dir = -1;
      }
    }

    if (horz_scroll == true)
    {
      GlobalGameState::playerShots.clear();
      GlobalGameState::enemyShots.clear();
    }
  }
  else if (update_scroll == 1)
  {
    update_scroll = 0;

    // Dont follow megaman (going into a death hole)
    if (dir == 1 && cameraForbiden(m_player->x, m_player->y + m_player->h))
    {
      scrollDelay = 7;
	    horz_scroll = false;
	    scroll_count = (mm_graphs_defs::TILES_Y * 2);
    }

    if (scrollDelay == 7)
    {
#ifdef DEBUG
      fprintf(stderr,"Stage::doCamera - atualizando scrool vertical [%d] \n", scroll_count);
#endif

      if (scroll_count < (mm_graphs_defs::TILES_Y*2))
      {
        ++scroll_count;
        camera.y += (dir*(mm_graphs_defs::TILE_SIZE/2));

        if (m_player->grabstair == true)
        {
          m_player->y += (dir * 0.5f);
          m_player->forceAnimation();
        }
      }
      else
      {
        scrollDelay = 0;
        horz_scroll = false;
        scroll_count = 0;
      }
    }
    else
    {
      ++scrollDelay;
    }
  }

  if (horz_scroll == false)
  {
    camera.x = camerax;
    scrollForbid(camera);
  }

  m_player->calcScreenCoords();

  return horz_scroll;
}

void Stage::resetReachMaxX()
{
  reach_max_x = false;
}

void Stage::loadTiles(const std::string & stage_file)
{
  tileset = load_bitmap(stage_file.c_str(), NULL);

  int w,h;
  w = tileset->w - mm_graphs_defs::TILE_SIZE;
  h = tileset->h - mm_graphs_defs::TILE_SIZE;
  for (int y = 2; y < h; y+=mm_graphs_defs::TILE_SIZE+2)
  {
    for (int x = 2; x < w; x+=mm_graphs_defs::TILE_SIZE+2)
    {
      tiles.tile_img.push_back(create_sub_bitmap(tileset, x, y, mm_graphs_defs::TILE_SIZE, mm_graphs_defs::TILE_SIZE));
      tiles.x_pos.push_back(0);
    }
  }
}

Character * Stage::createCharacter(int TYPE, int x, int y, int vx, int vy, void * param)
{
  Character * cur_character = NULL;

  switch(TYPE)
  {
    case mm_tile_actions::TILE_ENEMY_BLADER:
    {
      cur_character = new Blader(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_BEAK:
    {
      cur_character = new Beak(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_SUPERCUTTER:
    {
      cur_character = new SuperCutter(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_FLYINGSHELL:
    {
      cur_character = new FlyingShell(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_KILLERBULLET:
    {
      cur_character = new KillerBullet(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_SPINE:
    {
      cur_character = new Spine(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_MET:
    {
      cur_character = new Met(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_PICKETMAN:
    {
      cur_character = new PicketMan(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_SNIPERJOE:
    {
      cur_character = new SniperJoe(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_WATCHER:
    {
      cur_character = new Watcher(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_TACKLEFIRE:
    {
      cur_character = new TackleFire(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_PENG:
    {
      cur_character = new Peng(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_BIGEYE:
    {
      cur_character = new BigEye(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_FLEA:
    {
      cur_character = new Flea(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_BOMBOMB:
    {
      cur_character = new Bombomb(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_OCTOPUSBATTERY_V:
    {
      cur_character = new OctopusBattery(*this, x, y, OctopusBattery::VERTICAL);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_OCTOPUSBATTERY_H:
    {
      cur_character = new OctopusBattery(*this, x, y, OctopusBattery::HORIZONTAL);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_SCREWBOMBER:
    {
      cur_character = new ScrewBomber(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_CRAZYRAZY:
    {
      cur_character = new CrazyRazy(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_MOVING_PLATFORM:
    {
      cur_character = new MovePlatform(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_LIGHTNING_WALL:
    {
      cur_character = new LightningWall(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_FIRE_H_WALL:
    {
      cur_character = new FireBolt(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_FIRE_V_WALL:
    {
      cur_character = new FirePillar(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_BIG_LIFE_RECHARGE:
    {
      cur_character = new LifeChargerBig(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_LITTLE_LIFE_RECHARGE:
    {
      cur_character = new LifeChargerLittle(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_BIG_WEAPON_RECHARGE:
    {
      cur_character = new WeaponChargerBig(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_LITTLE_WEAPON_RECHARGE:
    {
      cur_character = new WeaponChargerLittle(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_ENEMY_FOOTHOLDER:
    {
      cur_character = new FootHolder(*this, x, y);
    }
      break;
    case mm_tile_actions::TILE_GUTSMAN_ROCK:
    {
      cur_character = new GutsmanRock(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_NEW_LIFE:
    {
      cur_character = new NewLifeItem(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_SPARKLE:
    {
      cur_character = new Sparkle(*this, x, y);
    }
    break;
    case mm_tile_actions::TILE_DOOR:
    {
      cur_character = new BossDoor(*this, x, y, param);
    }
    break;
    case mm_tile_actions::HIT_EXPLOSION_CHAR:
    {
      cur_character = new HitExplosion(*this, x, y);
    }
    break;
    case mm_tile_actions::EXPLOSION_LITTLE_CHAR:
    {
      cur_character = new ExplosionLittle(*this, x, y, param);
      cur_character->velx = vx;
      cur_character->vely = vy;
    }
    break;
    case mm_tile_actions::MEGAMAN_EXPLOSION_CHAR:
    {
      cur_character = new MegamanExplosion(*this, x, y, param);
      cur_character->velx = vx;
      cur_character->vely = vy;
    }
    break;
    case mm_tile_actions::BOMBOMB_FRAGMENT_CHAR:
    {
      cur_character = new Bombomb::BombombFragment(*this, x, y);
      cur_character->velx = vx;
      cur_character->vely = vy;
    }
    break;
    case mm_tile_actions::PICKETMAN_HAMMER_CHAR:
    {
      cur_character = new PicketMan::PicketManHammer(*this, x, y);
      cur_character->velx = vx;
      cur_character->vely = vy;
    }
    break;
    case mm_tile_actions::CRAZYRAZY_LOWER_CHAR:
    {
      cur_character = new CrazyRazy::CrazyRazyFragmentLowerHalf(*this, x, y);
    }
    break;
    case mm_tile_actions::CRAZYRAZY_UPPER_CHAR:
    {
      cur_character = new CrazyRazy::CrazyRazyFragmentUpperHalf(*this, x, y);
    }
    break;
    case mm_tile_actions::TACKLEFIRE_FRAGMENT:
    {
      cur_character = new TackleFire::TackleFireFragment(*this, x, y, param);
    }
    break;
    case mm_tile_actions::SPARKLE_ENM_CHAR:
    {
      cur_character = new SparkleEnm(*this, x, y);
      cur_character->velx = vx;
    }
    break;
    case mm_tile_actions::WATCHER_FRAGMENT:
    {
      cur_character = new Watcher::WatcherFragment(*this, x, y);
    }
    break;
    case mm_tile_actions::BONUS_POINT_CHAR:
    {
      cur_character = new BonusPoint(*this, x, y, param);
    }
    break;
    case mm_tile_actions::GUTSMAN:
    {
      cur_character = new Gutsman(*this, x, y, param);
    }
    break;
    case mm_tile_actions::FIREMAN:
    {
      cur_character = new Fireman(*this, x, y, param);
    }
    break;
    case mm_tile_actions::ELECMAN:
    {
      //cur_character = new Elecman(*this, x, y, param);
    }
    break;
    case mm_tile_actions::MOVING_FLAME:
    {
      cur_character = new MovingFlame(*this, x, y, param);
    }
    break;
    case mm_tile_actions::MOVING_FLAME_FRAGMENT:
    {
      cur_character = new MovingFlame::MovingFlameFragment(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_TIMER_PLATFORM:
    {
      cur_character = new DisappearingBlock(*this, x, y, param);
    }
    break;
    case mm_tile_actions::TILE_TIMER_PLATFORM_FRAGMENT:
    {
      cur_character = new DisappearingBlock::DisappearingBlockFragment(*this, x, y, param);
    }
    break;
	  case mm_tile_actions::MAGNET_BEAM:
	  {
		  cur_character = new MagneticBeam(*this, x, y);
	  }
	  break;
    case mm_tile_actions::GUTSMAN_GUN:
    {
      cur_character = new GutsmanGun(*this, x, y, param);
    }
    break;
    case mm_tile_actions::STAGE_ORB:
    {
      cur_character = new StageOrb(*this, x, y);
    }
    break;
  }

  return (cur_character);
}
