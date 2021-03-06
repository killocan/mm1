/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//FlyingShell
class FlyingShell : public Character
{
  public:
    FlyingShell(const Stage & stage, int x, int y, void * pTemp);
    virtual ~FlyingShell();

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);

    virtual void fire();

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    // This dude is kind of imortal.
    virtual void checkOnCamera();

    enum {RESTING, PRE_FIRING, FIRING, CLOSED};

  private:
    int * active_sectors;
};
