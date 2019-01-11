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
    ~GutsmanRock();

    virtual BITMAP * getFrame();
    //virtual void drawCharacter(BITMAP * bmp);

    virtual void doLogic();
    virtual void die();
    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void doGravitation();
    virtual void checkOnCamera();

    enum {STILL, BLINKING, ATTACHED_TO};
  private:
    BITMAP * sprite;
};
