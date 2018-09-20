/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "footholder.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//FootHolder

FootHolder::FootHolder(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FOOTHOLDER_SPRITES)
{
  this->x = x;
  this->y = y;
  this->old_x = this->x;
  this->old_y = this->y;

  this->velx = 2.0f;
  this->vely = 1.0f;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FOOTHOLDER_SPRITES);
  curState = FootHolder::MOVING;
  setAnimSeq(colorOffset + FootHolder::MOVING);

  h = getFrameH();

  this->isPlatform = true;

  fire_pause = Clock::clockTicks;
}

void FootHolder::doLogic()
{
  this->x += this->velx;
  this->y += this->vely;
  if (this->cur_stage->m_player->onPlatform == true && this->cur_stage->m_player->conPlayer == this)
  {
    this->cur_stage->m_player->x += this->velx;
    this->cur_stage->m_player->y += this->vely;
  }

  if (abs(this->x - this->old_x) > 120)
    this->velx *=-1;
  if (abs(this->y - this->old_y) > 40)
    this->vely *=-1;

  if ((Clock::clockTicks - fire_pause) > 70)
  {
    fire_pause = Clock::clockTicks;

    if (rand()%100 < 50)
    {
      fire();
    }
  }
}

void FootHolder::fire()
{
  mm_weapons::createMegaBuster(this, this->x, this->y + 16, -5.5f, 0.0f, 1);
  mm_weapons::createMegaBuster(this, this->x + this->utilXLen, this->y + 16, 5.5f, 0.0f, 1);

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void FootHolder::doGravitation()
{
  return;
}
