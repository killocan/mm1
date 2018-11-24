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

  DisappearingBlockSetupParam * p = (DisappearingBlockSetupParam*) param;
  this->delayFirstShow = p->delayFirstShow;
  this->delayOnScreen  = p->delayOnScreen;
  this->delayNextShow  = p->delayNextShow;

  this->ticks           = Clock::clockTicks;
  this->firstShow       = true;

  isFacingRight = true;

  colorOffset = cur_stage->getOffset(mm_spritefiles::TIMER_PLATFORM_SPRITES);
  setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::WAITING1);
  h = getFrameH();
  w = getFrameW();

  curState = DisappearingBlock::DisappearingBlockFragment::WAITING1;

  alive = true;

  canCollidePlayer = true;
}

void DisappearingBlock::DisappearingBlockFragment::drawCharacter(BITMAP * bmp)
{
  int tempY = sy;
  sy += 5;
  Character::drawCharacter(bmp);
  sy = tempY;
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

  switch (curState)
  {
    case DisappearingBlock::DisappearingBlockFragment::WAITING1:
    {
      if ((Clock::clockTicks - this->ticks) > this->delayFirstShow)
      {
        Sounds::mm_sounds->play(DBLOCK);

        setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::SHOWING);
        this->curState = DisappearingBlock::DisappearingBlockFragment::SHOWING;
      }
    }
    break;
    case DisappearingBlock::DisappearingBlockFragment::SHOWING:
    {
      bool bAnimEnded;
      handleAnimation(&bAnimEnded);

      if (bAnimEnded)
      {
        setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::ALIVE);
        this->curState = DisappearingBlock::DisappearingBlockFragment::ALIVE;
        this->ticks = Clock::clockTicks;

        int xd = ((int)this->x)/mm_graphs_defs::TILE_SIZE;
        int yd = ((int)this->y)/mm_graphs_defs::TILE_SIZE;
        cur_stage->setTileAction(xd,yd,mm_tile_actions::TILE_SOLID);
      }
    }
    break;
    case DisappearingBlock::DisappearingBlockFragment::ALIVE:
    {
      if ((Clock::clockTicks - this->ticks) > this->delayOnScreen)
      {
        setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::WAITING1);
        this->curState = DisappearingBlock::DisappearingBlockFragment::WAITING2;
        this->ticks = Clock::clockTicks;

        int xd = ((int)this->x)/mm_graphs_defs::TILE_SIZE;
        int yd = ((int)this->y)/mm_graphs_defs::TILE_SIZE;
        cur_stage->setTileAction(xd,yd,mm_tile_actions::TILE_VOID);
      }
    }
    break;
    case DisappearingBlock::DisappearingBlockFragment::WAITING2:
    {
      if ((Clock::clockTicks - this->ticks) > this->delayNextShow)
      {
        Sounds::mm_sounds->play(DBLOCK);

        setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::SHOWING);
        this->curState = DisappearingBlock::DisappearingBlockFragment::SHOWING;
        this->ticks = Clock::clockTicks;
      }
    }
    break;
    case DisappearingBlock::DisappearingBlockFragment::DEAD:
    {
      setAnimSeq(colorOffset + DisappearingBlock::DisappearingBlockFragment::WAITING1);

      int xd = ((int)this->x)/mm_graphs_defs::TILE_SIZE;
      int yd = ((int)this->y)/mm_graphs_defs::TILE_SIZE;
      cur_stage->setTileAction(xd,yd,mm_tile_actions::TILE_VOID);
    }
    break;
  }
}

void DisappearingBlock::DisappearingBlockFragment::respawn()
{
  alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  ticks = Clock::clockTicks;
  curState = DisappearingBlock::DisappearingBlockFragment::WAITING1;
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
    std::vector<DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam>
      * blocks = (std::vector<DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam> *) param;

    std::vector<DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam>::iterator i;
    DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam * blockSetup;
    for (i = blocks->begin(); i != blocks->end(); ++i)
    {
      blockSetup = &(*i);
      Character * p = cur_stage->createCharacter(mm_tile_actions::TILE_TIMER_PLATFORM_FRAGMENT,
                                                 blockSetup->x, blockSetup->y, 0,0, (void*)blockSetup);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(p);
      fragments.push_back(p);
    }
  }
  else
  {
    fprintf(stderr,"ERROR! MISSING BLOCKS SETUP INFO.\n");
    exit(-1);
  }
}

DisappearingBlock::~DisappearingBlock()
{
  destroy_bitmap(internalSprite);
  internalSprite = NULL;
}

void DisappearingBlock::doLogic()
{
  for (unsigned i = 0; i < fragments.size(); ++i)
  {
    if (fragments[i]->curState == DisappearingBlock::DisappearingBlockFragment::DEAD)
    {
      DisappearingBlock::DisappearingBlockFragment * cur;
      cur = dynamic_cast <DisappearingBlock::DisappearingBlockFragment *> (fragments[i]);
      cur->respawn();
    }
  }
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
  DisappearingBlock::DisappearingBlockFragment * cur;

  for (unsigned i = 0; i < fragments.size(); ++i)
  {
    if (fragments[i]->curState == DisappearingBlock::DisappearingBlockFragment::DEAD)
    {
      cur = dynamic_cast <DisappearingBlock::DisappearingBlockFragment *> (fragments[i]);
      cur->resetState();
    }
  }
}