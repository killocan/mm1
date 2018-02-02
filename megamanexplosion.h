/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//MegamanExplosion
class MegamanExplosion : public Character
{
  public:
    MegamanExplosion(const Stage & stage, int x, int y, void * param);

    virtual void doLogic();

    //virtual bool handleAnimation(bool * bAnimationEnded);

    // Do nothing
    virtual void doGravitation();

    // Manage its own death
    virtual void checkOnCamera();

    enum {EXPLODING};

  private:
    //Stage * cur_stage; // current stage.
    bool bOneCicle; // if true after one animation cicle it dies.
};
