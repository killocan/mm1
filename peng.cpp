/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include "peng.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Peng

Peng::Peng(const Stage & stage, int x, int y, void * pTemp) : Character(stage, mm_spritefiles::PENG_SPRITES)
{
  this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);

  this->old_y = cur_stage->m_player->y;

  velx          = -3.0f;
  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;
  ang           = M_PI_2;

  setAnimSeq(Peng::WAITING);
  h = getFrameH();
  w = getFrameW();

  curState = Peng::MOVING;

  life  = 1;

  active_sectors = NULL;
  if (pTemp != NULL)
  {
    active_sectors = (int*) pTemp;
  }

  ticks = Clock::clockTicks;
}

Peng::~Peng()
{
  delete [] active_sectors;
}

void Peng::doLogic()
{
  switch(curState)
  {
    case Peng::WAITING:
    {
      if ((Clock::clockTicks - ticks) > 20)
      {
        ticks = Clock::clockTicks;
        curState = Peng::MOVING;
      }
    }
    break;
    case Peng::MOVING:
    {
      tempY = 80.0f * cos(ang);
      ang  += 0.06f;

      // Subtract since screen coords are inverted for Y
      this->y = this->old_y - tempY;
      this->x += this->velx;

      if (((this->sx+this->w) < 0) ||
          ((this->sy+this->h) < 0) ||
          ((this->sy)         > mm_graphs_defs::UTIL_H))
      {
        respawn();
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

void Peng::doGravitation()
{
  return;
}

void Peng::hit(mm_weapons::weapon_st * pWeapon)
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

void Peng::checkOnCamera()
{
  int yd = ((int) (cur_stage->m_player->y + (cur_stage->m_player->h/1.5f))) / mm_graphs_defs::TILE_SIZE;
  int xd = ((int)cur_stage->m_player->x)/mm_graphs_defs::TILE_SIZE;
  int ydesl = yd / mm_graphs_defs::TILES_Y;
  int xdesl = xd / mm_graphs_defs::TILES_X;
  int sector = ydesl*(cur_stage->max_x / mm_graphs_defs::TILES_X) + xdesl;

  bool found = false;
  for (int i = 0; active_sectors[i] != -1; ++i)
  {
    if (active_sectors[i] == sector)
    {
      found = true;
      break;
    }
  }

  if (found && cur_stage->horz_scroll == false)
  {
    alive = true;
  }
  else if (curState != Peng::WAITING)
  {
    respawn();
    alive   = false;
  }
}

void Peng::respawn()
{
  life  = 1;

  int offset = 16;
  if (cur_stage->m_player->sy > 400)
    offset += 2*mm_graphs_defs::TILE_SIZE;

  this->x     = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w) + (2*mm_graphs_defs::TILE_SIZE);
  this->old_y = cur_stage->m_player->y-offset;
  this->y = this->old_y;
  ang = M_PI_2;

  ticks = Clock::clockTicks;
  curState = Peng::WAITING;
}
