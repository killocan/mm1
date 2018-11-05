/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "movingflame.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//MovingFlameFragment

MovingFlame::MovingFlameFragment::MovingFlameFragment(const Stage & stage, int x, int y, void * param)
  : Character(stage, mm_spritefiles::MOVING_FLAME_SPRITES)
{
  this->x = x;
  this->y = y;
  this->old_x = x;
  this->old_y = y;

  type = 0;
  if (param != 0)
    type = 1;
  else
    vely = 6.0f;

  isFacingRight = false;
  colorOffset   = 0;

  setAnimSeq(MovingFlame::MovingFlameFragment::ACTIVE);
  h = getFrameH();
  w = getFrameW();

  curState = MovingFlame::MovingFlameFragment::DEAD;

  canCollidePlayer = true;

  alive = true;
}

void MovingFlame::MovingFlameFragment::doGravitation()
{
}

void MovingFlame::MovingFlameFragment::logicType0()
{
  this->y += this->vely;

  if (this->y >= 43 * 32)
    respawn();
}

void MovingFlame::MovingFlameFragment::checkOnCamera()
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

void MovingFlame::MovingFlameFragment::doLogic()
{
  if (cur_stage->horz_scroll)
    curState = MovingFlame::MovingFlameFragment::DEAD;

  switch(curState)
  {
    case MovingFlame::MovingFlameFragment::ACTIVE:
    {
      Character::handleAnimation();

      if (type == 0)
        logicType0();
    }
    case MovingFlame::MovingFlameFragment::DEAD:
    {
      canCollidePlayer = false;
    }
    break;
  }
}

void MovingFlame::MovingFlameFragment::respawn()
{
  alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  if (type == 1)
  {
    rotation = 0;
  }

  curState = MovingFlame::MovingFlame::ACTIVE;
}

void MovingFlame::MovingFlameFragment::resetState()
{
  x = old_x;
  y = old_y;

  if (type == 1)
  {
    rotation = 0;
  }

  curState = MovingFlame::MovingFlameFragment::DEAD;
}

void MovingFlame::MovingFlameFragment::drawCharacter(BITMAP * bmp)
{
  if (this->curState != MovingFlame::MovingFlameFragment::DEAD)
  {
    if (type == 0)
    {
      Character::drawCharacter(bmp);
    }
  }
}

//MovingFlame

MovingFlame::MovingFlame(const Stage & stage, int x, int y, void * param)
  : Character(stage, mm_spritefiles::MOVING_FLAME_SPRITES)
{
  this->x = x;
  this->y = y;

  setAnimSeq(MovingFlame::ACTIVE);
  curState = MovingFlame::ACTIVE;

  alive = true;

  internalSprite = create_bitmap(10,10);
#ifdef DEBUG
  clear_to_color(internalSprite, makecol(0,255,0));
#else
  clear_to_color(internalSprite, MASK_COLOR_24);
#endif

  type = 0;
  if (param != NULL)
  {
    type = 1;

    fragments[0] = cur_stage->createCharacter(mm_tile_actions::MOVING_FLAME_FRAGMENT, x,y, 0,0, (void*)1);
    fragments[1] = cur_stage->createCharacter(mm_tile_actions::MOVING_FLAME_FRAGMENT, x,y, 0,0, (void*)1);
    fragments[2] = cur_stage->createCharacter(mm_tile_actions::MOVING_FLAME_FRAGMENT, x,y, 0,0, (void*)1);
    fragments[3] = cur_stage->createCharacter(mm_tile_actions::MOVING_FLAME_FRAGMENT, x,y, 0,0, (void*)1);

    TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[0]);
    TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[1]);
    TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[2]);
    TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[3]);
  }
  else
  {
    int xpos = 108 * 32;
    int ypos = 29 * 32;
    fragments[0] = cur_stage->createCharacter(mm_tile_actions::MOVING_FLAME_FRAGMENT, xpos,ypos, 0,0, NULL);

    TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[0]);
  }
}

MovingFlame::~MovingFlame()
{
  destroy_bitmap(internalSprite);
  internalSprite = NULL;
}

void MovingFlame::doLogic()
{
  switch(curState)
  {
    case MovingFlame::ACTIVE:
    {
      int fragmentsAlive;

      if (type == 0)
      {
        fragmentsAlive = 1;
        if (fragments[0]->curState == MovingFlame::MovingFlameFragment::DEAD)
        {
          --fragmentsAlive;
        }
      }
      else
      {
        fragmentsAlive = 4;
        for (int i = 0; i < 4; ++i)
        {
          if (fragments[i]->curState == MovingFlame::MovingFlameFragment::DEAD)
          {
            --fragmentsAlive;
          }
        }
      }

      if (fragmentsAlive == 0)
      {
        MovingFlame::MovingFlameFragment * cur;
        if (type == 0)
        {
          cur = dynamic_cast <MovingFlame::MovingFlameFragment *> (fragments[0]);
          cur->respawn();
          cur->canCollidePlayer = true;
        }
        else
        {
          for (int i = 0; i < 3; ++i)
          {
            cur = dynamic_cast <MovingFlame::MovingFlameFragment *> (fragments[i]);
            cur->respawn();
            cur->canCollidePlayer = true;
          }
        }
      }
    }
    break;
  }
}

void MovingFlame::doGravitation()
{
  return;
}

void MovingFlame::drawCharacter(BITMAP * bmp)
{
#ifdef DEBUG
  draw_sprite(bmp, internalSprite, sx, sy);
#endif
  return;
}

BITMAP * MovingFlame::getFrame()
{
  return internalSprite;
}

void MovingFlame::respawn()
{
  MovingFlame::MovingFlameFragment * cur;
  if (type == 0)
  {
    cur = dynamic_cast <MovingFlame::MovingFlameFragment *> (fragments[0]);
    cur->resetState();
  }
  else
  {
    for (int i = 0; i < 4; ++i)
    {
      cur = dynamic_cast <MovingFlame::MovingFlameFragment *> (fragments[i]);
      cur->resetState();
    }
  }
}