/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//WeaponChargerLittle
class WeaponChargerLittle : public TempCharacter
{
  public:
    WeaponChargerLittle(const Stage & stage, int x, int y, void * pTemp);

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void die();
    virtual void doLogic();

    enum {STILL};
    mm_weapons::WEAPONS curPlayerWeapon;
};
