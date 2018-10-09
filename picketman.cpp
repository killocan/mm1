/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <math.h>
#include <stdio.h>

#include "picketman.h"
#include "mm_math.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Picket Man Hammer
PicketMan::PicketManHammer::PicketManHammer(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::PICKETMANHAMMER_SPRITES)
{
  this->x = x;
  this->y = y;

  alive = true;
  canCollidePlayer = true;

  setAnimSeq(PicketManHammer::MOVING);
  curState = PicketManHammer::MOVING;
}

bool PicketMan::PicketManHammer::collisionHor(int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype)
{
  return false;
}

void PicketMan::PicketManHammer::doLogic()
{
  this->x += this->velx;
  Character::handleAnimation();
  if (this->sy >= mm_graphs_defs::UTIL_H || (cur_stage->horz_scroll == true))
  {
    alive = false;
  }
}

void PicketMan::PicketManHammer::checkOnCamera()
{
  return;
}

//Picket Man

PicketMan::PicketMan(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::PICKETMAN_SPRITES)
{
  // Map coords to global world coords
  //this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;
  this->old_y = this->y;

  velx = -5;
  if (cur_stage->m_player->x > this->x)
    velx *= -1;

  vely          = 1;
  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;

  //colorOffset = cur_stage->getOffset(mm_spritefiles::PicketMan_SPRITES);
  setAnimSeq(colorOffset + PicketMan::APPEARING);
  h = getFrameH();
  w = getFrameW();

  curState = PicketMan::APPEARING;

  life = 10;
}

bool PicketMan::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void PicketMan::fire()
{
  int centerX = this->x+w/2;
  int centerY = this->y+h/2;
  int px      = cur_stage->m_player->x - cur_stage->m_player->utilX;
  int py      = cur_stage->m_player->y;

  // Handle the work around ahwuhawuahw
  if (this->x+10 > px) px+=cur_stage->m_player->getFrameW();
  else if (this->y != py) px+=cur_stage->m_player->getFrameW()+10;

  double velx, vely;
  MM_Math::CalculateVector2D(centerX, centerY, px, py+20, velx, vely);

  if (centerX > cur_stage->m_player->x+cur_stage->m_player->utilXLen)
    velx *= -1.0f;

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);
  Character * pHammer = cur_stage->createCharacter(mm_tile_actions::PICKETMAN_HAMMER_CHAR, this->x, this->y, (int)velx, (int)-vely);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pHammer);
}

void PicketMan::doLogic()
{
  //int tilecoordx, tilecoordy, tiletype;
  int px  = cur_stage->m_player->x;

  isFacingRight = px > x;

  bool cycleDone = false;
  switch(curState)
  {
    case PicketMan::APPEARING:
    {
      if (onground == false)
      {
        x += velx;
      }
      else
      {
        curState = PicketMan::STILL;
        setAnimSeq(colorOffset + PicketMan::STILL);
      }
    }
    break;
    case PicketMan::STILL:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int nextAction = rand()%100;

        if (nextAction <= 85)
        {
          // Stay still.
          break;
        }
        else if (nextAction > 85)
        {
          howManyToFire = ((rand()%100)<50) ? 5 : 10;
          curState = PicketMan::FIRING;
          setAnimSeq(colorOffset + PicketMan::FIRING);
          animSeqSize = anim_seqs[curAnimSeq].size();
        }
      }
    }
    break;
    case PicketMan::FIRING:
    {
      //if (cur_stage->m_player->alive == false)
      //{
      //  break;
      //}

      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
        if (animSeqSize == 1) fire();
      }

      if (animSeqSize == 0)
      {
        curState = PicketMan::FIRING;
        setAnimSeq(colorOffset + PicketMan::FIRING);
        animSeqSize = anim_seqs[curAnimSeq].size();

        --howManyToFire;
        if (howManyToFire == 0)
        {
          curState = PicketMan::STILL;
          setAnimSeq(colorOffset + PicketMan::STILL);          
        }
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

void PicketMan::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == PicketMan::STILL)
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

#if 0
void PicketMan::checkOnCamera(Camera * camera)
{
  if (this->x >= camera->x && this->x <= camera->x+camera->w &&
      this->y >= camera->y && this->y <= camera->y+camera->h)
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

    if (this->x >= camera->x && this->x <= camera->x+camera->w &&
        this->y >= camera->y && this->y <= camera->y+camera->h)
    {
      onCamera = true;
    }
    else
    {
      onCamera = false;
    }
  }
}
#endif

void PicketMan::respawn()
{
  life  = 10;

  //this->x = (GlobalCamera::mm_camera->x + GlobalCamera::mm_camera->w);
  this->x = this->old_x;
  this->y = this->old_y;

  this->vely = 1;
  this->velx = -5;
  if (cur_stage->m_player->x > this->x)
    velx *= -1;

  isFacingRight = false;

  resetAnimSeq(colorOffset + PicketMan::APPEARING);
  curState = PicketMan::APPEARING;

  return;
}
