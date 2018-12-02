/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cstdio>
#include <cmath>

#include "met.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Met

Met::Met(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::MET_SPRITES)
{
  // Map coords to global world coords
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  //this->x -= 16;
  this->old_x = this->x;

  vely          = 1;
  overstair     = false;
  isFacingRight = false;

  curState = Met::CLOSED;
  setAnimSeq(Met::CLOSED);
  h = getFrameH();
 
  adjustToFloor();

  m_ypos = (this->y + (h/2));
  this->old_y = this->y;

  life  = 1;
}

bool Met::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Met::fire()
{
  int xpos = isFacingRight ? x+utilXLen : x;
  float vx   = isFacingRight ? 5.5f : -5.5f;
  float vy   = 0.0f;

  vy = 0.0f;
  mm_weapons::createMegaBuster(this, xpos, m_ypos, vx, vy, 1);
  vy = 5.0f;
  mm_weapons::createMegaBuster(this, xpos, m_ypos, vx, vy, 1);
  vy = -5.0f;
  mm_weapons::createMegaBuster(this, xpos, m_ypos, vx, vy, 1);

  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void Met::doLogic()
{
  bool cycleDone = false;
  int px = cur_stage->m_player->x;

  switch(curState)
  {
    case Met::CLOSED:
    {
      isFacingRight = x < px;

      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        int dist = abs(px-x);
        if (dist < 192)
        {
          if (rand()%100 < 90)
          {
            curState = Met::FIRING;
            setAnimSeq(Met::FIRING);
          }
        }
      }
    }
    break;
    case Met::FIRING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        fire();

        curState = Met::CLOSING;
        setAnimSeq(Met::CLOSING);
      }
    }
    break;
    case Met::CLOSING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = Met::CLOSED;
        setAnimSeq(Met::CLOSED);
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

void Met::doGravitation()
{
  return;
}

void Met::adjustToFloor()
{
  int tilecoordx, tilecoordy, tiletype;

  while(true)
  {
    if(collisionHor(x+utilX, y+vely+h, tilecoordx, tilecoordy, true, tiletype) == false)
    {
      y += vely;
    }
    else
    {
      y+=2;

#ifdef DEBUG
      fprintf(stderr,"FINAL Met: x=[%f] utilX=[%f] y=[%f] vely=[%f] h=[%f]\n",
                                    x,     utilX,     y,     vely,     h);
#endif
      break;
    }
  }
}

void Met::hit(mm_weapons::weapon_st * pWeapon)
{
  if (curState == Met::CLOSED)
  {
    Sounds::mm_sounds->play(HIT_SHIELD);
    if (pWeapon->weapon == mm_weapons::W_MEGA_BUSTER) pWeapon->alive = false;
    return;
  }
  else
  {
    Sounds::mm_sounds->play(HIT_ENEMY);  
  }

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
      freeze();
    break;
    case mm_weapons::W_MEGA_BUSTER:
      pWeapon->alive = false;
      life--;
    break;
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

void Met::respawn()
{
  life  = 1;
  this->x = this->old_x;
  this->y = this->old_y;
  isFacingRight = false;
  curState = Met::CLOSED;

  resetAnimSeq(Met::CLOSED);

  return;
}
