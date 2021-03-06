/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Gutsman
class Fireman : public Character
{
  public:
    Fireman(const Stage & stage, int x, int y, void * param);

    virtual void doLogic();

    virtual void fire();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);

    virtual void hit(mm_weapons::weapon_st * pWeapon);

    enum { DECIDING, SHOW_OFF, PRE_JUMP, JUMP, PRE_JUMP_ATTACK, JUMP_ATTACK, ATTACK, };

  private:
    bool jumping, jumping_right;
};
