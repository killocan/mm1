/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <math.h>
#include <stdio.h>

#include "flea.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Flea

Flea::Flea(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FLEA_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  collided = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FLEA_SPRITES);
  setAnimSeq(colorOffset + Flea::STILL);
  h = getFrameH();

  curState = Flea::STILL;

  life  = 1;
}

bool Flea::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Flea::doLogic()
{
  int tilecoordx, tilecoordy, tiletype;
  int px = cur_stage->m_player->x;

  bool cycleDone = false;
  switch(curState)
  {
    case Flea::STILL:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int dist = abs(px-x);
        int jumpSize = (rand()%2) ? 3 : 6;

        //if (collided == false)
        {
          isFacingRight = x<px;
          velx = (dist > 96) ? (jumpSize*((isFacingRight) ? 1 : -1)) : (jumpSize*((rand()%2) ? -1 : 1));
          isFacingRight = velx > 0;
        }
        //else
        {
        //  velx = (isFacingRight) ? (jumpSize * -1) : jumpSize;
        //  isFacingRight = velx > 0;
        }

        jumpSize = (jumpSize > 3) ? -12 : -14;
        vely = jumpSize;

        curState = Flea::JUMPING;
        setAnimSeq(colorOffset + Flea::JUMPING);
      }
    }
    break;
    case Flea::JUMPING:
    {
      if (onground == true)
      {
        curState = Flea::STILL;
        setAnimSeq(colorOffset + Flea::STILL);
      }
      else
      {
        if (isFacingRight == true)
        {
          if(collisionVer(*cur_stage, (x+utilX)+velx+utilXLen, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x += velx;
            //collided = false;
          }
          else
          {
            //collided = true;
          }
        }
        else
        {
          if(collisionVer(*cur_stage, (x+utilX)+velx, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x += velx;
            //collided = false;
          }
          else
          {
            //collided = true;
          }
        }
      }
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
      /*
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = last_state;
        resetAnimSeq(colorOffset + curState);
      }
      */
    }
    break;
  }
}

void Flea::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
      /*
      if (curState != Character::FREEZE)
      {
        last_state = curState;
        curState  = Character::FREEZE;
        curAnimFrameDuration  = 100; // freeze in the current sprite.
      }
      */
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

void Flea::respawn()
{
  life  = 1;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  vely = 1;
  velx = 0;

  isFacingRight = true;

  resetAnimSeq(colorOffset + Flea::STILL);
  curState = Flea::STILL;  

  return;
}
