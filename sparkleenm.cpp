/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "sparkleenm.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//SparkleEnm

SparkleEnm::SparkleEnm(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::SPARKLE_ENM_SPRITES)
{
  this->x = x;
  this->y = y;



  canCollidePlayer = true;

  setAnimSeq(SparkleEnm::ACTIVE);
  curState = SparkleEnm::ACTIVE;

  alive = true;
}

void SparkleEnm::doLogic()
{
  switch(curState)
  {
    case SparkleEnm::ACTIVE:
    {
      Character::handleAnimation();
      x += velx;
    }
    break;
  }
}

void SparkleEnm::doGravitation()
{
  return;
}
