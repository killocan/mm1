/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Sparkle
class Sparkle : public Character
{
  public:
    Sparkle(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual void respawn();

    enum {ON};
  private:
    // Adjust xy coord to center the tile.
    void adjustCoord();

    //Stage * cur_stage; // current stage.
};
