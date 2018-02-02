/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//FirePillar
class FirePillar : public Character
{
  public:
    FirePillar(const Stage & stage, int x, int y);
    ~FirePillar();

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    virtual BITMAP * getFrame(); // Virtual, so subclasses could manipulate and deliver customized bitmaps for colision check.

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);

    virtual void respawn();

    virtual void defrost();

    virtual void drawCharacter(BITMAP * bmp);

    enum {GOING_UP, GOING_DOWN};
  private:
    //Stage * cur_stage; // current stage.
    int offset;
    unsigned long pausebtwstates;
    BITMAP * sprite;
};
