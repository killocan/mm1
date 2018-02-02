/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

class Character;
class Player;
class Camera;
class AnimSequence;
class Stage;

// 0 CUTMAN
// 1 GUTSMAN
// 2 ICEMAN
// 3 BOMBMAN
// 4 FIREMAN
// 5 ELECMAN
namespace mm_weapons
{
  enum WEAPONS {W_MEGA_BUSTER, W_PLATFORM_CREATOR,
                W_CUTMAN_GUN, W_GUTSMAN_GUN,
                W_ICEMAN_GUN, W_BOMBMAN_GUN,
                W_FIREMAN_GUN, W_ELECMAN_GUN,
                WEAPONS_NUMBER};

  struct weapon_st
  {
    int sx, sy;
    int w, h;

    float x,y;
    float vx,vy;

    // x,y coord where collision happen.
    int xcol, ycol;

    float iceman_x_dist;

    bool alive; // if false, will be removed from list.

    int frameOffset;
    AnimSequence * bulletSpriteShet;
    mm_weapons::WEAPONS weapon;
  };

  void updateWeapons(Stage  * stage);
  void drawWeapons(BITMAP * buffer);

  void createMegaBuster(Player * player);
  void createMegaBuster(Character * character, int x, int y, float vx, float vy, int offset);

  void createIceSlasher(Player * player);
  void createIceSlasher(Character * character, int x, int y, float vx, float vy, int offset);
}
