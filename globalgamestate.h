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
    //static AnimSequence * weaponsFrames[mm_weapons::WEAPONS_NUMBER];
    //static void loadWeaponsFrames();
    static std::list<mm_weapons::weapon_st> playerShots;
    static std::list<mm_weapons::weapon_st> enemyShots;
    static bool hasPlatformGun;

    static bool allEnemysDefeated;
    static bool enemyDefeated[6];
    static int  enemyClearPoints[6];

    static volatile int game_logic_tick;
    static void update_game_logic_tick();
};
