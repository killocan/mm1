/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "movingflame.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//MovingFlame

MovingFlame::MovingFlame(const Stage & stage, int x, int y, void * param) : Character(stage, mm_spritefiles::MOVING_FLAME_SPRITES)
{
  this->x = x;
  this->y = y;

  canCollidePlayer = true;

  setAnimSeq(MovingFlame::ACTIVE);
  curState = MovingFlame::ACTIVE;

  alive = true;
}

void MovingFlame::doLogic()
{
  switch(curState)
  {
    case MovingFlame::ACTIVE:
    {
      //x += velx;
    }
    break;
  }
}

void MovingFlame::doGravitation()
{
  return;
}
