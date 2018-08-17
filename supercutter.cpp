/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "supercutter.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Super Cutter

SuperCutter::SuperCutter(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::SUPERCUTTER_SPRITES)
{
  // Map coords to global world coords
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  //this->x += 16;

  xDestination = 0;

  this->old_x = this->x;
  this->old_y = this->y;



  velx          = 0;
  overstair     = false;
  isFacingRight = false;

  setAnimSeq(SuperCutter::GOING_UP);

  curState = SuperCutter::GOING_UP;

  isFlying = false;

  life  = 5;

  alive = false;
}

void SuperCutter::move()
{
  if (xDestination == 0) return;

  if (isFacingRight == true)
  {
    tempX += tempVelX;
  }
  else
  {
    tempX -= tempVelX;
  }

  this->x = (int) tempX;
}

bool SuperCutter::collisionHor(const Stage & stage,   int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype)
{
  return false;
}

void SuperCutter::doLogic()
{
  int dist = abs(cur_stage->m_player->x - this->old_x);

  if (xDestination == 0)
  {
    if (dist < 160)
    {
      alive = true;
      xDestination  = cur_stage->m_player->x;
      isFacingRight = this->x < xDestination;
      tempVelX = (float)(dist/32);
      if (isFacingRight == true) tempVelX+=.5f;
    }
    else
    {
      alive = false;
    }
  }

  if ((this->alive == true) && (this->y > GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->w))
  {
    if (dist < 160)
    {
      respawn();
    }
  }

  switch(curState)
  {
    case SuperCutter::GOING_UP:
    {
      move();

      if (isFlying == false)
      {
        isFlying = true;
        vely = -15;
      }

      if (vely > -15)
      {
        curState = SuperCutter::CUTTING;
        setAnimSeq(SuperCutter::CUTTING);
      }
    }
    break;
    case SuperCutter::CUTTING:
    {
      move();
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void SuperCutter::hit(mm_weapons::weapon_st * pWeapon)
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

  if (life <= 0)
  {
    die();
    respawn();
  }

  return;
}

void SuperCutter::checkOnCamera()
{
  if (this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
  {
    int dist = abs(cur_stage->m_player->x - this->old_x);
    if (dist < 160)
    {
      if (alive == false)
      {
        respawn();
      }
    }
  }
}

void SuperCutter::respawn()
{
  life  = 5;
  alive = true;

  this->vely = 0;
  this->isFlying = false;

  this->x = this->old_x;
  this->y = this->old_y;

  tempX = this->x;

  resetAnimSeq(SuperCutter::GOING_UP);
  curState = SuperCutter::GOING_UP;

  xDestination = 0;

  return;
}
