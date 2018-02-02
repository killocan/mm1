/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Spine
class Spine : public Character
{
  public:
    Spine(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    enum {ROAMING, ACTIVE};
  private:
    // Adjust y coord to keep it right above the floor.
    void adjustToFloor();
    // Find minX and maxX based on the stage.
    void findBorders(const Stage & stage, int x, int y);

    int minX, maxX;  // min and max allowed to move.
    //int colorOffset; // which sprite color to use.
    //Stage * cur_stage; // current stage.
};
