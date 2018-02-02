/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//LifeChargerBig
class LifeChargerBig : public TempCharacter
{
  public:
    LifeChargerBig(const Stage & stage, int x, int y, void * pTemp);
    virtual void die();

    enum {STILL};
};
