/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Met
class Met : public Character
{
  public:
    Met(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Adjust bullets vx, vy.
    virtual void fire();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    enum {CLOSED, FIRING, CLOSING};
  private:
    // Adjust y coord to keep it right above the floor.
    void adjustToFloor();

    int m_ypos;

    //int cur_state;   // enum {CLD, FIR, CLNG}
    //Stage * cur_stage; // current stage.
};
