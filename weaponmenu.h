/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>

#include <allegro.h>
#include "player.h"

class WeaponMenu
{
  public:
    WeaponMenu(int stage, Player * player);
    ~WeaponMenu();

    bool inputAndDraw(BITMAP *buffer);

    int selected;
  private:
    const static int W = 192;
    const static int H = 320;
    const static int X = 232;
    const static int Y = 48;
    const static int QUADW = 16;
    const static int QUADH = 16;
    const static int TILEW = 64;
    const static int TILEH = 64;

    // This shit... better be a global :)
    Player * m_player;

    BITMAP * menuBitmap;
    BITMAP * menuLife;

    unsigned int color1, color2, color3;

    bool creating, destroing;

    int curStep;
    int destroyStep;

    static const unsigned int menuTileIndex[];
    int menuColors[30];

    std::vector<bool> availableWeapons;

    unsigned long lastTilePokeTime;
    unsigned long blinkerTime;

    void draw_enerybars(BITMAP * buffer, int x, int y, mm_weapons::WEAPONS weapon);
};
