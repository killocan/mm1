/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cstdio>

#include "screwbomber.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Screw Bomber

ScrewBomber::ScrewBomber(const Stage & stage, int x_map, int y_map) : Character(stage, mm_spritefiles::SCREWBOMBER_SPRITES)
{
  // Map coords to global world coords
  this->x = x_map;// *mm_graphs_defs::TILE_SIZE;
  this->y = y_map;// *mm_graphs_defs::TILE_SIZE;

  this->old_x = this->x;

  vely = velx   = 0;
  overstair     = false;
  isFacingDown  = false;
  colorOffset   = 0;

  fireCount      = 0;
  fireFrameCount = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::SCREWBOMBER_SPRITES);
  curState    = ScrewBomber::SLEEPING;
  setAnimSeq(colorOffset + ScrewBomber::SLEEPING);
  h = getFrameH();
  w = getFrameW();
  sprite = create_bitmap(w,h);

  switch(colorOffset)
  {
    case 0:
      bulletOffset = 1;
    break;
    case 4:
      bulletOffset = 2;
    break;
    case 8:
      bulletOffset = 3;
    break;
  }

  adjustToFloorOrCeiling(x_map / mm_graphs_defs::TILE_SIZE, y_map / mm_graphs_defs::TILE_SIZE);
  
  this->old_y = this->y;

  life  = 3;
}

ScrewBomber::~ScrewBomber()
{
  destroy_bitmap(sprite);
  sprite = NULL;
}

bool ScrewBomber::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void ScrewBomber::doLogic()
{
  bool cycleDone = false;

  switch(curState)
  {
    case ScrewBomber::SLEEPING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int px = cur_stage->m_player->x;
        int dist = abs(px-x);
        if (dist > 160) return;

        curState = ScrewBomber::OPENING;
        setAnimSeq(colorOffset + ScrewBomber::OPENING);
        animSeqSize = anim_seqs[curAnimSeq].size();
      }
    }
    break;
    case ScrewBomber::OPENING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
      }

      if (animSeqSize == 0)
      {
        curState = ScrewBomber::FIRING;
        setAnimSeq(colorOffset + ScrewBomber::FIRING);
        animSeqSize = anim_seqs[curAnimSeq].size();
      }
    }
    break;
    case ScrewBomber::FIRING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        fireFrameCount++;
      }

      if (fireFrameCount == 8)
      {
        fireFrameCount = 0;
        if (fireCount < 2) fire();
        fireCount++;
      }

      if (fireCount == 3)
      {
        fireCount      = 0;
        fireFrameCount = 0;

        curState = ScrewBomber::CLOSING;
        setAnimSeq(colorOffset + ScrewBomber::CLOSING);
        animSeqSize = anim_seqs[curAnimSeq].size();
      }
    }
    break;
    case ScrewBomber::CLOSING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        animSeqSize--;
      }

      if (animSeqSize == 0)
      {
        curState = ScrewBomber::SLEEPING;
        setAnimSeq(colorOffset + ScrewBomber::SLEEPING);
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

void ScrewBomber::fire()
{
  int xpos = (this->x + this->w/2)-5;
  int ypos = this->y;

  if (isFacingDown == true)
  {
    // Upside down.
    ypos +=  this->h - 10;

    mm_weapons::createMegaBuster(this, xpos, ypos,  3.7f, 0.0f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos,  2.7f, 2.7f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos,  0.0f, 3.7f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos, -2.7f, 2.7f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos, -3.7f, 0.0f, bulletOffset);
  }
  else
  {
    mm_weapons::createMegaBuster(this, xpos, ypos,  3.7f, 0.0f,  bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos,  2.5f, -2.5f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos,  0.0f, -3.7f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos, -2.5f, -2.5f, bulletOffset);
    mm_weapons::createMegaBuster(this, xpos, ypos, -3.7f, 0.0f,  bulletOffset);
  }

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void ScrewBomber::doGravitation()
{
  return;
}

void ScrewBomber::adjustToFloorOrCeiling(int x_map, int y_map)
{
  int yMin;
  for (yMin = y_map; yMin >= 0; yMin--)
  {
    if (cur_stage->tileAction(x_map, yMin) == mm_tile_actions::TILE_SOLID)
    {
      break;
    }
  }
  int yMax;
  for (yMax = y_map; yMax < cur_stage->max_y; yMax++)
  {
    if (cur_stage->tileAction(x_map, yMax) == mm_tile_actions::TILE_SOLID)
    {
      break;
    }
  }

  // which wall is closer, up or down?
  closer = (abs((int)(yMin-y_map)) < abs((int)(yMax-y_map))) ? 0 : 1;

  int tilecoordx, tilecoordy, tiletype;
  int dir = (closer == 0) ? -1 : 1;
  int h   = (closer == 0) ?  0 : this->h;
  isFacingDown = (closer == 0);

  while(true)
  {
    if(collisionHor(x, y+h, tilecoordx, tilecoordy, (closer!=0), tiletype) == false)
    {
      y += dir;
    }
    else
    {
      // adjust one pixel.
      if (dir == -1)
      {
        y += 1;
      }

#ifdef DEBUG
      fprintf(stderr,"FINAL ScrewBomber: x=[%f] utilX=[%f] y=[%f] vely=[%f] h=[%d]\n",
                                    x,     utilX,     y,     vely,     h);
#endif
      break;
    }
  }
}

void ScrewBomber::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
    }
    break;
    case mm_weapons::W_MEGA_BUSTER:
      pWeapon->alive = false;
    default:
      life--;
    break;
  }

  if (life == 0)
  {
    die();
    alive = false;
    respawn();
  }

  return;
}

void ScrewBomber::respawn()
{
  life  = 3;

  this->x = this->old_x;
  this->y = this->old_y;

  fireCount      = 0;
  fireFrameCount = 0;

  curState   = ScrewBomber::SLEEPING;
  resetAnimSeq(colorOffset + ScrewBomber::SLEEPING);

  return;
}

void ScrewBomber::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;

  clear_to_color(sprite, MASK_COLOR_24);

  if (isFacingDown == false)
  {
    draw_sprite(sprite, this->spriteSheet->getFrame(curSpriteFrame), 0, 0);
  }
  else
  {
    draw_sprite_vh_flip(sprite, this->spriteSheet->getFrame(curSpriteFrame), 0, 0);
  }

  masked_blit(sprite, bmp, 0, 0,
              this->sx, this->sy,
              sprite->w, sprite->h);
}

BITMAP * ScrewBomber::getFrame()
{
  return sprite;
}
