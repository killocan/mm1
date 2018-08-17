/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "moveplatform.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//MovePlatform

MovePlatform::MovePlatform(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::MOVING_PLATFORM_SPRITES)
{
  this->x = x;
  this->y = y-8;
  this->old_x = this->x;

  velx = 2;

  colorOffset = cur_stage->getOffset(mm_spritefiles::MOVING_PLATFORM_SPRITES);
  curState = MovePlatform::HOLDING_MOVE;
  setAnimSeq(colorOffset + MovePlatform::HOLDING_MOVE);
  h = getFrameH();
}

bool MovePlatform::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void MovePlatform::checkOnCamera()
{
  alive = true;
}

void MovePlatform::doLogic()
{
  if (cur_stage->hasMovingPlatform == false)
    return;

  int xpos = velx > 0 ? x+16 : x-8;
  unsigned char tileNumber = cur_stage->tileNumberUnnormalized(xpos, y+8);
  bool bAnimEnded = false;
  switch(curState)
  {
    case MovePlatform::HOLDING_MOVE:
    {
      if (tileNumber == cur_stage->platformTiles[0] || tileNumber == cur_stage->platformTiles[1])
      {
        this->isPlatform = true;
        x += velx;
        if (cur_stage->m_player->onPlatform == true && cur_stage->m_player->conPlayer == this)
        {
          cur_stage->m_player->x += velx;
        }
      }
      else if (tileNumber == cur_stage->platformTiles[2])
      {
        //cur_stage->m_player->onPlatform = false;
        this->isPlatform = false;
        curState = MovePlatform::DOWN_ANIM;
        setAnimSeq(colorOffset + MovePlatform::DOWN_ANIM);
      }
      else
      {
        velx *= -1;
        x+=velx;
      }
    }
    break;
    case MovePlatform::DOWN_ANIM:
    {
      Character::handleAnimation(&bAnimEnded);
      if (bAnimEnded == true)
      {
        curState = MovePlatform::DOWM_MOVE;
        setAnimSeq(colorOffset + MovePlatform::DOWM_MOVE);
      }
      else
      {
        x += velx;
      }
    }
    break;
    case MovePlatform::UP_ANIM:
    {
      Character::handleAnimation(&bAnimEnded);
      if (bAnimEnded == true)
      {
        curState = MovePlatform::HOLDING_MOVE;
        setAnimSeq(colorOffset + MovePlatform::HOLDING_MOVE);
      }
      else
      {
        x += velx;
      }
    }
    break;
    case MovePlatform::DOWM_MOVE:
    {
      if (tileNumber == cur_stage->platformTiles[2])
      {
        x += velx;
      }
      else if (tileNumber == cur_stage->platformTiles[0])
      {
        curState = MovePlatform::UP_ANIM;
        setAnimSeq(colorOffset + MovePlatform::UP_ANIM);
      }
    }
    break;
  }
}

void MovePlatform::doGravitation()
{
  return;
}

void MovePlatform::respawn()
{
  this->x = this->old_x;
  curState = MovePlatform::HOLDING_MOVE;
  resetAnimSeq(colorOffset + MovePlatform::HOLDING_MOVE);
  return;
}
