/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "explosionlittle.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//ExplosionLittle

ExplosionLittle::ExplosionLittle(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::EXPLOSIONLITTLE_SPRITES)
{
  // Coords already in world coord format
  this->x = x;
  this->y = y;



  setAnimSeq(ExplosionLittle::EXPLODING);
  curState = ExplosionLittle::EXPLODING;

  alive = true;
}

void ExplosionLittle::doLogic()
{
  switch(curState)
  {
    case ExplosionLittle::EXPLODING:
    {
      bool bAnimEnded = false;
      Character::handleAnimation(&bAnimEnded);
      if (bAnimEnded == true)
      {
        alive = false;
      }
    }
    break;
  }
}

void ExplosionLittle::doGravitation()
{
  return;
}

//bool ExplosionLittle::handleAnimation(bool * bAnimationEnded)
//{
//  return Character::handleAnimation(bAnimationEnded);
//}

void ExplosionLittle::checkOnCamera()
{
  return;
}
