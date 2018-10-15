/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//TackleFire
class TackleFire : public Character
{
  public:
    class TackleFireFragment : public Character
    {
      public:
        TackleFireFragment(const Stage & stage, int x, int y, void * param);

        virtual void hit(mm_weapons::weapon_st * pWeapon);
        virtual void doGravitation();
        virtual void doLogic();
        virtual void checkOnCamera();
        virtual void respawn();
        virtual void drawCharacter(BITMAP * bmp);
        void resetState();
        void setWait();

        enum {GOINGUP, GOINDDOWN, RESTING, WAITING};
      private:
        int xorder;
    };

    TackleFire(const Stage & stage, int x, int y);
    ~TackleFire();

    virtual void doLogic();
    virtual BITMAP * getFrame();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void doGravitation();
    virtual void respawn();
    virtual void drawCharacter(BITMAP * bmp);

    enum {CONTROLLING};

  private:
    //Stage * cur_stage; // current stage.
    int fragmentsAlive;
    Character * fragments[3];
    BITMAP * internalSprite;
};
