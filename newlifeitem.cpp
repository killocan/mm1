/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "newlifeitem.h"

#include "spritefiles.h"
#include "stage.h"

NewLifeItem::NewLifeItem(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::NEW_LIFE_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = NewLifeItem::STILL;
  setAnimSeq(NewLifeItem::STILL);
  h = getFrameH();
  w = getFrameW();
  curPlayerWeapon = cur_stage->m_player->curWeapon;

  if (pTemp != NULL)
  {
    bMortal = *((bool *)pTemp);
  }
}

bool NewLifeItem::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void NewLifeItem::die()
{
  alive = false;
}

void NewLifeItem::doLogic()
{
  TempCharacter::doLogic();

  static int offsets[] = {0, 0, 3, 5, 4, 6, 1, 2};
  if (cur_stage->m_player->curWeapon != curPlayerWeapon)
  {
    curPlayerWeapon = cur_stage->m_player->curWeapon;
    int offset = offsets[curPlayerWeapon];
    setAnimSeq(NewLifeItem::STILL + offset);
  }
}
