/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "crazyrazy.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

// CrazyRazy Upper half

CrazyRazy::CrazyRazyFragmentUpperHalf::CrazyRazyFragmentUpperHalf(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::CRAZYRAZY_UPPER_SPRITES)
{
  this->x = x;
  this->old_x = this->x;
  this->y = y;
  alive = true;
  canCollidePlayer = true;
  canCollideBullet = true;

  setAnimSeq(CrazyRazyFragmentUpperHalf::MOVING);
  h = getFrameH();

  curState = CrazyRazyFragmentUpperHalf::MOVING;
  velx = 3;

  curPoint = 0;
  m_bAnimation = true;

  life = 1;
}

void CrazyRazy::CrazyRazyFragmentUpperHalf::doGravitation()
{
  return;
}

void CrazyRazy::CrazyRazyFragmentUpperHalf::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
      freeze();
    break;
    case mm_weapons::W_MEGA_BUSTER:
    {
      pWeapon->alive = false;

      life = 0;
    }
    break;
    default:
      life = 0;
    break;
  }

  if (life == 0)
  {
    this->y-=16;
    die();
    alive = false;
  }

  return;
}

void CrazyRazy::CrazyRazyFragmentUpperHalf::doLogic()
{
  if (this->sx < 0 || this->sx > mm_graphs_defs::UTIL_W || this->sy < 0 || this->sy > mm_graphs_defs::UTIL_H)
  {
    alive = false;
  }
  else
  {
    int px = cur_stage->m_player->x + cur_stage->m_player->utilX;

    switch(curState)
    {
      case CrazyRazyFragmentUpperHalf::MOVING:
      {
        if (x < px)
        {
          isFacingRight = true;
          x += velx;
        }
        else
        {
          isFacingRight = false;
          x -= velx;
        }

        int dist = abs(px-x);
        if (dist < mm_graphs_defs::TILE_SIZE*3)
        {
          curState = CrazyRazyFragmentUpperHalf::CALC_ATTACK;
        }
      }
      break;
      case CrazyRazyFragmentUpperHalf::CALC_ATTACK:
      {
        int py = 0;
        if (y > py)
        {
          py = cur_stage->m_player->y + (2*mm_graphs_defs::TILE_SIZE);
          py += (py - y > 300) ? (mm_graphs_defs::TILE_SIZE+10) : 0;
        }
        else
        {
          py = cur_stage->m_player->y;
          py -= (y - py > 300) ? mm_graphs_defs::TILE_SIZE : 0;
        }

        int endX = mm_graphs_defs::TILE_SIZE*6;
        endX *= (isFacingRight ? 1 : -1);

        CTRL_POINTS[0] = x;
        CTRL_POINTS[1] = y;
        CTRL_POINTS[2] = px;
        CTRL_POINTS[3] = py;
        CTRL_POINTS[4] = px;
        CTRL_POINTS[5] = py;
        CTRL_POINTS[6] = x + endX;
        CTRL_POINTS[7] = y;
        calc_spline(CTRL_POINTS, CrazyRazyFragmentUpperHalf::CURVE_PNTS, curveX, curveY);

        m_bAnimation = true;

        curState = CrazyRazyFragmentUpperHalf::ATTACK;
      }
      break;
      case CrazyRazyFragmentUpperHalf::ATTACK:
      {
        if (m_bAnimation == true)
        {
          bool bAnimOver = false;
          Character::handleAnimation(&bAnimOver);
          m_bAnimation = !bAnimOver;
        }

        x = curveX[curPoint];
        y = curveY[curPoint];
        curPoint++;

        if (curPoint == CrazyRazyFragmentUpperHalf::CURVE_PNTS)
        {
          curPoint = 0;
          curState = CrazyRazyFragmentUpperHalf::MOVING;
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
}

// CrazyRazy Lower half

CrazyRazy::CrazyRazyFragmentLowerHalf::CrazyRazyFragmentLowerHalf(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::CRAZYRAZY_LOWER_SPRITES)
{
  this->x = x;
  this->old_x = this->x;
  this->y = y;
  alive = true;
  canCollidePlayer = true;
  canCollideBullet = true;

  setAnimSeq(CrazyRazyFragmentLowerHalf::MOVING);
  h = getFrameH();

  curState = CrazyRazyFragmentLowerHalf::MOVING;
  isFacingRight = velx>0;//x < cur_stage->m_player->x;
  //velx = (isFacingRight) ? 2 : -2;
  vely = 1;
}

void CrazyRazy::CrazyRazyFragmentLowerHalf::doLogic()
{
  isFacingRight = velx>0;//x < cur_stage->m_player->x;

  switch(curState)
  {
    case CrazyRazyFragmentLowerHalf::MOVING:
    {
      this->x += velx;
      if (this->sx < 0)
      {
        alive = false;
        break;
      }

      Character::handleAnimation();
      if (abs(this->x-this->old_x) > 192)
      {
        this->y+=16;
        die();
        alive = false;
      }
    }
    break;
  }
}

void CrazyRazy::CrazyRazyFragmentLowerHalf::hit(mm_weapons::weapon_st * pWeapon)
{
  this->y+=16;
  die();
  alive = false;
  if (pWeapon->weapon == mm_weapons::W_MEGA_BUSTER)
    pWeapon->alive = false;
}

// CrazyRazy

CrazyRazy::CrazyRazy(const Stage & stage, int x, int y) : Character(stage, mm_spritefiles::CRAZYRAZY_SPRITES),
                     bDieUpper(false)
{
  // Map coords to global world coords
  this->x = x;//*mm_graphs_defs::TILE_SIZE;
  this->y = y;//*mm_graphs_defs::TILE_SIZE;

  this->old_y = this->y;
  this->old_x = this->x;

  velx = -3;
  isFacingRight = cur_stage->m_player->x > this->x;

  curState = CrazyRazy::MOVING;
  setAnimSeq(CrazyRazy::MOVING);
  h = getFrameH();
  w = getFrameW();

  life = 3;
}

void CrazyRazy::fire()
{
  int xpos = x+w/2-8;
  int ypos = this->y + 18;
  mm_weapons::createMegaBuster(this, xpos, ypos, -5.5f, 0.0f, 4);
  Sounds::mm_sounds->play(ENEMY_MEGA_BUSTER);

  return;
}

void CrazyRazy::hit(mm_weapons::weapon_st * pWeapon)
{
  Sounds::mm_sounds->play(HIT_ENEMY);

  int /*xhit,*/ yhit;
  //xhit = abs((int)pWeapon->xcol - this->x);
  yhit = abs((int)pWeapon->ycol - this->y);

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
      freeze();
    break;
    case mm_weapons::W_MEGA_BUSTER:
    {
      pWeapon->alive = false;

      if (yhit >= (h>>1))
      {
        life--;
      }
      else
      {
        bDieUpper = true;
        life = 0;
      }
    }
    break;
    default:
      life = 0;
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

void CrazyRazy::die()
{
  if (collideWithPlayer == false)
  {
    if (bDieUpper == true)
    {
      // Die and create explosion for lower and upper part.
      int x = this->x + abs((getFrame()->w-32)>>1);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, x, this->y+16));
      TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, x, this->y));
    }
    else
    {
      // Create Upper part fragment and explode lower.
      Character * pFragment00 = cur_stage->createCharacter(mm_tile_actions::CRAZYRAZY_UPPER_CHAR, this->x, this->y-64);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment00);

      int x = this->x + abs((getFrame()->w-32)>>1);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, x, this->y+16));
    }
  }

  alive = false;
  respawn();
}

void CrazyRazy::doLogic()
{
  switch(curState)
  {
    case CrazyRazy::MOVING:
    {
      isFacingRight = cur_stage->m_player->x > this->x;

      int px = cur_stage->m_player->x;
      // When player pass through cause it's invencible(blinking).
      bool hasPlayerCrossed = (px > this->x);
      if (collideWithPlayer == true || hasPlayerCrossed == true)
      {
        // Create upper and lower parts.
        int lowerVelx = (px > this->x) ? 3 : -3;
        Character * pFragment00 = cur_stage->createCharacter(mm_tile_actions::CRAZYRAZY_LOWER_CHAR, this->x, this->y, lowerVelx);
        Character * pFragment01 = cur_stage->createCharacter(mm_tile_actions::CRAZYRAZY_UPPER_CHAR, this->x, this->y-64);
        TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment00);
        TemporaryCharacterList::mm_tempCharacterLst.push_back(pFragment01);

        alive = false;
        respawn();
      }

      // laziness
      if (rand()%10000 < 100) fire();

      this->x += velx;
    }
    break;
    case Character::FREEZE:
    {
      handleFreeze();
    }
    break;
  }
}

void CrazyRazy::respawn()
{
  life          = 3;
  this->x       = this->old_x;
  this->y       = this->old_y;

  collideWithPlayer = false;
  bDieUpper         = false;

  curState      = CrazyRazy::MOVING;
  resetAnimSeq(CrazyRazy::MOVING);

  return;
}
