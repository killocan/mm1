/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//PicketMan
class PicketMan : public Character
{
  public:
    class PicketManHammer : public Character
    {
      public:
        PicketManHammer(const Stage & stage, int x, int y);

        virtual bool collisionHor(int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype);
        virtual void doLogic();
        // This dude is kind of imortal.
        virtual void checkOnCamera();

        enum {MOVING};
    };

    PicketMan(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void fire();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    //virtual void checkOnCamera(Camera * camera);

    enum {APPEARING, STILL, FIRING};
  private:

    int howManyToFire;
    int animSeqSize; // How many frames the current animation has.
    //Stage * cur_stage; // current stage.
};
