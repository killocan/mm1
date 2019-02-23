/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//StageOrb
class StageOrb : public Character
{
  public:
    StageOrb(const Stage & stage, int x, int y);
    virtual void doLogic();

    enum {STILL};
};
