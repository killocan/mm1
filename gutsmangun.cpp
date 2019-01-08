/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cmath>
#include <cstdio>

#include "gutsmangun.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "gutsmangunmanager.h"

#include "spritefiles.h"

//Gutsman Rock Gun

GutsmanGun::GutsmanGunFragment::GutsmanGunFragment(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::GUTSMANROCK_SPRITES)
{
}

GutsmanGun::GutsmanGun(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::GUTSMANROCK_SPRITES)
{
  this->old_x = this->x = x;
  this->old_y = this->y = y;

  isFacingRight = true;
  overstair = false;

  colorOffset = cur_stage->getOffset(mm_spritefiles::GUTSMANROCK_SPRITES);
  setAnimSeq(colorOffset);
  h = getFrameH();
  w = getFrameW();

  alive = true;
  life = 1;

  curState = GutsmanGun::MOVING;

  float dest_x = cur_stage->m_player->x;
  float dest_y = cur_stage->m_player->y - this->h + 10.0f;
  this->x += (dest_x - this->x) / 2;
  this->y += (dest_y - this->y) / 2;

  ticks = Clock::clockTicks;

  GutmanGunManager::instance()->addRock(this);
}

void GutsmanGun::launch()
{
  curState = LAUNCH;
}

void GutsmanGun::doLogic()
{
  Player * player = cur_stage->m_player;
  switch (curState)
  {
  case GutsmanGun::MOVING:
    if ((Clock::clockTicks - ticks) > 2)
    {
      ticks = Clock::clockTicks;
      this->x = cur_stage->m_player->x;
      this->y = cur_stage->m_player->y - this->h;
      this->curState = GutsmanGun::ATTACHED_TO_MEGAMAN;
    }
  break;
  case GutsmanGun::ATTACHED_TO_MEGAMAN:
    this->x = player->x;
    this->y = player->y - this->h + 10.0f;
  break;
  case GutsmanGun::LAUNCH:
    ;
  break;
  }
}

void GutsmanGun::die()
{
  alive = false;
  life = 0;
}

void GutsmanGun::doGravitation()
{
}

void GutsmanGun::checkOnCamera()
{
  if (life > 0)
  {
    alive = true;
  }
}
