/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Watcher
class Watcher : public Character
{
  public:
    class WatcherFragment : public Character
    {
      public:
        WatcherFragment(const Stage & stage, int x, int y);

        virtual void hit(mm_weapons::weapon_st * pWeapon);
        virtual void doGravitation();
        virtual void doLogic();
        virtual void checkOnCamera();
        virtual void respawn();

        void resetState(int x, int y);
        void setWait();

        enum {WAITING, CHASING, OPENING, OPEN, FIRING, CLOSING, FLEEING, RESTING};
    };

    Watcher(const Stage & stage, int x, int y);
    virtual ~Watcher();

    virtual void doLogic();
    virtual BITMAP * getFrame();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void doGravitation();
    virtual void respawn();
    virtual void drawCharacter(BITMAP * bmp);

    enum {CONTROLLING};

  private:
    bool childAlive;
    Character * fragments[3];
    BITMAP * internalSprite;
};
