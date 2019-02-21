/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cmath>
#include <cstdio>

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

ExplosionLittle::ExplosionLittle(const Stage & stage, int x, int y, void * param) : Character(stage, mm_spritefiles::EXPLOSIONLITTLE_SPRITES)
{
  this->x = x;
  this->y = y;

  bOneCicle = (param != NULL) ? (*((bool *)param)) : true;
  int offset = bOneCicle ? ExplosionLittle::EXPLODING : ExplosionLittle::BOSS_DIE;
  setAnimSeq(offset);
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
        this->x += velx;
        this->y += vely;

        if (bOneCicle == true)
        {
          alive = false;
          break;
        }
      }

      if (((this->sx + this->w) < 0) ||
        ((this->sy + this->h) < 0) ||
        ((this->sx) > mm_graphs_defs::UTIL_W) ||
        ((this->sy) > mm_graphs_defs::UTIL_H))
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

void ExplosionLittle::checkOnCamera()
{
  return;
}
