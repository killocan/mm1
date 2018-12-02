/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "sparkle.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Sparkle

Sparkle::Sparkle(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::SPARKLE_SPRITES)
{
  this->x = x;
  this->y = y;



  vely          = 1;
  overstair     = false;
  isFacingRight = true;

  curState = Sparkle::ON;
  setAnimSeq(Sparkle::ON);
 
  adjustCoord();

  this->old_x = this->x;
  this->old_y = this->y;

  life  = 1;
}

void Sparkle::doLogic()
{
  ;
}

void Sparkle::doGravitation()
{
  return;
}

void Sparkle::adjustCoord()
{
  this->x += 16;
  this->y += 8;
}

void Sparkle::respawn()
{
  life = 1;
  this->x = this->old_x;
  this->y = this->old_y;

  return;
}
