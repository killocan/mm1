/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Gutsman Rock
class GutsmanRock : public Character
{
  public:
    GutsmanRock(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual void die();
    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void doGravitation();
    virtual void checkOnCamera();

    enum {STILL, BLINKING, ATTACHED_TO_MEGAMAN};
};
