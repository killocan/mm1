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

Peng::Peng(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::PENG_SPRITES)
{
  this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);



  this->old_y = cur_stage->m_player->y;

  velx          = -3;
  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;
  ang           = M_PI_2;

  setAnimSeq(Peng::MOVING);
  h = getFrameH();
  w = getFrameW();

  curState = Peng::MOVING;

  life  = 1;
}

void Peng::doLogic()
{
  switch(curState)
  {
    case Peng::MOVING:
    {
      tempY = 80.0f * cos(ang);
      ang  += 0.06f;

      // Subtract since screen coords are inverted for Y
      this->y = this->old_y - (int) tempY;
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
  alive = true;
}

void Peng::respawn()
{
  life  = 1;
  alive = true;

  this->x     = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->old_y = cur_stage->m_player->y;
  ang = M_PI_2;

  curState = Peng::MOVING;

  return;
}
