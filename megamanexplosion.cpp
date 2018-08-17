/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "megamanexplosion.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//MegamanExplosion

MegamanExplosion::MegamanExplosion(const Stage & stage, int x, int y, void * param) : Character(stage, mm_spritefiles::MEGAMANEXPLOSION_SPRITES)
{
  // Coords already in world coord format
  this->x = x;
  this->y = y;



  setAnimSeq(MegamanExplosion::EXPLODING);
  curState = MegamanExplosion::EXPLODING;
  h = getFrameH();
  w = getFrameW();

  alive = true;
  bOneCicle = (param != NULL) ? (*((bool *)param)) : false;
}

void MegamanExplosion::doLogic()
{
  switch(curState)
  {
    case MegamanExplosion::EXPLODING:
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

      if (((this->sx+this->w) < 0) ||
          ((this->sy+this->h) < 0) ||
          ((this->sx)         > mm_graphs_defs::UTIL_W) ||
          ((this->sy)         > mm_graphs_defs::UTIL_H))
      {
        alive = false;
      }
    }
    break;
  }
}

void MegamanExplosion::doGravitation()
{
  return;
}

//bool MegamanExplosion::handleAnimation(bool * bAnimationEnded)
//{
//  return Character::handleAnimation(bAnimationEnded);
//}

void MegamanExplosion::checkOnCamera()
{
  return;
}
