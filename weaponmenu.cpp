/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <stdio.h>

#include "weaponmenu.h"

#include "text_utils.h"

#include "scenesoundmanager.h"
#include "sfx.h"

#include "globals.h"
#include "globalgamestate.h"

const unsigned int WeaponMenu::menuTileIndex[] = {0,1,2,
                                                  3,4,5,
                                                  3,4,5,
                                                  3,4,5,
                                                  6,7,8};

static const mm_weapons::WEAPONS weaponMenuOrder[] = {mm_weapons::W_BOMBMAN_GUN,      mm_weapons::W_ELECMAN_GUN, 
                                                      mm_weapons::W_GUTSMAN_GUN,      mm_weapons::W_ICEMAN_GUN, 
                                                      mm_weapons::W_CUTMAN_GUN,       mm_weapons::W_FIREMAN_GUN, 
                                                      mm_weapons::W_PLATFORM_CREATOR, mm_weapons::W_MEGA_BUSTER,};

WeaponMenu::WeaponMenu(int stage, Player * player)
{
#ifdef DEBUG
  fprintf(stderr,"Stage: [%d]\n", stage);
#endif

  BITMAP * menuQuadTile;

  m_player = player;

  creating  = true;
  destroing = false;

  curStep = 0;
  destroyStep = 0;

  lastTilePokeTime = blinkerTime = 0;

  /*
  cutman  menu c1: (252,252,252) c2: (188,188,188) c3: (116,116,116)
  gutsman menu c1: (252,116,96)  c2: (200,76,12)   c3: (124,8,0)
  iceman  menu c1: (252,252,252) c2: (0,68,0)      c3: (0,128,136)
  bombman menu c1: (128,208,16)  c2: (0,148,0)     c3: (124,8,0)
  fireman menu c1: (252,252,252) c2: (188,188,188) c3: (164,0,0)
  elecman menu c1: (252,252,252) c2: (252,152,56)  c3: (200,76,12)
  wily1   menu c1: (252,252,252) c2: (114,115,116) c3: (11,41,200)
  wily2   menu c1: (110,255,113) c2: (124,78,0)    c3: (27,157,142)
  wily3   menu c1: (252,252,252) c2: (188,188,188) c3: (116,116,116)
  wily4   menu c1: (255,250,255) c2: (229,110,37)  c3: (172,32,3)
  */
  menuColors[0]  = makecol32(252,252,252); menuColors[1]  = makecol32(188,188,188); menuColors[2]  = makecol32(116,116,116);
  menuColors[3]  = makecol32(252,116,96);  menuColors[4]  = makecol32(200,76,12);   menuColors[5]  = makecol32(124,8,0);
  menuColors[6]  = makecol32(252,252,252); menuColors[7]  = makecol32(0,68,0);      menuColors[8]  = makecol32(0,128,136);
  menuColors[9]  = makecol32(128,208,16);  menuColors[10] = makecol32(0,148,0);     menuColors[11] = makecol32(124,8,0);
  menuColors[12] = makecol32(252,252,252); menuColors[13] = makecol32(188,188,188); menuColors[14] = makecol32(164,0,0);
  menuColors[15] = makecol32(252,252,252); menuColors[16] = makecol32(252,152,56);  menuColors[17] = makecol32(200,76,12);
  menuColors[18] = makecol32(252,252,252); menuColors[19] = makecol32(114,115,116); menuColors[20] = makecol32(11,41,200);
  menuColors[21] = makecol32(110,255,113); menuColors[22] = makecol32(124,78,0);    menuColors[23] = makecol32(27,157,142);
  menuColors[24] = makecol32(252,252,252); menuColors[25] = makecol32(188,188,188); menuColors[26] = makecol32(116,116,116);
  menuColors[27] = makecol32(255,250,255); menuColors[28] = makecol32(229,110,37);  menuColors[29] = makecol32(172,32,3);

  color1 = menuColors[stage*3];
  color2 = menuColors[stage*3+1];
  color3 = menuColors[stage*3+2];

#ifdef DEBUG
  fprintf(stderr,"Color1: [0X%X] Color2: [0X%X] Color3: [0X%X] Offset: [%d]\n", color1, color2, color3, stage*3);
#endif

  menuQuadTile = create_bitmap(QUADW, QUADH);
  menuBitmap   = create_bitmap(W, H);

  clear_bitmap(menuQuadTile);
  triangle(menuQuadTile, 2,  2,  13, 2, 2, 13, color1);
  triangle(menuQuadTile, 13, 13, 13, 4, 4, 13, color2);

  clear_to_color(menuBitmap, color3);
  for (int i = 0; i < W; i+=QUADW)
  {
    draw_sprite(menuBitmap, menuQuadTile, i, 0);
    draw_sprite(menuBitmap, menuQuadTile, i, H-QUADH);
  }
  for (int i = QUADH; i < H-QUADH; i+=QUADH)
  {
    draw_sprite(menuBitmap, menuQuadTile, 0,       i);
    draw_sprite(menuBitmap, menuQuadTile, W-QUADH, i);
  }

  destroy_bitmap(menuQuadTile);

  menuLife = load_bitmap("game_data/life.bmp", NULL);

  // 3 BOMBMAN   0 1st   260x88
  // 5 ELECMAN   1 2st   260x120
  // 1 GUTSMAN   2 3st   260x152
  // 2 ICEMAN    3 4st   260x184
  // 0 CUTMAN    4 5st   260x216
  // 4 FIREMAN   5 6st   260x248
  // MAGNETIC    6 7st   260x280
  // MEGABUSTER  7 8st   260x312
  for (int i = 0; i < 8; i++) availableWeapons.push_back(false);

  if (GlobalGameState::enemyDefeated[3] == true) availableWeapons[0] = true;
  if (GlobalGameState::enemyDefeated[5] == true) availableWeapons[1] = true;
  if (GlobalGameState::enemyDefeated[1] == true) availableWeapons[2] = true;
  if (GlobalGameState::enemyDefeated[2] == true) availableWeapons[3] = true;
  if (GlobalGameState::enemyDefeated[0] == true) availableWeapons[4] = true;
  if (GlobalGameState::enemyDefeated[4] == true) availableWeapons[5] = true;

  if (GlobalGameState::hasPlatformGun == true)
  {
    availableWeapons[6] = true; // Could be added in runtime.
  }
  availableWeapons[7] = true;

  selected = 7;
}

WeaponMenu::~WeaponMenu()
{
  destroy_bitmap(menuLife);
  destroy_bitmap(menuBitmap);
}

bool WeaponMenu::inputAndDraw(BITMAP *buffer)
{
  static int color = 0;
  static const int cyan  = makecol(0,232,216);
  static const int white = makecol(255,255,255);

  static bool isKeyUp=false, isKeyDown=false;
  if (key[KEY_ENTER])
  {
    if (creating == false)
    {
      destroing = true;
    }

    while(key[KEY_ENTER]);
  }

  if (key[KEY_UP] && isKeyUp == false)
  {
    Sounds::mm_sounds->play(MENU_ARROW);

    --selected;

    for (; selected > -1; selected--)
    {
      if (availableWeapons[selected] == true)
      {
        break;
      }
    }

    if (selected == -1)
    {
      selected = 7;
    }

    isKeyUp = true;
  }
  else if (isKeyUp == true && (!key[KEY_UP]))
  {
    isKeyUp = false;
  }

  if (key[KEY_DOWN] && isKeyDown == false)
  {
    Sounds::mm_sounds->play(MENU_ARROW);

    ++selected;
    if (selected < 8)
    {
      for (; selected < 7; selected++)
      {
        if (availableWeapons[selected] == true)
          break;
      }
    }
    else
    {
      for (selected = 0; selected < 8; selected++)
      {
        if (availableWeapons[selected] == true)
          break;
      }
    }

    isKeyDown = true;
  }
  else if (isKeyDown == true && (!key[KEY_DOWN]))
  {
    isKeyDown = false;
  }

  int x, y;
  x = y = 0;

  if (Clock::clockTicks > lastTilePokeTime)
  {
    lastTilePokeTime = Clock::clockTicks;

    if (creating == true)
    {
      if (curStep < 15)
      {
        curStep++;
      }
      else
      {
        x = y = 0;
        creating = false;
      }
    }
    else if (destroing == true)
    {
      if (destroyStep < 15)
      {
        destroyStep++;
      }
      else
      {
        creating  = true;
        destroing = false;

        curStep = 0;
        destroyStep = 0;

        m_player->curWeapon = weaponMenuOrder[selected];
        m_player->changeWeapon();
        return false;
      }
    }
  }

  if (destroing == true)
  {
    x = (destroyStep%3)*TILEW;
    y = (destroyStep/3)*TILEH;
  }

  for (int i = destroyStep; i < curStep; i++)
  {
    if (x == W)
    {
      x = 0;
      y += TILEH;
    }

    blit(menuBitmap, buffer, x,y, X+x, Y+y, TILEW, TILEH);

    x += TILEW;
  }

  //GlobalGameState::enemyDefeated[6]
  // 0 CUTMAN    5st   260x216
  // 1 GUTSMAN   3st   260x152
  // 2 ICEMAN    4st   260x184
  // 3 BOMBMAN   1st   260x88
  // 4 FIREMAN   6st   260x248
  // 5 ELECMAN   2st   260x120
  // MAGNETIC    7st   260x280
  // MEGABUSTER  8st   260x312
  if (curStep == 15 && destroing == false)
  {
    if (Clock::clockTicks-10 > blinkerTime)
    {
      blinkerTime = Clock::clockTicks;
      color = (color == 0) ? white : 0;
    }

    static const char * chars[] = {"B", "E", "G", "I", "C", "F", "M", "P"};
    static const int ypos[]     = {88,  120, 152, 184, 216, 248, 280, 312};
    static const int gunpos[]   = {3, 5, 1, 2, 0, 4};

    for (int i = 0; i < 8; i++)
    {
      if (availableWeapons[i] == true)
      {
        if (i < 6)
        {
          draw_text_shadow(buffer, Font::mm_font, 260, ypos[i], ((i != selected) ? cyan : color), chars[i]);
          rectfill(buffer, 286, ypos[i]+1, 398, ypos[i]+1+15, 0);
          draw_enerybars(buffer, 286+2, ypos[i]+1, (mm_weapons::WEAPONS)(gunpos[i] + 2));
        }
        else if (i == 6)
        {
          draw_text_shadow(buffer, Font::mm_font, 260, ypos[i], ((selected != 6) ? cyan : color), chars[6]);
          rectfill(buffer, 286, ypos[i]+1, 398, ypos[i]+1+15, 0);
          draw_enerybars(buffer, 286+2, 281, mm_weapons::W_PLATFORM_CREATOR);
        }
        else if (i == 7)
        {
          // P - Infinite energy ammo
          draw_text_shadow(buffer, Font::mm_font, 260, ypos[i], ((selected != 7) ? cyan : color), chars[7]);
        }
      }
    }

    static char buf[20];
    sprintf(buf, "= %02d", m_player->lives);
    draw_text_shadow(buffer, Font::mm_font, 340, 318, white, buf);

    draw_sprite(buffer, menuLife, 308, 312);
  }

  return true;
}

void WeaponMenu::draw_enerybars(BITMAP * buffer, int x, int y, mm_weapons::WEAPONS weapon)
{
  for (int i = 0; i < m_player->weapons[weapon]; i++)
  {
    rectfill(buffer, x, y+2, x+1, y+13, makecol(252,228,160));
    rectfill(buffer, x, y+6, x+1, y+9,  makecol(255,255,255));

    x+=4;
  }
}
