/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "bossdoor.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//BossDoor

BossDoor::BossDoor(const Stage & stage, int x, int y, void * param) : Character(stage, mm_spritefiles::BOSSDOOR_SPRITES)
{
  this->x = x;
  this->y = y;

  config = *((BossDoorSetupParam*)param);

  vely          = 1;
  overstair     = false;
  isFacingRight = true;
  life          = 1;
  alive         = true;
  doorOpenDelay = 0;
  canCollidePlayer = true;
  hasBeenUsed   = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BOSSDOOR_SPRITES);
  setAnimSeq(colorOffset + BossDoor::CLOSED);
 
  h = getFrameH();
  w = getFrameW();

  this->old_x = this->x;
  this->old_y = this->y;

  if (config.orientation == 0)
    sprite = create_bitmap(w * config.size,h * config.totalSize);
  else
    sprite = create_bitmap(w * config.totalSize, h * config.size);

  h = sprite->h;
  w = sprite->w;

  curState = config.defaultOpen ? BossDoor::OPENED : BossDoor::CLOSED;
  curDoorLevel = curState == BossDoor::CLOSED ? config.totalSize : 0;

  if (curState == BossDoor::CLOSED)
  {
    int xd = (((int)x)+mm_graphs_defs::TILE_SIZE+config.size*mm_graphs_defs::TILE_SIZE)/mm_graphs_defs::TILE_SIZE;
    int yd = ((int)y)/mm_graphs_defs::TILE_SIZE;
    xcamblock = xd;
    ycamblock = yd;
    cur_stage->defineCameraSector(xd, yd, true);
  }
}

BossDoor::~BossDoor()
{
  destroy_bitmap(sprite);
  sprite = NULL;
}

void BossDoor::openDoor()
{
  if (curState == BossDoor::CLOSED && hasBeenUsed == false)
  {
    curState = BossDoor::OPENING;
    doorOpenDelay = Clock::clockTicks;
    Sounds::mm_sounds->play(DOOR);
  }
}

void BossDoor::closeDoor()
{
  curState = BossDoor::CLOSING;
  doorOpenDelay = Clock::clockTicks;
  Sounds::mm_sounds->play(DOOR);
  block();
}

void BossDoor::block()
{
  if (config.orientation == 0)
  {
    int xd = ((int)x) / mm_graphs_defs::TILE_SIZE;
    int yd = ((int)y) / mm_graphs_defs::TILE_SIZE;
    for (int i = 0; i < config.totalSize; ++i, yd++)
    {
      cur_stage->setTileAction(xd, yd, mm_tile_actions::TILE_SOLID);
    }
  }
  else
  {
    int xd = ((int)x) / mm_graphs_defs::TILE_SIZE;
    int yd = ((int)y) / mm_graphs_defs::TILE_SIZE;
    for (int i = 0; i < config.totalSize; ++i, xd++)
    {
      cur_stage->setTileAction(xd, yd, mm_tile_actions::TILE_SOLID);
    }
  }
}

void BossDoor::blockPassage()
{
  if (config.orientation == 0)
  {
    int xd = ((int)(x+w))/mm_graphs_defs::TILE_SIZE;
    int yd = ((int)y)/mm_graphs_defs::TILE_SIZE;
    for (int i = 0; i < config.totalSize; ++i, yd++)
    {
      cur_stage->setTileAction(xd, yd, mm_tile_actions::TILE_SOLID);
    }

    yd = ((int)y)/mm_graphs_defs::TILE_SIZE;
    xd = (((int)x)-mm_graphs_defs::TILE_SIZE-config.size*mm_graphs_defs::TILE_SIZE)/mm_graphs_defs::TILE_SIZE;
    cur_stage->defineCameraSector(xd, yd, true);
    cur_stage->defineCameraSector(xcamblock, ycamblock, false);

    hasBeenUsed = true;
  }
  else
  {
    block();

    /*
    switch (config.blockedSide)
    {
      case 0:
        yd = ((int)y)/mm_graphs_defs::TILE_SIZE - (mm_graphs_defs::TILE_SIZE<<1);
      break;
      case 1:
        yd = ((int)y)/mm_graphs_defs::TILE_SIZE + (mm_graphs_defs::TILE_SIZE<<1);
      break;
    }
    xd = (((int)x)-mm_graphs_defs::TILE_SIZE);

    cur_stage->defineCameraSector(xd, yd, true);
    cur_stage->defineCameraSector(xcamblock, ycamblock, false);
    */
    hasBeenUsed = true;
  }
}

void BossDoor::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  BITMAP * curSprTile = this->spriteSheet->getFrame(curSpriteFrame);

  clear_to_color(sprite, MASK_COLOR_24);

  int x, y;
  x = y = 0;
  for (int j = 0; j < config.size; ++j)
  {
    if (config.orientation == 0) { x=curSprTile->w*j; y = 0;}
    else                         { y=curSprTile->h*j; x = 0;}
    for (int i = 0; i < curDoorLevel; ++i)
    {
      if (config.orientation == 0)
      {
        blit(curSprTile, sprite, 0, 0,
          x, y,
          curSprTile->w, curSprTile->h);
        y += curSprTile->h;
      }
      else
      {
        rotate_sprite(sprite, curSprTile, x, y, itofix(64));
        x += curSprTile->w;
      }
    }
  }

  masked_blit(sprite, bmp, 0, 0,
              this->sx, this->sy,
              sprite->w, sprite->h);
}

BITMAP * BossDoor::getFrame()
{
  return sprite;
}

void BossDoor::checkOnCamera()
{
  alive = true;
}

void BossDoor::doLogic()
{
  //static bool doCameraTrick = false;

  if (cur_stage->m_player->alive == false)
  {
    this->alive = false;
    return;
  }

  //if (hasBeenUsed == true && doCameraTrick == false)
  //{
    // Big Big Work Around :)
  //  if (config.cameraHack == false) doCameraTrick = true;

  //  int px = cur_stage->m_player->x;
  //  if (abs(x-px) > (10*mm_graphs_defs::TILE_SIZE))
  //  {
  //    int xd = (x+mm_graphs_defs::TILE_SIZE+config.size*mm_graphs_defs::TILE_SIZE)/mm_graphs_defs::TILE_SIZE;
  //    int yd = y/mm_graphs_defs::TILE_SIZE;
      //cur_stage->defineCameraSector(xd, yd, false);
  //  }
  //}

  switch(curState)
  {
    case BossDoor::OPENING:
    {
      if ((Clock::clockTicks - doorOpenDelay) > 7)
      {
        doorOpenDelay = Clock::clockTicks;
        if (curDoorLevel > 0) curDoorLevel--;
        else 
        { 
          curState = BossDoor::WAITING;
        }
      }
    }
    break;
    case BossDoor::CLOSING:
    {
      if ((Clock::clockTicks - doorOpenDelay) > 7)
      {
        doorOpenDelay = Clock::clockTicks;
        if (curDoorLevel < config.totalSize) curDoorLevel++;
        else
        {
          curState = BossDoor::WAITING;
        }
      }
    }
    break;
  }
}

void BossDoor::doGravitation()
{
  return;
}

void BossDoor::respawn()
{
  life = 1;
  this->x = this->old_x;
  this->y = this->old_y;

  return;
}
