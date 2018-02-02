/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "tempcharacter.h"

class Stage;

//WeaponChargerBig
class WeaponChargerBig : public TempCharacter
{
  public:
    WeaponChargerBig(const Stage & stage, int x, int y, void * pTemp);
    virtual void die();
    virtual void doLogic();

    enum {STILL};
    mm_weapons::WEAPONS curPlayerWeapon;
};
