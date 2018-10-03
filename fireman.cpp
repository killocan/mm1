/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include "fireman.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"
#include "spritefiles.h"

Fireman::Fireman(const Stage & stage, int x_map, int y_map, void * param) : Character(stage, mm_spritefiles::FIREMAN_SPRITES)
{
  this->x = x_map;
  this->y = y_map;

  this->vely = 0;
  this->velx = 3;

  overstair     = false;
  isFacingRight = false;

  jumping       = false;
  jumping_right = false;

  curState = Fireman::SHOW_OFF;
  setAnimSeq(Fireman::SHOW_OFF);
  h = getFrameH();
  w = getFrameW();

  life  = 1;
  alive = true;
}

bool Fireman::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Fireman::doLogic()
{
  isFacingRight = (this->x > cur_stage->m_player->x) ? false : true;

  switch(curState)
  {
    case Fireman::DECIDING:
    {
      int decision = rand() % 2;
      switch (decision)
      {
        case 0:
        {
          curState = Fireman::PRE_JUMP;
          setAnimSeq(Fireman::PRE_JUMP);
        }
        break;
        case 1:
        {
          curState = Fireman::PRE_JUMP_ATTACK;
          setAnimSeq(Fireman::PRE_JUMP_ATTACK);
        }
        break;
      }
    }
    break;
    case Fireman::SHOW_OFF:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Fireman::DECIDING;
        setAnimSeq(Fireman::DECIDING);
      }
    }
    break;
    case Fireman::PRE_JUMP:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Fireman::JUMP;
        setAnimSeq(Fireman::JUMP);
      }
    }
    break;
    case Fireman::JUMP:
    {
      if (onground == true && jumping == false)
      {
        bool cycleDone = Character::handleAnimation();
        if (cycleDone == true)
        {
          this->vely = -13;
          jumping = true;
          int i = rand() % 2;
          jumping_right = (bool)i;
        }
      }
      else
      { 
        int tilecoordx, tilecoordy, tiletype;

        if (jumping_right == true)
        {
          if (collisionVer((x + utilX) + velx + utilXLen, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x += velx;
          }
        }
        else
        {
          if (collisionVer((x + utilX) - velx, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x -= velx;
          }
        }

        if (onground == true)
        {
          curState = Fireman::DECIDING;
          setAnimSeq(Fireman::DECIDING);
          jumping = false;
        }
      }
    }
    break;
    case Fireman::PRE_JUMP_ATTACK:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Fireman::JUMP_ATTACK;
        setAnimSeq(Fireman::JUMP_ATTACK);
      }
    }
    break;
    case Fireman::JUMP_ATTACK:
    {
      if (onground == true && jumping == false)
      {
        bool cycleDone = Character::handleAnimation();
        if (cycleDone == true)
        {
          this->vely = -12;
          jumping = true;
        }
      }
      else
      {
        if (onground == true)
        {
          curState = Fireman::ATTACK;
          setAnimSeq(Fireman::ATTACK);
          jumping = false;
        }
      }
    }
    break;
    case Fireman::ATTACK:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        fire();
        curState = Fireman::DECIDING;
        setAnimSeq(Fireman::DECIDING);
      }
    }
    break;
  }
}

void Fireman::fire()
{
  //mm_weapons::createMegaBuster(this, xpos, y+12, vx, vy, bulletOffset);

  return;
}

void Fireman::hit(mm_weapons::weapon_st * pWeapon)
{
  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
    }
    break;
    case mm_weapons::W_MEGA_BUSTER:
    default:
      life--;
    break;
  }

  if (life == 0)
  {
    die();
    alive = false;
  }

  return;
}
