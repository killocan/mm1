/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "disappearingblock.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//DisappearingBlockFragment

DisappearingBlock::DisappearingBlockFragment::DisappearingBlockFragment(const Stage & stage, int x, int y, void * param)
  : Character(stage, mm_spritefiles::TIMER_PLATFORM_SPRITES)
{
  this->x = x;
  this->y = y;
  this->old_x = x;
  this->old_y = y;

  if (param != 0)
  {
    delay = (unsigned long) param;
  }

  velx = vely = 6.0f;

  isFacingRight = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::TIMER_PLATFORM_SPRITES);
  setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::WAITING1);
  h = getFrameH();
  w = getFrameW();

  curState = DisappearingBlock::DisappearingBlockFragment::WAITING1;

  canCollidePlayer = true;

  alive = true;
}

void DisappearingBlock::DisappearingBlockFragment::doGravitation()
{
}

void DisappearingBlock::DisappearingBlockFragment::checkOnCamera()
{
  if (this->x >= GlobalCamera::mm_camera->x &&
      this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w)
  {
    alive = true;
  }
  else
  {
    resetState();
  }
}

void DisappearingBlock::DisappearingBlockFragment::doLogic()
{
  if (cur_stage->horz_scroll)
    curState = DisappearingBlock::DisappearingBlockFragment::DEAD;
}

void DisappearingBlock::DisappearingBlockFragment::respawn()
{
  alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  {
    ticks = Clock::clockTicks;
    curState = DisappearingBlock::DisappearingBlockFragment::WAITING1;
  }
}

void DisappearingBlock::DisappearingBlockFragment::resetState()
{
  x = old_x;
  y = old_y;

  curState = DisappearingBlock::DisappearingBlockFragment::DEAD;
}

//DisappearingBlock

DisappearingBlock::DisappearingBlock(const Stage & stage, int x, int y, void * param)
  : Character(stage, mm_spritefiles::TIMER_PLATFORM_SPRITES)
{
  this->x = x;
  this->y = y;

  alive = true;

  internalSprite = create_bitmap(10,10);
#ifdef DEBUG
  clear_to_color(internalSprite, makecol(0,255,0));
#else
  clear_to_color(internalSprite, MASK_COLOR_24);
#endif

  if (param != NULL)
  {

  }
}

DisappearingBlock::~DisappearingBlock()
{
  destroy_bitmap(internalSprite);
  internalSprite = NULL;
}

void DisappearingBlock::doLogic()
{
  /*switch(curState)
  {

  }*/
}

void DisappearingBlock::doGravitation()
{
  return;
}

void DisappearingBlock::drawCharacter(BITMAP * bmp)
{
#ifdef DEBUG
  draw_sprite(bmp, internalSprite, sx, sy);
#endif
  return;
}

BITMAP * DisappearingBlock::getFrame()
{
  return internalSprite;
}

void DisappearingBlock::respawn()
{
  /*
  DisappearingBlock::DisappearingBlockFragment * cur;

  for (int i = 0; i < 4; ++i)
  {
    if (fragments[i]->curState == DisappearingBlock::DisappearingBlockFragment::DEAD)
    {
      cur = dynamic_cast <DisappearingBlock::DisappearingBlockFragment *> (fragments[i]);
      cur->resetState();
    }
  }
  */
}