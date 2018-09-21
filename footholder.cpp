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
static const float MAX_X = 126.0f;
// 4 * MM_H while standing.
static const float MAX_Y = 192.0f;
// 2.5 * MM_W while standing.
static const float X_OFFSET = 105.0f;

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

  this->velx = -2.0f;
  this->vely = 1.0f;

  printf("x = [%d] y = [%d] ymin = [%f] xmin [%f] ymax [%f] xmax [%f] midx[%f] midy[%f]\n",
    this->x, this->y, this->ymin, this->xmin, this->ymax, this->xmax, this->midx, this->midy);

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

  bool decide = false;

  if (this->velx < 0.0f)
  {
    if (this->x <= this->midx)
      decide = true;
  }
  else if (this->velx > 0.0f)
  {
    if (this->x >= this->midx)
      decide = true;
  }

  if (this->velx < 0.0f && this->x > this->xmin)
    this->x += this->velx;
  else
  {
    this->velx *= -1;
    decide = true;
  }

  if (this->velx > 0.0f && this->x < this->xmax)
    this->x += this->velx;
  else
  {
    this->velx *= -1;
    decide = true;
  }

  if (this->y > this->ymin && this->y < this->ymax)
    this->y += this->vely;
  else
  {
    this->vely *= -1;
    this->y += this->vely;
  }

  if (decide)
  {
    int value = rand()%100;
    if (value >= 0 && value <= 33)
      this->vely = 1.0f;
    else if (value > 33 && value <= 66)
      this->vely = -1.0f;
    else if (value > 66 && value <= 99)
      this->vely = 0.0f;
  }

  if ((Clock::clockTicks - fire_pause) > 60)
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
