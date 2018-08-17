/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Bombomb
class Bombomb : public Character
{
  public:
    class BombombFragment : public Character
    {
      public:
        BombombFragment(const Stage & stage, int x, int y);

        virtual void doLogic();
        // This dude is kind of imortal.
        virtual void checkOnCamera();

        // First (0) frame is bombomb full sprite. So, pick next one.
        enum {MOVING=1};
    };

    Bombomb(const Stage & stage, int x, int y);

    virtual void die();
    virtual void doLogic();
    // Do nothing
    virtual void doGravitation();
    // Control states based on animation state.
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void respawn();
    // This dude is kind of imortal.
    virtual void checkOnCamera();

    enum {MOVING, WAITING};
};
