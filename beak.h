/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Beak
class Beak : public Character
{
  public:
    Beak(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Adjust bullets vx, vy.
    virtual void fire();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    enum {SLEEPING, OPENING, FIRING, CLOSING};//, FREEZE};
  private:
    // Adjust x coord to keep it right next to the nearest wall.
    void adjustToWall(int x_map, int y_map);

    int old_map_x, old_map_y;
    int closer; // which wall is closer?

    int fireCount; // How many bullets did I fired?

    int bulletOffset;

    int animSeqSize; // Control how many frames the current animation has.

    //Stage * cur_stage; // current stage.
};
