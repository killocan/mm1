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

// 3 * MM_W while standing.
static const float MAX_X = 136.0f;
// 4 * MM_H while standing.
static const float MAX_Y = 192.0f;
// 2.5 * MM_W while standing.
static const float X_OFFSET = 100.0f;

FootHolder::FootHolder(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FOOTHOLDER_SPRITES)
{
  this->x = x;
  this->y = y + (mm_graphs_defs::TILE_SIZE / 2);
  this->old_x = this->x;
  this->old_y = this->y;

  this->ymin = this->y;
  this->ymax = this->y + MAX_Y;

  this->xmin = this->x - X_OFFSET;
  this->xmax = this->xmin + MAX_X;

  this->midx = this->xmin + ((this->xmax - this->xmin) / 2);

  this->velx = -1.0f;
  this->vely = 1.0f;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FOOTHOLDER_SPRITES);
  curState = FootHolder::MOVING;
  setAnimSeq(colorOffset + FootHolder::MOVING);

  h = getFrameH();
  w = getFrameW();

  this->isPlatform = true;

  crossed_min = crossed_max = false;

  fire_pause = Clock::clockTicks;
}

void FootHolder::doLogic()
{
  if (this->cur_stage->m_player->onPlatform == true && this->cur_stage->m_player->conPlayer == this)
  {
    this->cur_stage->m_player->x += this->velx;
    this->cur_stage->m_player->y += this->vely;
  }

  bool decide = false;

  if (this->velx < 0.0f && crossed_min == false)
  {
    if (this->x+(this->w/2) < this->midx)
    {
      crossed_max = false;
      crossed_min = true;
      decide = true;
    }
  }
  else if (this->velx > 0.0f && crossed_max == false)
  {
    if (this->x+(this->w/2) > this->midx)
    {
      crossed_min = false;
      crossed_max = true;
      decide = true;
    }
  }

  if (this->velx < 0.0f)
  {
    if (this->x > this->xmin)
      this->x += this->velx;
    else
    {
      this->x = this->xmin+1;
      this->velx *= -1;
      decide = true;
    }
  }
  else if (this->velx > 0.0f)
  {
    if (this->x < this->xmax)
      this->x += this->velx;
    else
    {
      this->x = this->xmax-1;
      this->velx *= -1;
      decide = true;
    }
  }

  if (this->y > this->ymin && this->y < this->ymax)
    this->y += this->vely;
  else
  {

    if (this->y <= this->ymin)
      this->y = this->ymin + 1;
    else if (this->y >= this->ymax)
      this->y = this->ymax - 1;


    this->vely *= -1.0f;
    decide = false;
  }

  if (decide)
  {
    int value = rand() % 100;
    if (value >= 0 && value <= 33)
      this->vely = 1.0f;
    else if (value > 33 && value <= 66)
      this->vely = -1.0f;
    else if (value > 66 && value <= 99)
      this->vely = 0.0f;
  }

  if ((Clock::clockTicks - fire_pause) > 40)
  {
    fire_pause = Clock::clockTicks;

    if (rand()%100 < 30)
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
