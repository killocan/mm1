/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cmath>
#include <cstdio>

#include "tacklefire.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Tackle Fire

TackleFire::TackleFireFragment::TackleFireFragment(const Stage & stage, int x, int y, void * param)
  : Character(stage, mm_spritefiles::TACKLEFIRE_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  overstair     = false;
  isFacingRight = false;
  colorOffset   = 0;

  setAnimSeq(TackleFire::TackleFireFragment::GOINGUP);
  h = getFrameH();
  w = getFrameW();

  alive = true;
  canCollidePlayer = true;
  canCollideBullet = true;

  curState = TackleFire::TackleFireFragment::WAITING;

  life = 1;
  vely = 3;

  xorder = *((int*)param);
}

void TackleFire::TackleFireFragment::doLogic()
{
  if (cur_stage->horz_scroll)
    curState = TackleFire::TackleFireFragment::WAITING;

  switch(curState)
  {
    case TackleFire::TackleFireFragment::GOINGUP:
    {
      if (y > (GlobalCamera::mm_camera->y-(2*mm_graphs_defs::TILE_SIZE)))
      {
        y -= mm_graphs_defs::TILE_SIZE/4;
      }
      else
      {
        int dist = cur_stage->m_player->x - x;
        if (abs(dist) > (8*mm_graphs_defs::TILE_SIZE))
          velx = 2;
        else
          velx = 1;

        // megaman x pos is lesser than fragment x pos, so invert velx signal.
        if (dist < 0)
          velx = -velx;

        // sometimes, some of them just go in the wrong direction :)
        velx *= rand()%10 < 8 ? 1 : -1;
        isFacingRight = velx > 0;

        x += xorder * (2*mm_graphs_defs::TILE_SIZE);

        curState = TackleFire::TackleFireFragment::RESTING;
        setAnimSeq(TackleFire::TackleFireFragment::RESTING);
      }
    }
    break;
    case TackleFire::TackleFireFragment::GOINDDOWN:
    {
      Character::handleAnimation();

      y += vely;
      x += velx;

      if (y > GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->w)
      {
        curState = TackleFire::TackleFireFragment::WAITING;
      }
    }
    break;
    case TackleFire::TackleFireFragment::RESTING:
    {
      if (Character::handleAnimation() == true)
      {
        curState = TackleFire::TackleFireFragment::GOINDDOWN;
        setAnimSeq(TackleFire::TackleFireFragment::GOINDDOWN);
      }
    }
    break;
    case TackleFire::TackleFireFragment::WAITING:
    {
      canCollidePlayer = false;
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void TackleFire::TackleFireFragment::doGravitation()
{
  return;
}

void TackleFire::TackleFireFragment::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == GOINGUP)
    return;

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

  if (life <= 0)
  {
    die();
    x = old_x;
    y = old_y;
    curState = TackleFire::TackleFireFragment::WAITING;
  }

  return;
}

void TackleFire::TackleFireFragment::checkOnCamera()
{
  if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w)
  {
    alive = true;
  }
  else
  {
    x = old_x;
    y = old_y;
    curState = TackleFire::TackleFireFragment::WAITING;
  }
}

void TackleFire::TackleFireFragment::resetState()
{
  respawn();
}

void TackleFire::TackleFireFragment::setWait()
{
  x = old_x;
  y = old_y;
  curState = TackleFire::TackleFireFragment::WAITING;
}

void TackleFire::TackleFireFragment::respawn()
{
  life  = 1;
  alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  curState = TackleFire::TackleFireFragment::GOINGUP;
  resetAnimSeq(TackleFire::TackleFireFragment::GOINGUP);

  return;
}

void TackleFire::TackleFireFragment::drawCharacter(BITMAP * bmp)
{
  if (this->curState != TackleFire::TackleFireFragment::WAITING)
    Character::drawCharacter(bmp);
}

TackleFire::TackleFire(const Stage & stage, int x, int y) 
  : Character(stage, mm_spritefiles::TACKLEFIRE_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = x;
  this->old_y = y;

  curState = TackleFire::CONTROLLING;
  canCollidePlayer = false;

  fragmentsAlive = 3;

  int xdes = 0; // center
  fragments[0] = cur_stage->createCharacter(mm_tile_actions::TACKLEFIRE_FRAGMENT, 
                                            x, this->y+(2*mm_graphs_defs::TILE_SIZE),0,0, (void*)&xdes);
  xdes = -1; // left
  fragments[1] = cur_stage->createCharacter(mm_tile_actions::TACKLEFIRE_FRAGMENT, 
                                            x, this->y+(4*mm_graphs_defs::TILE_SIZE),0,0, (void*)&xdes);
  xdes = 1; // right
  fragments[2] = cur_stage->createCharacter(mm_tile_actions::TACKLEFIRE_FRAGMENT, 
                                            x, this->y+(6*mm_graphs_defs::TILE_SIZE),0,0, (void*)&xdes);

  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[0]);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[1]);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[2]);

  internalSprite = create_bitmap(10,10);
#ifdef DEBUG
  clear_to_color(internalSprite, makecol(0,0,255));
#else
  clear_to_color(internalSprite, MASK_COLOR_24);
#endif
}

TackleFire::~TackleFire()
{
  destroy_bitmap(internalSprite);
  internalSprite = NULL;
}

BITMAP * TackleFire::getFrame()
{
  return internalSprite;
}

void TackleFire::doLogic()
{
  if (curState == TackleFire::CONTROLLING)
  {
    fragmentsAlive = 3;
    for (int i = 0; i < 3; ++i)
    {
      if (fragments[i]->curState == TackleFire::TackleFireFragment::WAITING)
      {
        --fragmentsAlive;
      }
    }

    if (fragmentsAlive == 0)
    {
      TackleFire::TackleFireFragment * cur;
      for (int i = 0; i < 3; ++i)
      {
        cur = dynamic_cast <TackleFire::TackleFireFragment*> (fragments[i]);
        cur->resetState();
        cur->canCollidePlayer = true;
        //curState = TackleFire::TackleFireFragment::GOINGUP;
      }
    }
  }
}

bool TackleFire::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void TackleFire::doGravitation()
{
  return;
}

void TackleFire::respawn()
{
  TackleFire::TackleFireFragment * cur;
  for (int i = 0; i < 3; ++i)
  {
    cur = dynamic_cast <TackleFire::TackleFireFragment*> (fragments[i]);
    if (cur->curState == TackleFire::TackleFireFragment::WAITING)
      cur->setWait();
  }
}

void TackleFire::drawCharacter(BITMAP * bmp)
{
#ifdef DEBUG
  draw_sprite(bmp, internalSprite, sx, sy);
#endif
  return;
}
