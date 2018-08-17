/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "blader.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Blader

Blader::Blader(const Stage & stage, int x, int y, void * pSpecialCase)
  : Character(stage, mm_spritefiles::BLADER_SPRITES)
{
  // Map coords to global world coords
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;
  this->old_y = this->y;



  curPoint = 0;

  velx          = 2;
  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BLADER_SPRITES);
  setAnimSeq(colorOffset + Blader::SEEKING);

  h = getFrameH();

  curState = Blader::SEEKING;

  life  = 1;

  bDontRespawn = false;
  bSpecialCase = false;
  if (pSpecialCase != NULL)
  {
    canCollidePlayer = true;
    canCollideBullet = true;
    bSpecialCase = true;
    alive = true;
    curState = Blader::STAND_STILL;
  }
}

void Blader::doLogic()
{
  int px = cur_stage->m_player->x + cur_stage->m_player->utilX;

  if (bSpecialCase == true)
  {
    Character::handleAnimation();
    checkOnCamera();

    if (cur_stage->m_player->alive == false)
    {
      this->alive = false;
      return;
    }
  }

  switch(curState)
  {
    case Blader::SEEKING:
    {
      if (x < px)
      {
        isFacingRight = true;
        x += velx;
      }
      else
      {
        isFacingRight = false;
        x -= velx;
      }

      int dist = abs(px-x);
      if (dist < mm_graphs_defs::TILE_SIZE*3)
      {
        curState = Blader::CALC_DIVING;
      }
    }
    break;
    case Blader::CALC_DIVING:
    {
      int py = 0;
      if (y > py)
      {
        py = cur_stage->m_player->y + (2*mm_graphs_defs::TILE_SIZE);
        py += (py - y > 300) ? (mm_graphs_defs::TILE_SIZE+10) : 0;
      }
      else
      {
        py = cur_stage->m_player->y;
        py -= (y - py > 300) ? mm_graphs_defs::TILE_SIZE : 0;
      }

      int endX = mm_graphs_defs::TILE_SIZE*6;
      endX *= (isFacingRight ? 1 : -1);

      CTRL_POINTS[0] = x;
      CTRL_POINTS[1] = y;
      CTRL_POINTS[2] = px;
      CTRL_POINTS[3] = py;
      CTRL_POINTS[4] = px;
      CTRL_POINTS[5] = py;
      CTRL_POINTS[6] = x + endX;
      CTRL_POINTS[7] = y;
      calc_spline(CTRL_POINTS, Blader::CURVE_PNTS, curveX, curveY);

      curState = Blader::DIVING;
    }
    break;
    case Blader::DIVING:
    {
      x = curveX[curPoint];
      y = curveY[curPoint];
      curPoint++;

      if (curPoint == Blader::CURVE_PNTS)
      {
        curPoint = 0;
        curState = Blader::SEEKING;
      }
    }
    break;
    case Blader::STAND_STILL:
    {
      ;
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void Blader::doGravitation()
{
  return;
}

void Blader::hit(mm_weapons::weapon_st * pWeapon)
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
    alive = false;
    respawn();
  }

  return;
}

void Blader::respawn()
{
  life  = 1;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  curPoint = 0;

  resetAnimSeq(colorOffset + Blader::SEEKING);
  curState = Blader::SEEKING;

  return;
}

void Blader::checkOnCamera()
{
  if (bSpecialCase == false)
  {
    Character::checkOnCamera();
  }
  else
  {
    if (bDontRespawn == false)
    {
      if (this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
      {
        if (onCamera == false)
        {
          curState = Blader::SEEKING;
        }

        onCamera = true;
      }
      else
      {
        if (alive == true && onCamera == true)
        {
          this->x = this->old_x;
          this->y = this->old_y;
          curState =  Blader::STAND_STILL;
          bDontRespawn = true;
          alive = false;
        }
      }
    }
  }
}
