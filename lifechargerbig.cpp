/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "lifechargerbig.h"

#include "spritefiles.h"

LifeChargerBig::LifeChargerBig(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::BIG_LIFE_RECHARGE_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = LifeChargerBig::STILL;
  setAnimSeq(LifeChargerBig::STILL);
  h = getFrameH();
  w = getFrameW();
  
  if (pTemp != NULL)
  {
    bMortal = *((bool *)pTemp);
  }
}

void LifeChargerBig::die()
{
  alive = false;
}
