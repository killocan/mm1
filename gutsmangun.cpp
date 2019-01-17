/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cmath>
#include <cstdio>

#include "gutsmangun.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "gutsmangunmanager.h"
#include "collision.h"
#include "spritefiles.h"

//Gutsman Rock Gun

float GutsmanGun::getXDest()
{
  float offsetx = thrower == cur_stage->m_player ? 0.0f : 5.0f;
  float dest_x = thrower->x + offsetx;

  return dest_x;
}

float GutsmanGun::getYDest()
{
  float offsety = thrower == cur_stage->m_player ? 21.0f : 7.0f;
  float dest_y = thrower->y - this->h + offsety;

  if (thrower == cur_stage->m_player)
  {
    if (!thrower->onground)
      dest_y -= 10.0f;
  }

  return dest_y;
}

GutsmanGun::GutsmanGun(const Stage & stage, int x, int y, void * param) : Character(stage, mm_spritefiles::GUTSMANROCK_SPRITES)
{
  this->old_x = this->x = x;
  this->old_y = this->y = y;

  isFacingRight = true;
  overstair = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::GUTSMANROCK_SPRITES);
  // set sprite to the pickup one
  setAnimSeq(colorOffset + 1);
  h = getFrameH();
  w = getFrameW();

  alive = true;
  life = 1;

  curState = GutsmanGun::MOVING;

  thrower = (Character *) param;

  float dest_x = getXDest();
  float dest_y = getYDest();

  if (thrower == cur_stage->m_player)
  {
    this->x += (dest_x - this->x) / 2;
    this->y += (dest_y - this->y) / 2;
  }
  ticks = Clock::clockTicks;

  GutsmanGunManager::instance()->addRock(this);
}

void GutsmanGun::launch()
{
  curState = LAUNCH;
}

void GutsmanGun::moveToMegaman()
{
  if ((Clock::clockTicks - ticks) > 2)
  {
    ticks = Clock::clockTicks;
    
    this->x = getXDest();
    this->y = getYDest();

    this->curState = GutsmanGun::ATTACHED_TO;
  }
}

void GutsmanGun::moveToThrower()
{
  if ((Clock::clockTicks - ticks) > 2)
  {
    ticks = Clock::clockTicks;

    if (this->y+ 22.0f >= thrower->y - this->h + 7)
    {
      this->curState = GutsmanGun::ATTACHED_TO;
    }
    else
    {
      this->y += 22.0f;
    }
  }
}

void GutsmanGun::calcAcceleration()
{
  if (thrower == cur_stage->m_player)
  {
    this->vely = -5.5f;
    this->velx = 11.0f;
    this->isFacingRight = thrower->isFacingRight;
  }
  else
  {
    this->vely = -6.0f;
    this->velx = 12.0f;
    this->isFacingRight = thrower->isFacingRight;
  }
}

void GutsmanGun::doLogic()
{
  switch (curState)
  {
  case GutsmanGun::MOVING:
    if (thrower == cur_stage->m_player)
      moveToMegaman();
    else
      moveToThrower();
  break;
  case GutsmanGun::ATTACHED_TO:
    this->x = getXDest();
    this->y = getYDest();
    this->isFacingRight = !thrower->isFacingRight;
  break;
  case GutsmanGun::LAUNCH:
    if (this->velx != 0.0f)
    {
      if (onground == true || checkCollision())
      {
        Sounds::mm_sounds->play(SUPER_ARM);
        curState = GutsmanGun::FRAGMENT;
      }
      else
      {
        int tilecoordx, tilecoordy, tiletype;
        float xoffset = (this->isFacingRight) ? (x + w + velx) : (x - velx);
        if (collisionVer(xoffset, y, tilecoordx, tilecoordy, tiletype) == false)
        {
          x = (this->isFacingRight) ? (x+velx) : (x-velx);
        }
        else
        {
          Sounds::mm_sounds->play(SUPER_ARM);
          curState = GutsmanGun::FRAGMENT;
        }
      }
    }
    else
    {
      calcAcceleration();
    }
  break;
  case GutsmanGun::FRAGMENT:
    mm_weapons::createGutsmanRock(this);
    curState = GutsmanGun::DEAD;
  break;
  case GutsmanGun::DEAD:
    die();
  break;
  }
}

void GutsmanGun::die()
{
  alive = false;
  life = 0;
}

void GutsmanGun::doGravitation()
{
  if (curState != GutsmanGun::LAUNCH)
    return;

  Character::doGravitation();
}

void GutsmanGun::checkOnCamera()
{
  if (life > 0 && (curState == GutsmanGun::ATTACHED_TO || curState == GutsmanGun::MOVING))
  {
    alive = true;
  }
  else
  {
    Character::checkOnCamera();
  }
}

bool GutsmanGun::checkCollision()
{
  Character * curr_character = NULL;
  std::vector<Character *>::iterator it;

  for (it = CurrentCharacterList::mm_characterLst->begin();
       it != CurrentCharacterList::mm_characterLst->end(); ++it)
  {
    curr_character = *it;
    if (curr_character != thrower && curr_character != this)
    {
      if (curr_character->alive == true)
      {
        if (Collision::pixelCollision((int)this->x, (int)this->y, this->getFrame(),
            curr_character->x, curr_character->y, curr_character->getFrame(),
            &this->xcol, &this->ycol) == true)
        {
          mm_weapons::weapon_st pWeapon;
          pWeapon.weapon = mm_weapons::W_GUTSMAN_GUN;
          
          curr_character->hit(&pWeapon);

          return true;
        }
      }
    }
  }

  return false;
}
