/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include "gutsman.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Gutsman

Gutsman::Gutsman(const Stage & stage, int x_map, int y_map, void * param) : Character(stage, mm_spritefiles::GUTSMAN_SPRITES)
{
  this->x = x_map;
  this->y = y_map;

  this->vely = 0;
  this->velx = 3;

  overstair     = false;
  isFacingRight = false;

  jumping       = false;
  jumping_right = false;

  curState = Gutsman::SHOW_OFF;
  setAnimSeq(Gutsman::SHOW_OFF);
  h = getFrameH();
  w = getFrameW();

  life  = 1;
  alive = true;
}

bool Gutsman::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Gutsman::doLogic()
{
  isFacingRight = (this->x > cur_stage->m_player->x) ? false : true;

  switch(curState)
  {
    case Gutsman::DECIDING:
    {
      int decision = rand() % 2;
      switch (decision)
      {
        case 0:
        {
          curState = Gutsman::PRE_JUMP;
          setAnimSeq(Gutsman::PRE_JUMP);
        }
        break;
        case 1:
        {
          curState = Gutsman::PRE_JUMP_ATTACK;
          setAnimSeq(Gutsman::PRE_JUMP_ATTACK);
        }
        break;
      }
    }
    break;
    case Gutsman::SHOW_OFF:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::DECIDING;
        setAnimSeq(Gutsman::DECIDING);
      }
    }
    break;
    case Gutsman::PRE_JUMP:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::JUMP;
        setAnimSeq(Gutsman::JUMP);
      }
    }
    break;
    case Gutsman::JUMP:
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
          curState = Gutsman::DECIDING;
          setAnimSeq(Gutsman::DECIDING);
          jumping = false;

          GlobalGameState::earthquake = true;
        }
      }
    }
    break;
    case Gutsman::PRE_JUMP_ATTACK:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::JUMP_ATTACK;
        setAnimSeq(Gutsman::JUMP_ATTACK);
      }
    }
    break;
    case Gutsman::JUMP_ATTACK:
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
          curState = Gutsman::ATTACK;
          setAnimSeq(Gutsman::ATTACK);
          jumping = false;
        }
      }
    }
    break;
    case Gutsman::ATTACK:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        fire();
        curState = Gutsman::DECIDING;
        setAnimSeq(Gutsman::DECIDING);
      }
    }
    break;
  }
}

void Gutsman::fire()
{
  //mm_weapons::createMegaBuster(this, xpos, y+12, vx, vy, bulletOffset);

  return;
}

void Gutsman::hit(mm_weapons::weapon_st * pWeapon)
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

//void Gutsman::respawn()
//{
//  return;
//}
