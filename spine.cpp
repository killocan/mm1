/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>

#include "spine.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Spine

Spine::Spine(const Stage & stage, int xmap, int ymap) : Character(stage, mm_spritefiles::SPINE_SPRITES)
{
  // Map coords to global world coords
  this->x = xmap;//*mm_graphs_defs::TILE_SIZE;
  this->y = ymap;//*mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::SPINE_SPRITES);
  setAnimSeq(colorOffset + Spine::ROAMING);
  h = getFrameH();

  adjustToFloor();
  this->old_y = this->y;

  findBorders(stage, xmap / mm_graphs_defs::TILE_SIZE, ymap / mm_graphs_defs::TILE_SIZE);

#ifdef DEBUG
  fprintf(stderr,"Spine xmin = [%d] xmax = [%d] colorOffset=[%d]\n", minX, maxX, colorOffset);
#endif

  life  = 1;

  // Just to handle freeze.
  curState = Spine::ROAMING;
}

void Spine::findBorders(const Stage & stage, int x, int y)
{
  int yNext = y+1;

  int xMin;
  for (xMin = x; xMin >= 0; xMin--)
  {
    if ((stage.tileAction(xMin, yNext) != mm_tile_actions::TILE_VOID) &&
        (stage.tileAction(xMin, y)     != mm_tile_actions::TILE_SOLID))
    {
    }
    else
    {
      break;
    }
  }
  minX = (xMin+1)*mm_graphs_defs::TILE_SIZE;

  int xMax;
  for (xMax = x; xMax < stage.max_x; xMax++)
  {
    if ((stage.tileAction(xMax, yNext) != mm_tile_actions::TILE_VOID) &&
        (stage.tileAction(xMax, y)     != mm_tile_actions::TILE_SOLID))
    {
    }
    else
    {
      break;
    }
  }
  maxX = (xMax-1)*mm_graphs_defs::TILE_SIZE;
}

void Spine::doLogic()
{
  if (curState == Character::FREEZE)
  {
    handleFreeze();
    return;
  }

  // am i and megaman standing in the same platform?
  int py = cur_stage->m_player->y;
  int ph = cur_stage->m_player->h;
  int pxl = cur_stage->m_player->x + cur_stage->m_player->utilX + cur_stage->m_player->utilXLen;
  int pxr = cur_stage->m_player->x + cur_stage->m_player->utilX;
  if (((py+ph)/mm_graphs_defs::TILE_SIZE) == (this->y/mm_graphs_defs::TILE_SIZE))
  {
    //maxX + tilesize cause maxX points on the first pixel of last tile.
    //Original game does not test this.
    if ((cur_stage->m_player->onground == true) && (pxl >= minX && pxr <= (maxX+mm_graphs_defs::TILE_SIZE)))
    {
      //Also different from original (there this enemy just blinks).
      curState = Spine::ACTIVE;
      setAnimSeq(colorOffset + Spine::ACTIVE);
      velx = 4;
    }
    else
    {
      curState = Spine::ROAMING;
      setAnimSeq(colorOffset + Spine::ROAMING);
      velx = 1;
    }
  }
  else
  {
    curState = Spine::ROAMING;
    setAnimSeq(colorOffset + Spine::ROAMING);
    velx = 1;
  }

  if (isFacingRight == true)
  {
    if (x < maxX)
    {
      x+=velx;
      if (x > maxX)
      {
        x = maxX;
      }
    }
    else
    {
      isFacingRight = false;
    }
  }
  else
  {
    if (x > minX)
    {
      x-=velx;
      if (x < minX)
      {
        x = minX;
      }
    }
    else
    {
      isFacingRight = true;
    }
  }
}

void Spine::doGravitation()
{
  return;
}

void Spine::adjustToFloor()
{
  int tilecoordx, tilecoordy, tiletype;

  while(true)
  {
    if(collisionHor(*cur_stage, x+utilX, y+vely+h, tilecoordx, tilecoordy, true, tiletype) == false)
    {
      y += vely;
    }
    else
    {
      y+=2;

#ifdef DEBUG
      fprintf(stderr,"FINAL Spine: x=[%f] utilX=[%f] y=[%f] vely=[%f] h=[%f]\n",
                                    x,     utilX,     y,     vely,     h);
#endif
      break;
    }
  }
}

void Spine::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
      pWeapon->alive = false;
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
    }
    break;
    default:
      life--;
    break;
  }

  if (life == 0)
  {
    die();
    alive = false;
    respawn();
  }

  return;
}

void Spine::respawn()
{
  life  = 1;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  isFacingRight = true;

  curState = Spine::ROAMING;
  resetAnimSeq(colorOffset + Spine::ROAMING);

  return;
}
