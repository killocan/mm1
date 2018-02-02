/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "firebolt.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//FireBolt

FireBolt::FireBolt(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FIRE_H_WALL_SPRITES)
{
  this->x = x;
  this->y = y;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;

  curState = FireBolt::ON;
  setAnimSeq(FireBolt::ON);
 
  this->old_x = this->x;
  this->old_y = this->y;

  life  = 1;
}

void FireBolt::doLogic()
{
  ;
}

void FireBolt::doGravitation()
{
  return;
}

void FireBolt::respawn()
{
  life = 1;
  this->x = this->old_x;
  this->y = this->old_y;

  return;
}
