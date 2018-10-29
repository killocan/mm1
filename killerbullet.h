/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//KillerBullet
class KillerBullet : public Character
{
  public:
    KillerBullet(const Stage & stage, int x, int y, void * pTemp);
    ~KillerBullet();

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    // This dude is kind of imortal.
    virtual void checkOnCamera();

    enum {MOVING};

  private:
    float tempY, ang;
    int * active_sectors;
};
