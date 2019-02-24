/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//ExplosionLittle
class ExplosionLittle : public Character
{
  public:
    ExplosionLittle(const Stage & stage, int x, int y, void * param);

    virtual void doLogic();
    // Do nothing
    virtual void doGravitation();

    // Manage its own death
    virtual void checkOnCamera();

    enum {EXPLODING, BOSS_DIE};

  private:
    bool bOneCicle; // if true after one animation cicle it dies.
};
