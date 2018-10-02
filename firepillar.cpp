/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "firepillar.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//FirePillar

#define ICE_SPRITES 1

FirePillar::FirePillar(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::FIRE_V_WALL_SPRITES)
{
  this->x = x;
  this->y = y;

  offset = 0;
  pausebtwstates = 0;

  isFacingRight = true;

  colorOffset = 0;
  curState = FirePillar::GOING_UP;
  setAnimSeq(colorOffset);

  h = getFrameH();
  w = getFrameW();

  sprite = create_bitmap(w,h);

  this->y += mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;
  this->old_y = this->y;

  life  = 1;
}

FirePillar::~FirePillar()
{
  destroy_bitmap(sprite);
  sprite = NULL;
}

void FirePillar::hit(mm_weapons::weapon_st * pWeapon)
{
  // avoid annoying repeted sound
  if (pWeapon->weapon == mm_weapons::W_MEGA_BUSTER)
  {
    pWeapon->alive = false;
    Sounds::mm_sounds->play(HIT_ENEMY);
  }

  if ((pWeapon->weapon == mm_weapons::W_ICEMAN_GUN) && (curState != Character::FREEZE))
  {
    colorOffset = ICE_SPRITES;
    setAnimSeq(colorOffset, false);

    Sounds::mm_sounds->play(HIT_ENEMY);

    int desl = (int) ceil(float(offset)/mm_graphs_defs::TILE_SIZE);
    int xd = ((int)this->x) / mm_graphs_defs::TILE_SIZE;
    int yd = (((int)this->old_y) - mm_graphs_defs::TILE_SIZE) / mm_graphs_defs::TILE_SIZE;

    offset = desl * mm_graphs_defs::TILE_SIZE;

    if (offset > h)
      offset = h-4;

    this->y = (this->old_y - offset)-4;
    offset+=4;

    for (int i = 0; i < MIN(desl,4); ++i, --yd)
    {
      cur_stage->setTileAction(xd,yd,mm_tile_actions::TILE_SOLID);
    }

    freeze();
  }

  return;
}

void FirePillar::defrost()
{
  int xd = ((int)this->old_x)/mm_graphs_defs::TILE_SIZE;
  int yd = (((int)this->old_y)-mm_graphs_defs::TILE_SIZE)/mm_graphs_defs::TILE_SIZE;
  for (int i = 0; i < 5; ++i, --yd)
  {
    cur_stage->setTileAction(xd,yd,mm_tile_actions::TILE_VOID);
  }
}

void FirePillar::doLogic()
{
  if (curState != Character::FREEZE)
  {
    colorOffset = 0;
    setAnimSeq(colorOffset);
    Character::handleAnimation();
  }

  switch(curState)
  {
    case FirePillar::GOING_UP:
    {
      if (offset < h)
      {
        offset+=4;
        this->y = this->old_y - offset;

        pausebtwstates = Clock::clockTicks;
      }
      else
      {
        offset = h;
        this->y = this->old_y - offset;

        if ((Clock::clockTicks - pausebtwstates) > 60)
        {
          pausebtwstates = 0;
          curState = FirePillar::GOING_DOWN;
        }
      }
    }
    break;
    case FirePillar::GOING_DOWN:
    {
      if (offset > 0)
      {
        offset-=4;
        this->y = this->old_y - offset;

        pausebtwstates = Clock::clockTicks;
      }
      else
      {
        offset = 0;
        this->y = this->old_y - offset;

        if ((Clock::clockTicks - pausebtwstates) > 60)
        {
          pausebtwstates = 0;
          curState = FirePillar::GOING_UP;
        }
      }
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void FirePillar::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  
  clear_to_color(sprite, MASK_COLOR_24);

  blit(this->spriteSheet->getFrame(curSpriteFrame), sprite, 0, 0, 0, 0,
       sprite->w, (sprite->h-(sprite->h-offset)));

  masked_blit(sprite, bmp, 0, 0,
              this->sx, this->sy,
              sprite->w, (sprite->h-(sprite->h-offset)));
}

BITMAP * FirePillar::getFrame()
{
  return sprite;
}

bool FirePillar::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void FirePillar::doGravitation()
{
  return;
}

void FirePillar::respawn()
{
  life = 1;
  this->x = this->old_x;
  this->y = this->old_y;
  offset  = 0;
  colorOffset = 0;
  curState = FirePillar::GOING_UP;
  setAnimSeq(colorOffset);
  pausebtwstates = 0;

  return;
}
