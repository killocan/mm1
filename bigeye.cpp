/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <math.h>
#include <stdio.h>

#include "bigeye.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Big Eye

BigEye::BigEye(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::BIGEYE_SPRITES)
{
  // Map coords to global world coords
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;
  this->old_y = this->y;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BIGEYE_SPRITES);
  setAnimSeq(colorOffset + BigEye::STILL);
  h = getFrameH();

  curState = BigEye::STILL;

  life  = 20;
}

bool BigEye::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void BigEye::doLogic()
{
  int tilecoordx, tilecoordy, tiletype;
  int px = cur_stage->m_player->x;

  bool cycleDone = false;
  switch(curState)
  {
    case BigEye::STILL:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int jumpSize = (rand()%10 < 6) ? -10 : -15;
        vely = jumpSize;

        isFacingRight = (x < px);

        int dist = abs(px-x);
        velx = (dist > 64) ? 4 : 2;

        curState = BigEye::JUMPING;
        setAnimSeq(colorOffset + BigEye::JUMPING);
      }
    }
    break;
    case BigEye::JUMPING:
    {
      if (onground == true)
      {
        Sounds::mm_sounds->play(BIGEYEJUMP);

        curState = BigEye::STILL;
        setAnimSeq(colorOffset + BigEye::STILL);
      }
      else
      {
        if (isFacingRight == true)
        {
          if(collisionVer(*cur_stage, (x+utilX)+velx+utilXLen, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x += velx;
          }
        }
        else
        {
          if(collisionVer(*cur_stage, (x+utilX)-velx, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x -= velx; 
          }
        }
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

void BigEye::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

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

void BigEye::respawn()
{
  life  = 20;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  this->vely = 1;
  this->velx = 0;

  isFacingRight = true;

  resetAnimSeq(colorOffset + BigEye::STILL);
  curState = BigEye::STILL;

  return;
}
