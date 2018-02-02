/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <math.h>
#include <stdio.h>

#include "sniperjoe.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Sniper Joe

SniperJoe::SniperJoe(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::SNIPERJOE_SPRITES)
{
  // Map coords to global world coords
  //this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;
  this->old_y = this->y;



  velx          = -5;
  vely          = 1;
  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;

  //colorOffset = cur_stage->getOffset(mm_spritefiles::SniperJoe_SPRITES);
  setAnimSeq(colorOffset + SniperJoe::APPEARING);
  h = getFrameH();
  w = getFrameW();

  curState = SniperJoe::APPEARING;

  life  = 10;
}

bool SniperJoe::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void SniperJoe::fire()
{
  mm_weapons::createMegaBuster(this, x, y+38, -5.5f, 0, 4);

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void SniperJoe::doLogic()
{
  int tilecoordx, tilecoordy, tiletype;
  int px  = cur_stage->m_player->x;
  int pvx = cur_stage->m_player->velx;

  bool cycleDone = false;
  switch(curState)
  {
    case SniperJoe::APPEARING:
    {
      if (onground == false)
      {
        x += velx;
      }
      else
      {
        velx = pvx;
        curState = SniperJoe::STILL;
        setAnimSeq(colorOffset + SniperJoe::STILL);
      }
    }
    break;
    case SniperJoe::STILL:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int nextAction = rand()%101;

        if (this->x <= px)
        {
          vely = -16;

          velx = pvx;
          if (abs(this->x - px) > 64)
          {
            velx = pvx<<1;
          }

          curState = SniperJoe::JUMPINGAWAY;
          setAnimSeq(colorOffset + SniperJoe::JUMPING);

          break;
        }

        if (nextAction <= 60)
        {
          // Stay still.
          break;
        }
        else if (nextAction > 60 && nextAction <= 75)
        {
          curState = SniperJoe::FIRING;
          setAnimSeq(colorOffset + SniperJoe::FIRING);
          animSeqSize = anim_seqs[curAnimSeq].size();
        }
        else if (nextAction > 75 && nextAction <= 100)
        {
          vely = -16;

          curState = SniperJoe::JUMPING;
          setAnimSeq(colorOffset + SniperJoe::JUMPING);
        }
      }
    }
    break;
    case SniperJoe::FIRING:
    {
      if (this->x <= px)
      {
        vely = -16;

        velx = pvx;
        if (abs(this->x - px) > 64)
        {
          velx = pvx<<1;
        }

        curState = SniperJoe::JUMPINGAWAY;
        setAnimSeq(colorOffset + SniperJoe::JUMPING);

        break;
      }

      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
      }

      if (animSeqSize == 0)
      {
        fire();

        curState = SniperJoe::STILL;
        setAnimSeq(colorOffset + SniperJoe::STILL);
      }
    }
    break;
    case SniperJoe::JUMPING:
    {
      if (this->onground == true)
      {
        curState = SniperJoe::STILL;
        setAnimSeq(colorOffset + SniperJoe::STILL);
      }
    }
    break;
    case SniperJoe::JUMPINGAWAY:
    {
      if(collisionVer(*cur_stage, (x+utilX)+velx+utilXLen, y, tilecoordx, tilecoordy, tiletype) == false)
      {
        x += velx;
      }

      if (this->onground == true)
      {
        curState = SniperJoe::STILL;
        setAnimSeq(colorOffset + SniperJoe::STILL);
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

void SniperJoe::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == SniperJoe::STILL)
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

void SniperJoe::checkOnCamera()
{
  if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w &&
      this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
  {
    if (onCamera == false && alive == false)
    {
      if (this->x > cur_stage->m_player->x)
      {
        alive = true;
      }
    }

    onCamera = true;
  }
  else
  {
    if (alive == true)
    {
      alive    = false;
      respawn();
    }

    if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w &&
        this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
    {
      onCamera = true;
    }
    else
    {
      onCamera = false;
    }
  }
}

void SniperJoe::respawn()
{
  life  = 10;

  //this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->x = this->old_x;
  this->y = this->old_y;

  this->vely = 1;
  this->velx = -5;

  isFacingRight = false;

  resetAnimSeq(colorOffset + SniperJoe::APPEARING);
  curState = SniperJoe::APPEARING;

  return;
}
