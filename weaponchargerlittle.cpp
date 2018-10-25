/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "defines.h"
#include "weaponchargerlittle.h"
#include "spritefiles.h"
#include "stage.h"

WeaponChargerLittle::WeaponChargerLittle(const Stage & stage, int x, int y, void * pTemp)
  : TempCharacter(stage, mm_spritefiles::LITTLE_WEAPON_RECHARGE_SPRITES)
{
  this->x = x;
  this->y = y + (mm_graphs_defs::TILE_SIZE/2);

  this->old_x = this->x;
  this->old_y = this->y;

  curState = WeaponChargerLittle::STILL;
  setAnimSeq(WeaponChargerLittle::STILL);
  h = getFrameH();
  w = getFrameW();
  curPlayerWeapon = cur_stage->m_player->curWeapon;

  if (pTemp != NULL)
  {
    bMortal = *((bool *)pTemp);
  }
}

bool WeaponChargerLittle::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void WeaponChargerLittle::die()
{
  alive = false;
}

void WeaponChargerLittle::doLogic()
{
  TempCharacter::doLogic();

  static int offsets[] = {0, 0, 3, 5, 4, 6, 1, 2};
  if (cur_stage->m_player->curWeapon != curPlayerWeapon)
  {
    curPlayerWeapon = cur_stage->m_player->curWeapon;
    int offset = offsets[curPlayerWeapon];
    setAnimSeq(WeaponChargerLittle::STILL + offset);
  }
}
