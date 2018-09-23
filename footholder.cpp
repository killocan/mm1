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

FootHolder::FootHolder(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FOOTHOLDER_SPRITES)
{
  this->x = x + 2;
  this->y = y + (mm_graphs_defs::TILE_SIZE / 2);
  this->old_x = this->x;
  this->old_y = this->y;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FOOTHOLDER_SPRITES);
  curState = FootHolder::MOVING;
  setAnimSeq(colorOffset + FootHolder::MOVING);

  this->h = getFrameH();
  this->w = getFrameW();

  this->ymin = this->y;
  this->ymax = this->ymin + (6 * this->h);

  this->xmin = this->x - 100;
  this->xmax = this->xmin + ((5 * this->w)/2);

  this->midx = this->xmin + ((this->xmax - this->xmin) / 2) + (this->w / 2);

  this->velx = -1.0f;
  this->vely = 0.0f;

  this->isPlatform = true;

  crossed_min = crossed_max = false;

  fire_pause = Clock::clockTicks;
}

void FootHolder::respawn()
{
  this->x = this->old_x;
  this->y = this->old_y;

  this->velx = -1.0f;
  this->vely = 0.0f;
  this->crossed_min = this->crossed_max = false;
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
    if (this->x+(this->w/2) == this->midx)
    {
      crossed_max = false;
      crossed_min = true;
      decide = true;
    }
  }
  else if (this->velx > 0.0f && crossed_max == false)
  {
    if (this->x+(this->w/2) == this->midx)
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
      this->velx *= -1;
      decide = true;
    }
  }

  if (vely < 0)
  {
    if (this->y > this->ymin)
      this->y += this->vely;
    else
    {
      this->vely *= -1;
      decide = false;
    }
  }
  else if (vely > 0)
  {
    if (this->y < this->ymax)
      this->y += this->vely;
    else
    {
      this->vely *= -1;
      decide = false;
    }
  }

  {

    /*if (this->y <= this->ymin)
      this->y = this->ymin + 1;
    else if (this->y >= this->ymax)
      this->y = this->ymax - 1;*/

    //this->vely *= -1.0f;

    //decide = false;
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

#ifdef DEBUG
void FootHolder::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

  rectfill(bmp, (this->xmin - GlobalCamera::mm_camera->x),
                (this->ymin - GlobalCamera::mm_camera->y),
                ((this->xmax+this->w) - GlobalCamera::mm_camera->x),
                ((this->ymax+this->h) - GlobalCamera::mm_camera->y),
                makecol(0,0,255));
  vline(bmp, (this->midx - GlobalCamera::mm_camera->x),
             (this->ymin - GlobalCamera::mm_camera->y),
             ((this->ymax+this->h) - GlobalCamera::mm_camera->y),
             makecol(0,0,0));

  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

  if (isFacingRight == true)
  {
    draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
}
#endif

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
