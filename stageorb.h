/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//StageOrb
class StageOrb : public TempCharacter
{
  public:
    StageOrb(const Stage & stage, int x, int y);
    virtual void die();

    enum {STILL};
};
