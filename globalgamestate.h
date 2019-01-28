/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <list>
#include "weapons.h"
#include "animsequence.h"

class GlobalGameState
{
  public:
    static std::list<mm_weapons::weapon_st> playerShots;
    static std::list<mm_weapons::weapon_st> enemyShots;
    static bool hasPlatformGun;

    static bool earthquake;
    static int  earthquakecount;

    static bool allEnemysDefeated;
    static bool enemyDefeated[6];
    static int  enemyClearPoints[6];

    static volatile int game_logic_tick;
    static volatile int game_logic_lps;
    static volatile int game_logic_lps_count;
    static void update_game_logic_tick();

    static volatile int sm_fps;
    static volatile int sm_fps_count;
    static void sm_check_fps();
};
