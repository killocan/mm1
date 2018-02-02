/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//BigEye
class BigEye : public Character
{
  public:
    BigEye(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    enum {STILL, JUMPING};//, FREEZE};
  private:

    //int curState;    // still or jumping
    //int last_state;
    //int colorOffset; // which sprite color to use.
    //Stage * cur_stage; // current stage.
};
