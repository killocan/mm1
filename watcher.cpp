/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <math.h>
#include <stdio.h>

#include "watcher.h"

#include "stage.h"
#include "tileactions.h"
#include "defines.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

#include "spritefiles.h"

//Watcher

Watcher::WatcherFragment::WatcherFragment(const Stage & stage, int x, int y)
  : Character(stage, mm_spritefiles::WATCHER_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = this->x;
  this->old_y = this->y;

  overstair     = false;
  isFacingRight = true;
  colorOffset   = 0;

  setAnimSeq(Watcher::WatcherFragment::WAITING);
  h = getFrameH();
  w = getFrameW();

  alive = true;
  canCollidePlayer = true;
  canCollideBullet = true;

  curState = Watcher::WatcherFragment::WAITING;

  life = 1;
  vely = 2;
}

void Watcher::WatcherFragment::doLogic()
{
  if (cur_stage->m_player->alive == false)
  {
    this->alive = false;
    return;
  }

  int xpos = cur_stage->m_player->x;
  isFacingRight = (xpos > x);

  switch(curState)
  {
    case Watcher::WatcherFragment::WAITING:
    {
      vely = 2;
    }
    break;
    case Watcher::WatcherFragment::CHASING:
    case Watcher::WatcherFragment::OPENING:
    case Watcher::WatcherFragment::OPEN:
    {
      int ypos = cur_stage->m_player->y;
      int yh = 0;

      if (y > ypos)
      {
        y -= vely;
      }
      else
      {
        yh = mm_graphs_defs::TILE_SIZE-8;
        y += vely;
      }

      int dist = ypos - y - yh;
      dist = abs(dist);
      if (dist < (mm_graphs_defs::TILE_SIZE/2))
      {
        curState = FIRING;
      }
      else if (dist < mm_graphs_defs::TILE_SIZE)
      {
        curState = OPEN;
        setAnimSeq(Watcher::WatcherFragment::OPEN);
      }
      else if(dist < (mm_graphs_defs::TILE_SIZE*2))
      {
        curState = OPENING;
        setAnimSeq(Watcher::WatcherFragment::OPENING);
      }
      else
      {
        setAnimSeq(Watcher::WatcherFragment::CHASING);
      }
    }
    break;
    case Watcher::WatcherFragment::FIRING:
    {
      int svelx = xpos < this->x ? -6 : 6;

      TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::SPARKLE_ENM_CHAR, 
                                                                                       x,y, svelx,0));
      TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::SPARKLE_ENM_CHAR, 
                                                                                       x,y+h, svelx,0));

      curState = CLOSING;
      setAnimSeq(Watcher::WatcherFragment::CLOSING);
      vely = 4;
    }
    break;
    case Watcher::WatcherFragment::CLOSING:
    case Watcher::WatcherFragment::FLEEING:
    {
      if (y > old_y)
      {
        y -= vely;
      }
      else
      {
        y += vely;
      }

      if ((y > (GlobalCamera::mm_camera->y + GlobalCamera::mm_camera->h)) ||
          (y < (GlobalCamera::mm_camera->y)))
      {
#ifdef DEBUG
        fprintf(stderr,"WatcherFragment[%p] - Entrando em waiting (sai da tela)\n",this);
#endif
        curState = Watcher::WatcherFragment::WAITING;
        setAnimSeq(Watcher::WatcherFragment::WAITING);
        break;
      }

      int ypos = cur_stage->m_player->y;
      int dist = ypos - y;
      dist = abs(dist);
      if (dist > (2*mm_graphs_defs::TILE_SIZE))
      {
        curState = FLEEING;
        setAnimSeq(Watcher::WatcherFragment::FLEEING);
      }

    }
    break;
    case RESTING:
    {
      if (Character::handleAnimation() == true)
      {
        curState = Watcher::WatcherFragment::CHASING;
        resetAnimSeq(Watcher::WatcherFragment::CHASING);
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

void Watcher::WatcherFragment::doGravitation()
{
  return;
}

void Watcher::WatcherFragment::hit(mm_weapons::weapon_st * pWeapon)
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

  if (life <= 0)
  {
    die();
    x = old_x;
    y = old_y;
    resetAnimSeq(Watcher::WatcherFragment::WAITING);
    curState = Watcher::WatcherFragment::WAITING;
  }

  return;
}

void Watcher::WatcherFragment::checkOnCamera()
{
  if ((this->x >= GlobalCamera::mm_camera->x) &&
      (this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w) &&
      (cur_stage->horz_scroll == false))
  {
    alive = true;
  }
  else
  {
    x = old_x;
    y = old_y;
    //curState = Watcher::WatcherFragment::WAITING;
  }
}

void Watcher::WatcherFragment::resetState(int x, int y)
{
  this->x = x;
  this->y = y;
  this->old_x = this->x;
  this->old_y = this->y;
  respawn();
}

void Watcher::WatcherFragment::setWait()
{
  x = old_x;
  y = old_y;
  curState = Watcher::WatcherFragment::WAITING;
}

void Watcher::WatcherFragment::respawn()
{
  life  = 1;
  alive = true;

  curState = Watcher::WatcherFragment::RESTING;
  resetAnimSeq(Watcher::WatcherFragment::RESTING);

  return;
}

Watcher::Watcher(const Stage & stage, int x, int y) 
  : Character(stage, mm_spritefiles::WATCHER_SPRITES)
{
  this->x = x;
  this->y = y;

  this->old_x = x;
  this->old_y = y;

  curState = Watcher::CONTROLLING;

  internalSprite = create_bitmap(10,10);
#ifdef DEBUG
  clear_to_color(internalSprite, makecol(255,0,0));
#else
  clear_to_color(internalSprite, MASK_COLOR_24);
#endif

  fragments[0] = cur_stage->createCharacter(mm_tile_actions::WATCHER_FRAGMENT, x,y, 0,0);
  fragments[1] = cur_stage->createCharacter(mm_tile_actions::WATCHER_FRAGMENT, x,y, 0,0);
  fragments[2] = cur_stage->createCharacter(mm_tile_actions::WATCHER_FRAGMENT, x,y, 0,0);

  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[0]);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[1]);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(fragments[2]);

  childAlive = false;
  alive = false;
}

Watcher::~Watcher()
{
  destroy_bitmap(internalSprite);
  internalSprite = NULL;
}

void Watcher::doLogic()
{
  // center right left -> down - up
  // right left center -> up - down
  if (childAlive == false)
  {
    int yini = this->y;

    int xpos[] = 
    {
      x, x+(2*mm_graphs_defs::TILE_SIZE), x-(2*mm_graphs_defs::TILE_SIZE), // DOWN
      x+(mm_graphs_defs::TILE_SIZE), x-(2*mm_graphs_defs::TILE_SIZE), x    // UP
    };
    int ypos[] = 
    {
      yini+(2*mm_graphs_defs::TILE_SIZE), y+(6*mm_graphs_defs::TILE_SIZE), y+(12*mm_graphs_defs::TILE_SIZE), // DOWN 
      yini-(2*mm_graphs_defs::TILE_SIZE), y-(6*mm_graphs_defs::TILE_SIZE), y-(12*mm_graphs_defs::TILE_SIZE)  // UP
    };
    int px = cur_stage->m_player->x;
    int *ptsx = px < x ? xpos : &xpos[3];
    int *ptsy = px < x ? ypos : &ypos[3];

    childAlive = true;

    Watcher::WatcherFragment * cur;
    for (int i = 0; i < 3; ++i)
    {
      cur = dynamic_cast <Watcher::WatcherFragment*> (fragments[i]);
#ifdef DEBUG
      fprintf(stderr,"Watcher [%p] - criando fragmento em x[%d] y[%d]\n", this, ptsx[i], ptsy[i]);
#endif
      cur->resetState(ptsx[i], ptsy[i]);
    }
  }
}

BITMAP * Watcher::getFrame()
{
  return internalSprite;
}

bool Watcher::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Watcher::doGravitation()
{
  return;
}

void Watcher::respawn()
{
  Watcher::WatcherFragment * cur;
  for (int i = 0; i < 3; ++i)
  {
    cur = dynamic_cast <Watcher::WatcherFragment*> (fragments[i]);
    cur->setWait();
  }
  childAlive = false;
  
  return;
}

void Watcher::drawCharacter(BITMAP * bmp)
{
#ifdef DEBUG
  draw_sprite(bmp, internalSprite, sx, sy);
#endif
  return;
}
