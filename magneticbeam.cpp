/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "magneticbeam.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "magneticbeamhandler.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"
#include "spritefiles.h"


MagneticBeam::MagneticBeam(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::WEAPONS_MAGNETIC)
{
  this->pos = MagneticBeamHandler::instance()->addBeam(this);

  this->commited = false;

  this->x = x;
  this->y = y;
  this->old_x = this->x;
  this->old_y = this->y;

  colorOffset = cur_stage->getOffset(mm_spritefiles::WEAPONS_MAGNETIC);
  curState = MagneticBeam::STATIC;
  setAnimSeq(colorOffset + MagneticBeam::STATIC);

  this->h = getFrameH();
  this->w = getFrameW();

  this->isPlatform = true;
  this->alive = true;
  this->canCollidePlayer = true;

  lifetime = Clock::clockTicks;
}

void MagneticBeam::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy+1);
}

BITMAP * MagneticBeam::getFrame()
{
  return (this->spriteSheet->getFrame(0));
}

void MagneticBeam::doLogic()
{
  bool bAnimEnded;
  handleAnimation(&bAnimEnded);

  if ((Clock::clockTicks - lifetime) > 300)
  {
	this->alive = false;
  }

  if (cur_stage->m_player->fireKeyPressed && !commited)
  {
	calculateXY();
  }
  else
  {
    MagneticBeamHandler::instance()->commit();
    commited = true;
  }
}

void MagneticBeam::doGravitation()
{
}

void MagneticBeam::commit()
{
  commited = true;
  MagneticBeamHandler::instance()->removeBeam(this);
}

void MagneticBeam::calculateXY()
{
  if (cur_stage->m_player->grabstair == false)
  {
    x = (float)(cur_stage->m_player->x + 72);
    if (cur_stage->m_player->isFacingRight == false)
    {
      x -= 115.0f;
    }

    if (cur_stage->m_player->onground == true)
    {
      y = (float)(cur_stage->m_player->y + 32.0f);
    }
    else
    {
      y = (float)(cur_stage->m_player->y + 18.0f);
    }
  }
  else
  {
    x = (float)(cur_stage->m_player->x + 64.0f);
    if (cur_stage->m_player->isFacingRight == false)
    {
      x -= 96.0f;
    }

    y = (float)(cur_stage->m_player->y + 21.0f);
  }

  this->x *= this->w * this->pos;
}
