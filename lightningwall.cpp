/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "lightningwall.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//LightningWall

LightningWall::LightningWall(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::LIGHTNING_WALL_SPRITES)
{
  this->x = x;
  this->y = y;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;

  curState = LightningWall::OFF;
  setAnimSeq(LightningWall::OFF);

  this->old_x = this->x;
  this->old_y = this->y;

  life  = 1;
}

void LightningWall::doLogic()
{
  bool cycleDone = false;
  switch(curState)
  {
    case LightningWall::OFF:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = LightningWall::ON;
        setAnimSeq(LightningWall::ON);

        Sounds::mm_sounds->play(LIGHTNING);
      }
    }
    break;
    case LightningWall::ON:
    {
      bool bAnimOver = false;
      Character::handleAnimation(&bAnimOver);
      if (bAnimOver == true)
      {
        curState = LightningWall::OFF;
        setAnimSeq(LightningWall::OFF);
      }
    }
    break;
  }
}

bool LightningWall::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void LightningWall::doGravitation()
{
  return;
}

void LightningWall::respawn()
{
  life = 1;
  this->x = this->old_x;
  this->y = this->old_y;

  return;
}
