/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//SniperJoe
class SniperJoe : public Character
{
  public:
    SniperJoe(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void fire();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    virtual void checkOnCamera();

    enum {APPEARING, STILL, FIRING, JUMPING, JUMPINGAWAY};
  private:

    int animSeqSize; // How many frames the current animation has.
    //Stage * cur_stage; // current stage.
};
