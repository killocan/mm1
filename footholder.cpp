/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "footholder.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"
#include "spritefiles.h"

float FootHolder::decisions[6][3] =
{
  {1.0f,  1.0f, 0.0f},
  {-1.0f, 1.0f, 0.0f},
  {-1.0f, 1.0f, 0.0f},
  {-1.0f, 1.0f, 0.0f},
  {-1.0f,-1.0f, 0.0f},
  {-1.0f,-1.0f, 0.0f},
};

#define STEPX 1.20f
FootHolder::FootHolder(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FOOTHOLDER_SPRITES)
{
  this->x = x+60;
  this->y = y + (mm_graphs_defs::TILE_SIZE / 2);
  this->old_x = this->x;
  this->old_y = this->y;

  colorOffset = cur_stage->getOffset(mm_spritefiles::FOOTHOLDER_SPRITES);
  curState = FootHolder::MOVING;
  setAnimSeq(colorOffset + FootHolder::MOVING);

  this->h = getFrameH();
  this->w = getFrameW();

  current_direction = FootHolder::LEFT;
  displacement_x = this->w - 4;
  x_quadrant = 2;
  y_line = 0;
  current_decision = 0.0f;
  xstep = -STEPX;

  this->isPlatform = true;

  fire_pause  = Clock::clockTicks;
}

void FootHolder::respawn()
{
  this->x = this->old_x;
  this->y = this->old_y;

  current_direction = FootHolder::LEFT;
  displacement_x = this->w - 4;
  x_quadrant = 2;
  y_line = 0;
  current_decision = 0.0f;
  xstep = -STEPX;
}

void FootHolder::doLogic()
{
  if (this->cur_stage->m_player->onPlatform == true && this->cur_stage->m_player->conPlayer == this)
  {
    Player * p = this->cur_stage->m_player;
    int tilecoordx, tilecoordy, tiletype;

    if (this->xstep > 0)
    {
      if (!p->collisionVer((p->x + p->utilX) + this->xstep + p->utilXLen,
                           p->y,
                           tilecoordx, tilecoordy, tiletype))
        p->x += this->xstep;
    }
    else if (this->xstep < 0)
    {
      if (!p->collisionVer((p->x + p->utilX) - this->xstep,
                           p->y,
                           tilecoordx, tilecoordy, tiletype))
        p->x += this->xstep;
    }

    this->cur_stage->m_player->y += this->current_decision;
  }

  if (displacement_x < 60) //this->w)
  {
    ++displacement_x;

    this->x += this->xstep;
    this->y += this->current_decision;
  }
  else
  {
    displacement_x = 0;

    if (this->current_decision > 0)
    {
      if (y_line < 4) y_line++;
    }
    else if(this->current_decision < 0)
    {
      if (y_line > 0) y_line--;
    }

    if (current_direction == FootHolder::LEFT)
    {
      if (x_quadrant > 0)
      {
        --x_quadrant;
      }
      else
      {
        xstep = STEPX;
        current_direction = FootHolder::RIGHT;
      }
    }
    else if (current_direction == FootHolder::RIGHT)
    {
      if (x_quadrant < 1)
      {
        ++x_quadrant;
      }
      else
      {
        xstep = -STEPX;
        current_direction = FootHolder::LEFT;
      }
    }

    int rnd = rand()%3;
    current_decision = decisions[y_line][rnd];
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
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

  int xmin = this->old_x - 160.0f;
  int xmax = xmin + 180;
  int ymin = this->old_y;
  int ymax = ymin + 288;
  int midx = xmin + ((xmax-xmin)/2);

  rectfill(bmp, (xmin - GlobalCamera::mm_camera->x),
                (ymin - GlobalCamera::mm_camera->y),
                ((xmax) - GlobalCamera::mm_camera->x),
                ((ymax) - GlobalCamera::mm_camera->y),
                makecol(0,0,255));

  int yp;
  yp = ymin;
  for (int j = 0; j < 5; j++,yp+=(this->h*STEPX))
    for (int i = 0, xp = xmin; i < 3; i++, xp+= 60)
      rect(bmp, xp- GlobalCamera::mm_camera->x,
                yp- GlobalCamera::mm_camera->y,
                (xp + 60) - GlobalCamera::mm_camera->x,
                (yp + (this->h*STEPX)) - GlobalCamera::mm_camera->y, makecol(255,0,0));

  vline(bmp, (midx - GlobalCamera::mm_camera->x),
             (ymin - GlobalCamera::mm_camera->y),
             ((ymax) - GlobalCamera::mm_camera->y),
             makecol(0,0,0));
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;

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
