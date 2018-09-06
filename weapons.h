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
    weapon_st()
    {
      sx = 0;
      sy = 0;
      w = 0;
      h = 0;

      x = 0.0f;
      y = 0.0f;
      vx = 0.0f;
      vy = 0.0f;

      xcol = 0;
      ycol = 0;

      ticks = 0;

      iceman_x_dist = 0.0f;

      //hyper_bomb_lifetime = 0;
      hyper_bomb_bounce_count = 0;
      hyper_bomb_should_bounce = false;
      life = 0;

      alive = true;
      can_hurt = true;
      subtype = false;

      subtype_num = 0;
      frameOffset = 0;
      frames = 0;
      current_frame = 0;

      bulletSpriteShet = NULL;
    }
    int sx, sy;
    int w, h;

    float x,y;
    float vx,vy;

    // x,y coord where collision happen.
    int xcol, ycol;

    int life; // How far the weapon should live?

    unsigned long ticks; // to control time related stuff

    float iceman_x_dist;

    //int hyper_bomb_lifetime;
    int hyper_bomb_bounce_count;
    bool hyper_bomb_should_bounce;

    int CURVE_PNTS = 50;
    int cutter_current_point = 0;
    int cutter_curve_X[CURVE_PNTS];
    int cutter_curve_Y[CURVE_PNTS];
    int cutter_ctrl_points[8];


    bool alive; // if false, will be removed from list.
    bool can_hurt; // if false, dont hurt player or enemy, just to see.
    bool subtype; // is it created by other weapon?
    int subtype_num; // witch subtype is it?

    int frameOffset; // initial frame
    int frames; // number of frames
    int current_frame;

    AnimSequence * bulletSpriteShet;
    mm_weapons::WEAPONS weapon;
  };

  void updateWeapons(Stage  * stage);
  void drawWeapons(BITMAP * buffer);

  void createMegaBuster(Player * player);
  void createMegaBuster(Character * character, int x, int y, float vx, float vy, int offset);

  void createIceSlasher(Player * player);
  void createIceSlasher(Character * character, int x, int y, float vx, float vy, int offset);

  void createHyperBomb(Player *player);
  void createHyperBomb(Character * character, int x, int y, float vx, float vy, int offset);

  void createThunderBeam(Player *player);
  void createThunderBeam(Character * character, int x, int y, float vx, float vy, int offset);

  void createRollingCutter(Player *player);
  void createRollingCutter(Character * character, int x, int y, float vx, float vy, int offset);
}
