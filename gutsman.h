/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class GutsmanGun;
class Stage;

//Gutsman
class Gutsman : public Character
{
  public:
    Gutsman(const Stage & stage, int x, int y, void * param);

    virtual void doLogic();
    virtual void touchGround();
    virtual void die();
    virtual void fire();
    virtual void drawCharacter(BITMAP * bmp);
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);

    virtual void hit(mm_weapons::weapon_st * pWeapon);

    enum { SHOW_OFF, DECIDING, PRE_JUMP, JUMP, PRE_JUMP_ATTACK, JUMP_ATTACK, ATTACK, };
    int cycleCount;

  private:
    GutsmanGun * rock;
    bool bFirstTime;
    bool jumping, jumping_right;

    bool bDying;
    unsigned long dyingTimer;
    int dieStep;

    bool isHitAnimOn;
    bool bDraw;
    unsigned long blinkBegin;
    int blinkCount;
};
