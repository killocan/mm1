/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//BonusPoint
class BonusPoint : public TempCharacter
{
  public:
    BonusPoint(const Stage & stage, int x, int y, void * pTemp);
    virtual void doLogic();
    virtual void die();

    enum {STILL};
};
