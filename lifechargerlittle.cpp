/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "defines.h"
#include "lifechargerlittle.h"
#include "spritefiles.h"

LifeChargerLittle::LifeChargerLittle(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::LITTLE_LIFE_RECHARGE_SPRITES)
{
  this->x = x;
  this->y = y + (mm_graphs_defs::TILE_SIZE/2);;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = LifeChargerLittle::STILL;
  setAnimSeq(LifeChargerLittle::STILL);
  h = getFrameH();
  w = getFrameW();

  alive = true;

  if (pTemp != NULL)
  {
    bMortal = *((bool *)pTemp);
  }
}

bool LifeChargerLittle::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void LifeChargerLittle::die()
{
  alive = false;
}
