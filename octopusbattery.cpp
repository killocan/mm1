/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <math.h>

#include "octopusbattery.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Octopus Battery

OctopusBattery::OctopusBattery(const Stage & stage, int x_map, int y_map,  e_orientation orientation) : Character(stage, mm_spritefiles::OCTOPUSBATTERY_SPRITES)
{
  // Map coords to global world coords
  this->x = x_map;//*mm_graphs_defs::TILE_SIZE;
  this->y = y_map;//*mm_graphs_defs::TILE_SIZE;

  this->m_orientation = orientation;


  
  overstair     = false;
  isFacingRight = true;
  isFacingDown  = true;
  colorOffset   = 0;

  colorOffset = cur_stage->getOffset(mm_spritefiles::OCTOPUSBATTERY_SPRITES);
  setAnimSeq(colorOffset + OctopusBattery::SLEEPING);
  curState = OctopusBattery::SLEEPING;
  h = getFrameH();

  velx = 1;
  vely = 1;

  findBorders(stage, x_map / mm_graphs_defs::TILE_SIZE, y_map / mm_graphs_defs::TILE_SIZE);
  adjustToWall();

  this->old_y = this->y;
  this->old_x = this->x;

#ifdef DEBUG
  fprintf(stderr,"OctopusBattery xmin = [%d] xmax = [%d] ymin = [%d] ymax = [%d]\n", minX, maxX, minY, maxY);
#endif

  life  = 5;
}

void OctopusBattery::findBorders(const Stage & stage, int x_map, int y_map)
{
  minX = minY = maxX = maxY = 0;

  if (m_orientation == OctopusBattery::HORIZONTAL)
  {
    int xMin;
    for (xMin = x_map; xMin >= 0; xMin--)
    {
      if (stage.tileAction(xMin, y_map) == mm_tile_actions::TILE_SOLID)
      {
        break;
      }
    }
    minX = (xMin+1)*mm_graphs_defs::TILE_SIZE;

    unsigned int xMax;
    for (xMax = x_map; xMax < stage.max_x; xMax++)
    {
      if (stage.tileAction(xMax, y_map) == mm_tile_actions::TILE_SOLID)
      {
        break;
      }
    }
    maxX = (xMax-1)*mm_graphs_defs::TILE_SIZE;

    // which wall is closer, left or right?
    closer = (abs(xMin-x_map) < abs(xMax-x_map)) ? 0 : 1;
    isFacingRight = (closer == 0);
  }
  else
  {
    unsigned int yMin;
    for (yMin = y_map; yMin >= 0; yMin--)
    {
      if (stage.tileAction(x_map, yMin) == mm_tile_actions::TILE_SOLID)
      {
        break;
      }
    }
    minY = (yMin+1)*mm_graphs_defs::TILE_SIZE;

    unsigned int yMax;
    for (yMax = y_map; yMax < stage.max_y; yMax++)
    {
      if (stage.tileAction(x_map, yMax) == mm_tile_actions::TILE_SOLID)
      {
        break;
      }
    }
    maxY = (yMax-1)*mm_graphs_defs::TILE_SIZE;

    // which wall is closer, left or right?
    closer = (abs(yMin-y_map) < abs(yMax-y_map)) ? 0 : 1;
    isFacingDown = (closer == 0);
  }
}

bool OctopusBattery::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void OctopusBattery::horLogic()
{
  if (isFacingRight == true)
  {
    if (x < maxX)
    {
      x+=velx;
      if (x > maxX)
      {
        x = maxX;
      }
    }
    else
    {
      isFacingRight = false;
      curState = OctopusBattery::SLEEP;
      setAnimSeq(colorOffset + OctopusBattery::SLEEP);
    }
  }
  else
  {
    if (x > minX)
    {
      x-=velx;
      if (x < minX)
      {
        x = minX;
      }
    }
    else
    {
      isFacingRight = true;
      curState = OctopusBattery::SLEEP;
      setAnimSeq(colorOffset + OctopusBattery::SLEEP);
    }
  }
}

void OctopusBattery::verLogic()
{
  if (isFacingDown == true)
  {
    if (y < maxY)
    {
      y+=vely;
      if (y > maxY)
      {
        y = maxY;
      }
    }
    else
    {
      isFacingDown = false;
      curState = OctopusBattery::SLEEP;
      setAnimSeq(colorOffset + OctopusBattery::SLEEP);
    }
  }
  else
  {
    if (y > minY)
    {
      y-=vely;
      if (y < minY)
      {
        y = minY;
      }
    }
    else
    {
      isFacingDown = true;
      curState = OctopusBattery::SLEEP;
      setAnimSeq(colorOffset + OctopusBattery::SLEEP);
    }
  }
}


void OctopusBattery::doLogic()
{
  bool cycleDone = false;

  switch(curState)
  {
    case OctopusBattery::SLEEPING:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = OctopusBattery::ACTIVE;
        setAnimSeq(colorOffset + OctopusBattery::ACTIVE);
      }
    }
    break;
    case OctopusBattery::ACTIVE:
    {
      // If it's the first frame, let it play, the second frame dont change 
      // until it's time to sleep again  i.e: reach the other wall.
      if (curAnimFrame == 0)
      {
        cycleDone = Character::handleAnimation();
      }

      if (m_orientation == OctopusBattery::HORIZONTAL)
      {
        horLogic();
      }
      else
      {
        verLogic();
      }
    }
    break;
    case OctopusBattery::SLEEP:
    {
      cycleDone = Character::handleAnimation();
      if (cycleDone == true)
      {
        curState = OctopusBattery::SLEEPING;
        setAnimSeq(colorOffset + OctopusBattery::SLEEPING);
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

void OctopusBattery::doGravitation()
{
  return;
}

void OctopusBattery::adjustToWall()
{
  int tilecoordx, tilecoordy, tiletype;
  int dir = (closer == 0) ? -1 : 1;
  int w   = (closer == 0) ?  0 : utilXLen;
  int h   = (closer == 0) ?  0 : this->h;

  if (m_orientation == OctopusBattery::HORIZONTAL)
  {
    while(true)
    {
      if(collisionVer(*cur_stage, (x+utilX)+velx+w, y, tilecoordx, tilecoordy, tiletype) == false)
      {
        x += dir;
      }
      else
      {
        velx = 5;
        break;
      }
    }
  }
  else
  {
    while(true)
    {
      if(collisionHor(*cur_stage, x, y+h, tilecoordx, tilecoordy, (closer!=0), tiletype) == false)
      {
        y += dir;
      }
      else
      {
        vely = 5;
        break;
      }
    }
  }
}

void OctopusBattery::hit(mm_weapons::weapon_st * pWeapon)
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

void OctopusBattery::respawn()
{
  life  = 5;
  //alive = true;

  this->x = this->old_x;
  this->y = this->old_y;

  isFacingRight = (closer == 0);
  isFacingDown  = (closer == 0);

  resetAnimSeq(colorOffset + OctopusBattery::SLEEPING);
  curState = OctopusBattery::SLEEPING;

  return;
}

