/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//MovePlatform
class MovePlatform : public Character
{
  public:
    MovePlatform(const Stage & stage, int x, int y);

    virtual void doLogic();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);

    // Do nothing
    virtual void doGravitation();
    virtual void checkOnCamera();

    virtual void respawn();

    enum {HOLDING_MOVE, DOWN_ANIM, UP_ANIM, DOWM_MOVE};
};
