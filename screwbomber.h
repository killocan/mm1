/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//ScrewBomber
class ScrewBomber : public Character
{
  public:
    ScrewBomber(const Stage & stage, int x, int y);
    ~ScrewBomber();

    virtual void doLogic();

    // Adjust bullets vx, vy.
    virtual void fire();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    virtual void drawCharacter(BITMAP * bmp);
    virtual BITMAP * getFrame();

    enum {SLEEPING, OPENING, FIRING, CLOSING};
  private:
    // Adjust x coord to keep it right next to the nearest wall.
    void adjustToFloorOrCeiling(int x_map, int y_map);

    int old_map_x, old_map_y;
    int closer; // which one is closer (floor or ceiling)?

    int fireCount; // How many fire sequences did I fired?
    int fireFrameCount;

    int animSeqSize; // How many frames the current animation has.

    int bulletOffset;

    //Stage * cur_stage; // current stage.
    BITMAP * sprite;
};
