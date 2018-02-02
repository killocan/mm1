/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Gutsman Rock
class GutsmanRock : public Character
{
  public:
    GutsmanRock(const Stage & stage, int x, int y);

    virtual void doLogic();
    virtual void doGravitation();

    enum {STILL, BLINKING, ATTACHED_TO_MEGAMAN};
};
