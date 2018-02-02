/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//HitExplosion
class HitExplosion : public Character
{
  public:
    HitExplosion(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    // Manage its own death
    virtual void checkOnCamera();

    enum {EXPLODING};

  private:
    //Stage * cur_stage; // current stage.
};
