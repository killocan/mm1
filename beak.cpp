/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>

#include "beak.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Beak

Beak::Beak(const Stage & stage, int x_map, int y_map) : Character(stage, mm_spritefiles::BEAK_SPRITES)
{
  // Map coords to global world coords
  this->x = x_map;//*mm_graphs_defs::TILE_SIZE;
  this->y = y_map;//*mm_graphs_defs::TILE_SIZE;

  this->old_y = this->y;

  //old_map_x = x_map;
  //old_map_y = y_map;

  //

  vely = velx   = 0;
  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  fireCount     = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BEAK_SPRITES);
  curState    = Beak::SLEEPING;
  setAnimSeq(colorOffset + Beak::SLEEPING);
  h = getFrameH();

  switch(colorOffset)
  {
    case 0:
      bulletOffset = 2;
    break;
    case 4:
      bulletOffset = 1;
    break;
    case 8:
      bulletOffset = 3;
    break;
  }

  adjustToWall(x_map / mm_graphs_defs::TILE_SIZE, y_map / mm_graphs_defs::TILE_SIZE);
  
  this->old_x = this->x;

  life  = 1;
}

bool Beak::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Beak::doLogic()
{
  bool cycleDone = false;

  switch(curState)
  {
    case Beak::SLEEPING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = Beak::OPENING;
        setAnimSeq(colorOffset + Beak::OPENING);
        animSeqSize = anim_seqs[curAnimSeq].size();
      }
    }
    break;
    case Beak::OPENING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
      }

      if (animSeqSize == 0)
      {
        curState = Beak::FIRING;
        setAnimSeq(colorOffset + Beak::FIRING);

        // First bullet is fired right away.
        fire();
        animSeqSize = anim_seqs[curAnimSeq].size();
      }
    }
    break;
    case Beak::FIRING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {    
        fireCount++;
        fire();

        // 4 bullets (these 3 plus the first one in opening state)
        if (fireCount == 3)
        {
          fireCount = 0;

          curState = Beak::CLOSING;
          setAnimSeq(colorOffset + Beak::CLOSING);
          animSeqSize = anim_seqs[curAnimSeq].size();
        }
      }
    }
    break;
    case Beak::CLOSING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
      }

      if (animSeqSize == 0)
      {
        curState = Beak::SLEEPING;
        setAnimSeq(colorOffset + Beak::SLEEPING);
      }
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void Beak::fire()
{
  static float vy_array[4] = {-5.5f, -1.0f, 1.0f, 5.5f};
  float vx = isFacingRight ? 5.5f : -5.5f;
  float vy = vy_array[fireCount];

  int xpos = isFacingRight ? x+28 : x-2;

  mm_weapons::createMegaBuster(this, xpos, y+12, vx, vy, bulletOffset);

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void Beak::doGravitation()
{
  return;
}

void Beak::adjustToWall(int x_map, int y_map)
{
  int xMin;
  for (xMin = x_map; xMin >= 0; xMin--)
  {
    if (cur_stage->tileAction(xMin, y_map) == mm_tile_actions::TILE_SOLID)
    {
      break;
    }
  }
  int xMax;
  for (xMax = x_map; xMax < cur_stage->max_x; xMax++)
  {
    if (cur_stage->tileAction(xMax, y_map) == mm_tile_actions::TILE_SOLID)
    {
      break;
    }
  }

  // which wall is closer, left or right?
  closer = (abs(xMin-x_map) < abs(xMax-x_map)) ? 0 : 1;
  isFacingRight = (closer == 0);

  int tilecoordx, tilecoordy, tiletype;
  int dir = (closer == 0) ? -1 : 1;
  int w   = (closer == 0) ?  0 : utilXLen;

  while(true)
  {
    if(collisionVer(*cur_stage, (x+utilX)+velx+w, y, tilecoordx, tilecoordy, tiletype) == false)
    {
      x += dir;
    }
    else
    {
      // Left Wall, adjust one pixel.
      if (dir == -1)
      {
        x += 1;
      }

#ifdef DEBUG
      fprintf(stderr,"FINAL Beak: x=[%d] utilX=[%d] y=[%d] vely=[%d] h=[%d]\n",
                                    x,     utilX,     y,     vely,     h);
#endif
      break;
    }
  }
}

void Beak::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == Beak::SLEEPING)
  {
    Sounds::mm_sounds->play(HIT_SHIELD);
    if (pWeapon->weapon == mm_weapons::W_MEGA_BUSTER) pWeapon->alive = false;
    return;
  }
  else
  {
    Sounds::mm_sounds->play(HIT_ENEMY);
  }

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
    }
    break;
    case mm_weapons::W_MEGA_BUSTER:
      pWeapon->alive = false;
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

void Beak::respawn()
{
  life  = 1;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  isFacingRight = (closer == 0);
  fireCount     = 0;

  curState   = Beak::SLEEPING;
  resetAnimSeq(colorOffset + Beak::SLEEPING);
  //adjustToWall(old_map_x, old_map_y);

  return;
}
