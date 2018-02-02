/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//LifeChargerLittle
class LifeChargerLittle : public TempCharacter
{
  public:
    LifeChargerLittle(const Stage & stage, int x, int y, void * pTemp);

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void die();

    enum {STILL};
};
