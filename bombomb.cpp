/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "bombomb.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Bombomb Fragment

Bombomb::BombombFragment::BombombFragment(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::BOMBOMB_SPRITES)
{
  this->x = x;
  this->y = y;

  alive = true;
  canCollidePlayer = true;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BOMBOMB_SPRITES);
  setAnimSeq(colorOffset + BombombFragment::MOVING);
  curState = BombombFragment::MOVING;
  h = getFrameH();

  dropItem = false;
}

void Bombomb::BombombFragment::doLogic()
{
  this->x += this->velx;

  if (onground == true)
  {
    die();
    Sounds::mm_sounds->play(BOMBOMB_HIT);
    alive = false;
  }
}

void Bombomb::BombombFragment::checkOnCamera()
{
  return;
}

//Bombomb

Bombomb::Bombomb(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::BOMBOMB_SPRITES)
{
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  //this->x += 16;
  this->y = (GlobalCamera::mm_camera->y + GlobalCamera::mm_camera->w);
  this->old_y = this->y;

  vely = 9;

  colorOffset = cur_stage->getOffset(mm_spritefiles::BOMBOMB_SPRITES);
  setAnimSeq(colorOffset + Bombomb::MOVING);
  curState = Bombomb::WAITING;
}

void Bombomb::die()
{
  Character::die();
  // Spawn 4 fragments.
  int ypos = this->y-8;
  Character * pFragment01 = cur_stage->createCharacter(mm_tile_actions::BOMBOMB_FRAGMENT_CHAR, this->x, ypos, -6,-3);
  Character * pFragment02 = cur_stage->createCharacter(mm_tile_actions::BOMBOMB_FRAGMENT_CHAR, this->x, ypos,  6,-3);
  Character * pFragment03 = cur_stage->createCharacter(mm_tile_actions::BOMBOMB_FRAGMENT_CHAR, this->x, ypos, -3,-5);
  Character * pFragment04 = cur_stage->createCharacter(mm_tile_actions::BOMBOMB_FRAGMENT_CHAR, this->x, ypos,  3,-5);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment01);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment02);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment03);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment04);
}

void Bombomb::doLogic()
{
  switch(curState)
  {
    case Bombomb::WAITING:
    {
      bool cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = Bombomb::MOVING;
      }
    }
    break;
    case Bombomb::MOVING:
    {
      if ((this->old_y-this->y) < 400)
      {
        this->y -= this->vely;
      }
      else
      {
        die();
        respawn();
      }
    }
    break;
  }
}

void Bombomb::doGravitation()
{
  return;
}

bool Bombomb::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Bombomb::checkOnCamera()
{
  if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w)
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

void Bombomb::respawn()
{
  this->y = (GlobalCamera::mm_camera->y + GlobalCamera::mm_camera->w);

  resetAnimSeq(colorOffset + Bombomb::MOVING);
  curState = Bombomb::WAITING;

  return;
}
