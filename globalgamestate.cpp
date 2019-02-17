/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "globalgamestate.h"

std::list<mm_weapons::weapon_st> GlobalGameState::playerShots;
std::list<mm_weapons::weapon_st> GlobalGameState::enemyShots;

bool GlobalGameState::hasPlatformGun          = true;

bool GlobalGameState::earthquake              = false;
int  GlobalGameState::earthquakecount         = 0;

int GlobalGameState::bonus_points             = 0;
int GlobalGameState::lives                    = 2;

bool GlobalGameState::allEnemysDefeated       = false;
bool GlobalGameState::enemyDefeated[6]        = {true, true, true, true, true, true};
int  GlobalGameState::enemyClearPoints[6]     = {50000, 80000, 90000, 30000, 50000, 100000};

volatile int GlobalGameState::game_logic_tick = 0;
volatile int GlobalGameState::game_logic_lps  = 0;
volatile int GlobalGameState::game_logic_lps_count = 0;
void GlobalGameState::update_game_logic_tick()
{
  game_logic_tick = 1;
}

volatile int GlobalGameState::sm_fps = 0;
volatile int GlobalGameState::sm_fps_count = 0;
void GlobalGameState::sm_check_fps()
{
  GlobalGameState::game_logic_lps = GlobalGameState::game_logic_lps_count;
  GlobalGameState::game_logic_lps_count = 0;

  sm_fps = sm_fps_count;
  sm_fps_count = 0;
}
