/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "footholder.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//FootHolder

FootHolder::FootHolder(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FOOTHOLDER_SPRITES)
{
  this->x = x;
  this->y = y;
  this->old_x = this->x;
  this->old_y = this->y;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FOOTHOLDER_SPRITES);
  curState = FootHolder::MOVING;
  setAnimSeq(colorOffset + FootHolder::MOVING);
}

void FootHolder::doLogic()
{
}

void FootHolder::doGravitation()
{
  return;
}
