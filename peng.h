/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Peng
class Peng : public Character
{
  public:
    Peng(const Stage & stage, int x, int y, void * pTemp);
    ~Peng();

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    // This dude is kind of imortal.
    virtual void checkOnCamera();

    enum {WAITING, MOVING};

  private:
    // For smooth sin move.
    float tempY, ang;
    int * active_sectors;
    unsigned long ticks;
    //Stage * cur_stage; // current stage.
};
