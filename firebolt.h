/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//FireBolt
class FireBolt : public Character
{
  public:
    FireBolt(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual void respawn();

    enum {ON};
  private:

    //Stage * cur_stage; // current stage.
};
