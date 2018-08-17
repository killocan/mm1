/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//SuperCutter
class SuperCutter : public Character
{
  public:
    SuperCutter(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual bool collisionHor(const Stage & stage,   int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype);

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    // This dude is kind of imortal.
    virtual void checkOnCamera();

    enum {GOING_UP, CUTTING};

  private:
    void move();

    int xDestination;

    // Smooth movement.
    float tempX, tempVelX;

    bool isFlying;
    //Stage * cur_stage; // current stage.
};
