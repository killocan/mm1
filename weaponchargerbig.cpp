/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "weaponchargerbig.h"

#include "spritefiles.h"
#include "stage.h"

WeaponChargerBig::WeaponChargerBig(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::BIG_WEAPON_RECHARGE_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = WeaponChargerBig::STILL;
  setAnimSeq(WeaponChargerBig::STILL);
  h = getFrameH();
  w = getFrameW();
  curPlayerWeapon = cur_stage->m_player->curWeapon;

  if (pTemp != NULL)
  {
    bMortal = *((bool *)pTemp);
  }
}

void WeaponChargerBig::die()
{
  alive = false;
}

void WeaponChargerBig::doLogic()
{
  TempCharacter::doLogic();

  static int offsets[] = {0, 0, 3, 5, 4, 6, 1, 2};
  if (cur_stage->m_player->curWeapon != curPlayerWeapon)
  {
    curPlayerWeapon = cur_stage->m_player->curWeapon;
    int offset = offsets[curPlayerWeapon];
    setAnimSeq(WeaponChargerBig::STILL + offset);
  }
}

