/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//OctopusBattery
class OctopusBattery : public Character
{
  public:
    enum e_orientation {VERTICAL, HORIZONTAL};
    OctopusBattery(const Stage & stage, int x, int y, e_orientation orientation);

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    enum {SLEEPING, ACTIVE, SLEEP};
  private:
    // Adjust x coord to keep it right next to the nearest floor.
    void adjustToWall();
    // Find minX and maxX based on the stage.
    void findBorders(const Stage & stage, int x, int y);

    // Do specific shit if H or V
    void horLogic();
    void verLogic();

    // Horizontal or Vertical enemy? this don't change.
    e_orientation m_orientation;

    int minX, maxX;  // min and max allowed to move on X axis.
    int minY, maxY;  // min and max allowed to move on Y axis.

    int closer;      // 0 left | 1 right
    //int cur_state;   // enum {SNG, A, SL}
    //int colorOffset; // which sprite color to use.
    //Stage * cur_stage; // current stage.
};
