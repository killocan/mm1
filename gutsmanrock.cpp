/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cmath>
#include <cstdio>

#include "gutsmanrock.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Gutsman Rock

GutsmanRock::GutsmanRock(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::GUTSMANROCK_SPRITES)
{
  this->old_x = this->x = x;
  this->old_y = this->y = y;

  velx          = 0;
  overstair     = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::GUTSMANROCK_SPRITES);
  setAnimSeq(colorOffset + GutsmanRock::STILL);
  h = getFrameH();
  w = getFrameW();

  alive = true;
  life = 1;

  int xd, yd;
  xd = this->x / mm_graphs_defs::TILE_SIZE;
  yd = this->y / mm_graphs_defs::TILE_SIZE;
  cur_stage->setTileAction(xd,   yd, mm_tile_actions::TILE_SOLID);
  cur_stage->setTileAction(xd+1, yd, mm_tile_actions::TILE_SOLID);
  cur_stage->setTileAction(xd,   yd+1, mm_tile_actions::TILE_SOLID);
  cur_stage->setTileAction(xd+1, yd+1, mm_tile_actions::TILE_SOLID);

  sprite = create_bitmap(w,h);
}

GutsmanRock::~GutsmanRock()
{
  destroy_bitmap(sprite);
  sprite = NULL;
}

BITMAP * GutsmanRock::getFrame()
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  BITMAP * curSprTile = this->spriteSheet->getFrame(curSpriteFrame);
  clear_to_color(sprite, 0);
  blit(curSprTile, sprite, 0,0,0,0, curSprTile->w, curSprTile->h);

  return sprite;
}

/*
void GutsmanRock::drawCharacter(BITMAP * bmp)
{
}
*/

void GutsmanRock::doLogic()
{
  Player * player = cur_stage->m_player;
  if (life > 0)
  {
    if (this->x < player->x)
    {
      if (player->x - (this->x + this->w) > 4)
      {
        if (player->conPlayer == this)
          player->conPlayer = NULL;
      }
      else
        player->conPlayer = this;
    }
    else if (this->x > player->x)
    {
      if (this->x - (player->x + player->w) > 10)
      {
        if (player->conPlayer == this)
          player->conPlayer = NULL;
      }
      else
        player->conPlayer = this;
    }

    if ((player->y > (this->y+this->h)) || ((player->y+player->h) < this->y))
      player->conPlayer = NULL;

    if ((cur_stage->m_player->conPlayer == this) && (player->curWeapon == mm_weapons::W_GUTSMAN_GUN))
    {
      setAnimSeq(GutsmanRock::BLINKING);
    }
    else
    {
      setAnimSeq(GutsmanRock::STILL);
    }
  }
  else
  {
    die();
  }
}

void GutsmanRock::die()
{
  int xd, yd;
  xd = this->x / mm_graphs_defs::TILE_SIZE;
  yd = this->y / mm_graphs_defs::TILE_SIZE;
  cur_stage->setTileAction(xd, yd, mm_tile_actions::TILE_VOID);
  cur_stage->setTileAction(xd + 1, yd, mm_tile_actions::TILE_VOID);
  cur_stage->setTileAction(xd, yd + 1, mm_tile_actions::TILE_VOID);
  cur_stage->setTileAction(xd + 1, yd + 1, mm_tile_actions::TILE_VOID);

  alive = false;
  life = 0;
  cur_stage->m_player->conPlayer = NULL;
}

void GutsmanRock::hit(mm_weapons::weapon_st * pWeapon)
{
  switch (pWeapon->weapon)
  {
  case mm_weapons::W_ELECMAN_GUN:
    pWeapon->alive = false;
    die();
  default:
    break;
  }
}

void GutsmanRock::doGravitation()
{
}

void GutsmanRock::checkOnCamera()
{
  if (life > 0)
  {
    alive = true;
  }
}
