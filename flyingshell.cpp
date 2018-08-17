/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "flyingshell.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Flying Shell

FlyingShell::FlyingShell(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FLYINGSHELL_SPRITES)
{
  // Map coords to global world coords
  //this->x = x*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  //TODO: need camera x to place it always offscreen.

  this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  //this->old_x = this->x;
  this->old_y = this->y;



  velx          = -2;
  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;

  setAnimSeq(FlyingShell::CLOSED);
  h = getFrameH();
  w = getFrameW();

  curState = FlyingShell::CLOSED;

  life  = 1;
}

void FlyingShell::doLogic()
{
  bool cycleDone = false;

  switch(curState)
  {
    case FlyingShell::RESTING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = FlyingShell::CLOSED;
        setAnimSeq(FlyingShell::CLOSED);
      }
    }
    break;
    case FlyingShell::CLOSED:
    {
      this->x += this->velx;
      if ((this->sx+this->w) < 0)
      {
        curState = FlyingShell::RESTING;
        setAnimSeq(FlyingShell::RESTING);
        respawn();
      }
      else
      {
        cycleDone = Character::handleAnimation();
        if (cycleDone == true)
        {
          fire();

          curState = FlyingShell::FIRING;
          setAnimSeq(FlyingShell::FIRING);
        }
      }
    }
    break;
    case FlyingShell::FIRING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = FlyingShell::CLOSED;
        setAnimSeq(FlyingShell::CLOSED);
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

bool FlyingShell::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void FlyingShell::doGravitation()
{
  return;
}

void FlyingShell::fire()
{
  int xpos = (this->x + w/2)-6;
  int ypos = (this->y + h/2)-6;

  mm_weapons::createMegaBuster(this, xpos, ypos,  3.7f, 0, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, -3.7f, 0, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, 0,  3.7f, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, 0, -3.7f, 1);

  mm_weapons::createMegaBuster(this, xpos, ypos, 3.0f,  3.0f, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, 3.0f, -3.0f, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, -3.0f,  3.0f, 1);
  mm_weapons::createMegaBuster(this, xpos, ypos, -3.0f, -3.0f, 1);

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void FlyingShell::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == FlyingShell::CLOSED)
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

  if (life <= 0)
  {
    die();
    respawn();
  }

  return;
}

void FlyingShell::checkOnCamera()
{
  if (this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
  {
    if (alive == false)
    {
      respawn();
    }

    alive = true;
  }
  else
  {
    alive = false;
  }
}

void FlyingShell::respawn()
{
  life  = 1;
  alive = true;

  //this->x = this->old_x;
  this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->y = this->old_y;

  resetAnimSeq(FlyingShell::RESTING);
  curState = FlyingShell::RESTING;

  return;
}
