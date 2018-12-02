/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#include "killerbullet.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Killer Bullet

KillerBullet::KillerBullet(const Stage & stage, int x, int y, void * pTemp) : Character(stage, mm_spritefiles::KILLERBULLET_SPRITES)
{
  this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);

  this->old_y = y;

  velx          = -3.0f;
  overstair     = false;
  isFacingRight = false;
  ang           = M_PI_2;

  colorOffset = cur_stage->getOffset(mm_spritefiles::KILLERBULLET_SPRITES);

  setAnimSeq(colorOffset + KillerBullet::WAITING);
  h = getFrameH();
  w = getFrameW();

  curState = KillerBullet::MOVING;

  life  = 1;

  active_sectors = NULL;
  if (pTemp != NULL)
  {
    active_sectors = (int*) pTemp;
  }

  ticks = Clock::clockTicks;
}

KillerBullet::~KillerBullet()
{
  delete [] active_sectors;
}

void KillerBullet::doLogic()
{
  switch(curState)
  {
    case KillerBullet::WAITING:
    {
      if ((Clock::clockTicks - ticks) > 20)
      {
        ticks = Clock::clockTicks;
        curState = KillerBullet::MOVING;
      }
    }
    break;
    case KillerBullet::MOVING:
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

void KillerBullet::doGravitation()
{
  return;
}

bool KillerBullet::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void KillerBullet::hit(mm_weapons::weapon_st * pWeapon)
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
    mm_weapons::createExplosionParts(cur_stage, x, y, false);
    respawn();
  }

  return;
}

void KillerBullet::checkOnCamera()
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
  else if (curState != KillerBullet::WAITING)
  {
    respawn();
    alive   = false;
  }
}

void KillerBullet::respawn()
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
  curState = KillerBullet::WAITING;
}
