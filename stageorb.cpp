/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageorb.h"

#include "spritefiles.h"

StageOrb::StageOrb(const Stage & stage, int x, int y)
  : TempCharacter(stage, mm_spritefiles::STAGE_ORB_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  curState = StageOrb::STILL;
  setAnimSeq(StageOrb::STILL);
  h = getFrameH();
  w = getFrameW();
}

void StageOrb::die()
{
  alive = false;
}
