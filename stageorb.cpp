/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageorb.h"
#include "stage.h"
#include "spritefiles.h"

StageOrb::StageOrb(const Stage & stage, int x, int y)
  : Character(stage, mm_spritefiles::STAGE_ORB_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  colorOffset = cur_stage->getOffset(mm_spritefiles::STAGE_ORB_SPRITES);
  setAnimSeq(colorOffset + StageOrb::STILL);
  
  curState = StageOrb::STILL;

  h = getFrameH();
  w = getFrameW();

  alive = true;
}

void StageOrb::doLogic()
{
  Character::handleAnimation();
}
