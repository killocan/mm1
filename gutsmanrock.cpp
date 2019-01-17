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

void GutsmanRock::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  BITMAP * curSprTile = this->spriteSheet->getFrame(curSpriteFrame);
  clear_to_color(sprite, 0);
  masked_blit(curSprTile, sprite, 0,0,0,0, curSprTile->w, curSprTile->h);

  if (isFacingRight == true)
  {
    draw_sprite(bmp, sprite, this->sx, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, sprite, this->sx, this->sy);
  }
}


void GutsmanRock::doLogic()
{
  Player * player = cur_stage->m_player;
  if (life > 0)
  {
    if (player->conPlayer == this)
    {
      float tcenterx = this->x + this->w / 2.0f;
      float tcentery = this->y + this->h / 2.0f;

      float pcenterx = player->x + player->getFrameW() / 2.0f;
      float pcentery = player->y + player->h / 2.0f;

      float dist = sqrtf(pow((tcenterx - pcenterx), 2.0f) + pow((tcentery - pcentery), 2.0f));

      if (dist > this->w - 10)
      {
        player->conPlayer = NULL;
      }
    }

    if ((cur_stage->m_player->conPlayer == this) && (player->curWeapon == mm_weapons::W_GUTSMAN_GUN))
    {
      setAnimSeq(colorOffset + GutsmanRock::BLINKING);
    }
    else
    {
      setAnimSeq(colorOffset + GutsmanRock::STILL);
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
    
    Sounds::mm_sounds->play(SUPER_ARM);
    this->isFacingRight = cur_stage->m_player->isFacingRight;
    mm_weapons::createGutsmanRock(this, true);

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
