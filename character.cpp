/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <stdio.h>

#include "character.h"
#include "tileactions.h"
#include "camera.h"
#include "stage.h"
#include "defines.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "sfx.h"
#include "dropitem.h"

Character::Character(const Stage & stage, int TYPE) :
  x(0), y(0),
  old_x(0), old_y(0),
  sx(0), sy(0),
  h(0), w(0),
  velx(0), vely(0),
  utilX(0), utilXLen(0),
  life(0),
  curState(0),
  last_state(0),
  colorOffset(0),
  lastShot(0),
  curAnimFrame(0),
  curAnimFrameDuration(0),
  curAnimSeq(0),
  isFacingRight(true),isFacingDown(false),
  grabstair(false),
  overstair(false),
  lockjump(false),
  onground(false),
  firing(false),
  alive(false),
  onCamera(false),
  isInvincible(false),
  canCollidePlayer(false),
  canCollideBullet(false),
  collideWithPlayer(false),
  touchSpike(false),
  onPlatform(false),
  isPlatform(false),
  dropItem(true),
  conPlayer(NULL),
  spriteSheet(NULL),
  type(TYPE),
  xcol(0), ycol(0)
{
  cur_stage = const_cast<Stage*> (&stage);
  loadAnimSeqs(TYPE);
}

Character::Character() :
  x(0), y(0),
  old_x(0), old_y(0),
  sx(0), sy(0),
  h(0), w(0),
  velx(0), vely(0),
  utilX(0), utilXLen(0),
  life(0),
  curState(0),
  last_state(0),
  colorOffset(0),
  lastShot(0),
  curAnimFrame(0),
  curAnimFrameDuration(0),
  curAnimSeq(0),
  isFacingRight(false),isFacingDown(false),
  grabstair(false),
  overstair(false),
  lockjump(false),
  onground(false),
  firing(false),
  alive(false),
  onCamera(false),
  isInvincible(false),
  canCollidePlayer(false),
  canCollideBullet(false),
  collideWithPlayer(false),
  touchSpike(false),
  onPlatform(false),
  isPlatform(false),
  dropItem(true),
  conPlayer(NULL),
  cur_stage(NULL),
  spriteSheet(NULL),
  type(-1),
  xcol(0), ycol(0)
{
}

Character::~Character()
{
  //delete spriteSheet;
}

void Character::goRight()
{
  x+=2.0f;
}

void Character::doLogic()
{
  return;  
}

void Character::loadAnimSeqs(int TYPE)
{
  spriteSheet = cur_stage->getAnimSeq(TYPE);
  utilX       = spriteSheet->getUtilXInfo()[0];
  utilXLen    = spriteSheet->getUtilXInfo()[1];
  anim_seqs   = spriteSheet->getAnimSeq();
  isFacingDown = false;

  curAnimFrameDuration = getCurrFrameDuration();
}

void Character::calcScreenCoords() //const Camera & camera)
{
  this->sx = (this->x - GlobalCamera::mm_camera->x);
  this->sy = (this->y - GlobalCamera::mm_camera->y);
}

bool Character::canJumpAgain()
{
  return true;
}

void Character::die()
{
  int x,y;
  x = this->x + abs((getFrame()->w-32)/2);
  y = this->y + abs((getFrame()->h-32)/2);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, x, y));

  if (dropItem == true)
  {
    DropItem::dropItem(cur_stage, x, y);
  }
}

void Character::doGravitation()
{
  int tilecoordx, tilecoordy, tiletype;

  if (curState == Character::FREEZE) return;

  overstair = collisionStair(x+utilX, y, tilecoordx, tilecoordy, tiletype);

  // Grabstair is true only for megaman.
  if (grabstair == false && cur_stage->horz_scroll == false)
  {
    // Goind UP
    if(vely < 0)
    {
      onground = false;

      // If outside map don't test collision, just pretend its all void out there :)
      if((y+vely >= 0) && collisionHor(x+utilX, y+vely, tilecoordx, tilecoordy, false, tiletype))
      {
        touchCelling();

        y    = (tilecoordy+1)*mm_graphs_defs::TILE_SIZE + 1;
        vely = 0;
      }
      else
      {
        isFacingDown = false;
        y    += vely;
        vely += mm_player_defs::GRAVITATION;
      }
    }
    else
    {
      int current_height = h; //getFrameH();
      // If outside map don't test collision, just pretend its all void out there :)
      if((y+vely+h >= 0) &&
        //((collisionHor(x+utilX, y+vely+h, tilecoordx, tilecoordy, !overstair, tiletype)) || (onPlatform == true)))
        ((collisionHor(x+utilX, y+vely+current_height, tilecoordx, tilecoordy, !overstair, tiletype)) || (onPlatform == true)))
      {
        if (onground == false)
        {
          touchGround();
          onground = true;
        }

        if (onPlatform == false)
          y = tilecoordy*mm_graphs_defs::TILE_SIZE - h-1;

        // 1 so we test against the ground again 
        // in the next frame (0 would test against the ground in the next+1 frame)
        vely = 1;

        if(canJumpAgain())
        {
          lockjump = false;
        }
      }
      else
      {
        onground = false;

        firingOnJump();

        isFacingDown = true;
        y    += vely;
        vely += mm_player_defs::GRAVITATION;

        if(vely >= mm_graphs_defs::TILE_SIZE/2)
          vely = mm_graphs_defs::TILE_SIZE/2;

        lockjump = true;
      }
    }
  }
}

void Character::firingOnJump()
{
  return;
}

void Character::touchGround()
{
  return;
}

void Character::touchCelling()
{
  return;
}

bool Character::collisionHor(int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype)
{
  int tilexpixels = x - (x % mm_graphs_defs::TILE_SIZE);
  int testend     = x + utilXLen;

  tilecoordy = y / mm_graphs_defs::TILE_SIZE;
  tilecoordx = tilexpixels/mm_graphs_defs::TILE_SIZE;
  while(tilexpixels <= testend)
  {
    tiletype = cur_stage->tileAction(tilecoordx, tilecoordy);

    if (tiletype == mm_tile_actions::TILE_DEATH)
    {
      touchSpike = true;
    }

    if (going_down == false)
    {
      if(tiletype == mm_tile_actions::TILE_SOLID)
        return true;
    }
    else
    {
      //tiletype = cur_stage->tileAction(tilecoordx, tilecoordy);

      if (this->overstair == true)
      {
        if((tiletype == mm_tile_actions::TILE_SOLID)       || 
           (tiletype == mm_tile_actions::TILE_STAIR_BEGIN) ||
           (tiletype == mm_tile_actions::TILE_STAIR))
          return true;
      }
      else
      {
        if((tiletype == mm_tile_actions::TILE_SOLID) || 
           (tiletype == mm_tile_actions::TILE_STAIR_BEGIN))
          return true;
      }
    }

    ++tilecoordx;
    tilexpixels += mm_graphs_defs::TILE_SIZE;
  }

  return false;
}

bool Character::collisionVer(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype)
{
  int realFrameHeight = getFrameH();
  y = (y + (h-realFrameHeight));
  //printf("realFrameHeight = [%d] y = [%d] testend= [%d]\n", realFrameHeight, y, ((y + (h-realFrameHeight)) + realFrameHeight));

  int tileypixels = y-(y%mm_graphs_defs::TILE_SIZE);
  int testend     = y + realFrameHeight;

  tilecoordx = x / mm_graphs_defs::TILE_SIZE;

  tilecoordy = tileypixels/mm_graphs_defs::TILE_SIZE;
  while(tileypixels <= testend)
  {
    tiletype = cur_stage->tileAction(tilecoordx, tilecoordy);

    if(tiletype == mm_tile_actions::TILE_SOLID)
    {
      return true;
    }
    else if (tiletype == mm_tile_actions::TILE_DEATH)
    {
      touchSpike = true;
    }

    tilecoordy++;
    tileypixels += mm_graphs_defs::TILE_SIZE;
  }

  return false;
}

bool Character::collisionStair(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing)
{
  return false;
}

bool Character::checkStair()
{
  return false;
}

void Character::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;

  //blit(this->spriteSheet->getFrame(curSpriteFrame), bmp, 0, 0, this->sx, this->sy, 
  //this->spriteSheet->getFrame(curSpriteFrame)->w, this->spriteSheet->getFrame(curSpriteFrame)->h);

  if (isFacingRight == true)
  {
    draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
}

int Character::getFrameH()
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  //return (this->spriteSheet->getFrame(curSpriteFrame)->h);
  return (this->spriteSheet->getFrameHeight(curSpriteFrame));
}

int Character::getFrameW()
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  return (this->spriteSheet->getFrame(curSpriteFrame)->w);
}

BITMAP * Character::getFrame()
{
  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;
  return (this->spriteSheet->getFrame(curSpriteFrame)); 
}

bool Character::handleAnimation(bool * bAnimationEnded)
{
  bool bEnded = false;

  if (this->curAnimFrameDuration == 0)
  {
    if (curState == Character::FREEZE)
    {
      curState = last_state;
      defrost();
    }

    // Either or not the anim cicle ended.
    bEnded = this->nextAnimFrame();

    if (bAnimationEnded != NULL)
    {
      *bAnimationEnded = bEnded;
    }

    // Curr frame ended
    return true;
  }
  else
  {
    this->curAnimFrameDuration--;
  }

  return false;
}

bool Character::nextAnimFrame()
{
  if ((unsigned int)(curAnimFrame+1) < anim_seqs[curAnimSeq].size())
  {
    curAnimFrame++;
    curAnimFrameDuration = getCurrFrameDuration();

    //h = getFrameH();

    return false;
  }
  else
  {
    curAnimFrame = 0;
    curAnimFrameDuration = getCurrFrameDuration();

    //h = getFrameH();

    return true;
  }
}

void Character::handleFreeze()
{
  if (Character::handleAnimation() == true)
  {
    //curState = last_state;
    resetAnimSeq(colorOffset + curState);
  }
}

void Character::freeze()
{
  if (curState != Character::FREEZE)
  {
    last_state = curState;
    curState  = Character::FREEZE;
    curAnimFrameDuration  = 200; // freeze in the current sprite.
  }
}

void Character::defrost()
{
  return;
}

int Character::getCurrFrameDuration()
{
  return anim_seqs[curAnimSeq][curAnimFrame].frameDuration;
}

void Character::setAnimSeq(int newAnimSeq, bool reset)
{
  if ((unsigned int)newAnimSeq < anim_seqs.size() && newAnimSeq != curAnimSeq)
  {
    curAnimSeq = newAnimSeq;
    if (reset == true) curAnimFrame = 0;
    curAnimFrameDuration = getCurrFrameDuration();
  }
}

void Character::resetAnimSeq(int animSeq)
{
  if ((unsigned int) animSeq < anim_seqs.size())
  {
    curAnimSeq = animSeq;
    curAnimFrame = 0;
    curAnimFrameDuration = getCurrFrameDuration();
  }
}

void Character::fire()
{
  return;
}

void Character::loadWeapon()
{
  return;
}

void Character::changeWeapon()
{
  return;
}

void Character::hit(mm_weapons::weapon_st * pWeapon)
{
  return;
}

void Character::hit(Character * pCharacter)
{
  return;
}

void Character::respawn()
{
  return;
}

void Character::checkOnCamera()
{
  if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w &&
      this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
  {
    if (onCamera == false && alive == false)
    {
      alive = true;
    }

    onCamera = true;
  }
  else
  {
    if (alive == true)
    {
      alive = false;
      respawn();
    }

    // In the original game, if a character get it's first x coord
    // behind the camera it's marked as "outside" camera clipping.
    if (this->x >= GlobalCamera::mm_camera->x && this->x <= GlobalCamera::mm_camera->x+GlobalCamera::mm_camera->w &&
        this->y >= GlobalCamera::mm_camera->y && this->y <= GlobalCamera::mm_camera->y+GlobalCamera::mm_camera->h)
    {
      onCamera = true;
    }
    else
    {
      onCamera = false;
    }
  }
}
