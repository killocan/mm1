/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "globalgamestate.h"

std::list<mm_weapons::weapon_st> GlobalGameState::playerShots;
std::list<mm_weapons::weapon_st> GlobalGameState::enemyShots;

bool GlobalGameState::hasPlatformGun          = true;

bool GlobalGameState::allEnemysDefeated       = false;
bool GlobalGameState::enemyDefeated[6]        = {true, true, true, true, true, true};
int  GlobalGameState::enemyClearPoints[6]     = {50000, 80000, 90000, 30000, 50000, 100000};

volatile int GlobalGameState::game_logic_tick = 0;

void GlobalGameState::update_game_logic_tick()
{
  game_logic_tick = 1;
}
