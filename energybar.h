/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>

#include <allegro.h>
#include "player.h"

class EnergyBar
{
  public:
    static void drawEnerybar(BITMAP * buffer, int x, int y, mm_weapons::WEAPONS weapon);
    static void updateEnergyBar(mm_weapons::WEAPONS weapon, int units);
    // This shit... better be a global :)
    static Player * m_player;
    static int iUnits;
    static unsigned long iTimer;

//    static const int menuColors[];
};
