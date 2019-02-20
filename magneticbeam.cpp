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

void MagneticBeam::checkOnCamera()
{
  if (this->y < GlobalCamera::mm_camera->y || this->y > GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
  {
    alive = false;
  }
}

void MagneticBeam::doLogic()
{
  if (cur_stage->horz_scroll)
  {
    alive = false;
    return;
  }

  bool bAnimEnded;
  handleAnimation(&bAnimEnded);

  if (commited && ((Clock::clockTicks - lifetime) > 220))
  {
	  this->alive = false;
  }

  if (cur_stage->m_player->fireKeyPressed && !commited)
  {
	  calculateXY();
  }
  else if (commited == false)
  {
    MagneticBeamHandler::instance()->commit();
	  if (!cur_stage->m_player->fireKeyPressed) MagneticBeamHandler::instance()->newBeams();
  }

  int ta1 = cur_stage->tileActionUnnormalized(int(x) + 1.0f, int(y) + 1.0f);
  int ta2 = cur_stage->tileActionUnnormalized(int(x) + int(w) - 1.0f, int(y) + 1.0f);
  if (!commited && (ta1 == mm_tile_actions::TILE_SOLID || ta2 == mm_tile_actions::TILE_SOLID))
  {
    int xpos = int(this->x) & 0x1F;
    int offset = ta2 == mm_tile_actions::TILE_SOLID ? -xpos : 0x20-xpos;
	  MagneticBeamHandler::instance()->commit(true, offset);
  }
}

void MagneticBeam::doGravitation()
{
}

void MagneticBeam::commit()
{
  commited = true;
  lifetime = Clock::clockTicks;
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

  if (cur_stage->m_player->isFacingRight)
    this->x += this->w * (this->pos - 1);
  else
	this->x -= this->w * (this->pos - 1);
}
