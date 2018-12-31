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

  setAnimSeq(GutsmanRock::STILL);
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
}

void GutsmanRock::doLogic()
{
  Player * player = cur_stage->m_player;
  if (life > 0)
  {
    //TODO: just calc the distance between both sprites centers... only one if :)
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
    int xd, yd;
    xd = this->x / mm_graphs_defs::TILE_SIZE;
    yd = this->y / mm_graphs_defs::TILE_SIZE;
    cur_stage->setTileAction(xd,     yd,     mm_tile_actions::TILE_VOID);
    cur_stage->setTileAction(xd + 1, yd,     mm_tile_actions::TILE_VOID);
    cur_stage->setTileAction(xd,     yd + 1, mm_tile_actions::TILE_VOID);
    cur_stage->setTileAction(xd + 1, yd + 1, mm_tile_actions::TILE_VOID);

    alive = false;
  }
}

void GutsmanRock::doGravitation()
{
}
