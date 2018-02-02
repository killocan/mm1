/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//NewLifeItem
class NewLifeItem : public TempCharacter
{
  public:
    NewLifeItem(const Stage & stage, int x, int y, void * pTemp);

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void die();
    virtual void doLogic();

    enum {STILL};
    mm_weapons::WEAPONS curPlayerWeapon;
};
