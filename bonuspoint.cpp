/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "bonuspoint.h"

#include "spritefiles.h"
#include "stage.h"

#include <cstdio>

BonusPoint::BonusPoint(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::BONUS_POINT_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = BonusPoint::STILL;
  colorOffset = cur_stage->getOffset(mm_spritefiles::BONUS_POINT_SPRITES);
  setAnimSeq(colorOffset + BonusPoint::STILL);

  canCollidePlayer = true;
  alive = true;

  h = getFrameH();
  w = getFrameW();

  bMortal = (pTemp != NULL) ? *((bool *)pTemp) : false;
}

void BonusPoint::doLogic()
{
  if (cur_stage->m_player->alive == false)
  {
    this->alive = false;
    return;
  }

  TempCharacter::doLogic();
}

void BonusPoint::die()
{
    alive = false;
}
