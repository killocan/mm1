/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//LightningWall
class LightningWall : public Character
{
  public:
    LightningWall(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL); // Control states based on animation state.

    // Do nothing
    virtual void doGravitation();

    virtual void respawn();

    enum {OFF, ON};
  private:
    //Stage * cur_stage; // current stage.
};
