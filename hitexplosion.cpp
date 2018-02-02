/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "hitexplosion.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//HitExplosion

HitExplosion::HitExplosion(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::HITEXPLOSION_SPRITES)
{
  this->x = x;
  this->y = y-16;



  setAnimSeq(HitExplosion::EXPLODING);
  curState = HitExplosion::EXPLODING;

  alive = true;
}

void HitExplosion::doLogic()
{
  switch(curState)
  {
    case HitExplosion::EXPLODING:
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

void HitExplosion::doGravitation()
{
  return;
}

void HitExplosion::checkOnCamera()
{
  return;
}
