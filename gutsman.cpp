/*
 * Killocan 2016
 * http://killocan.blogspot.com
*/

#include "gutsman.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "gutsmangun.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"
#include "energybar.h"
#include "spritefiles.h"
#include "mm_math.h"
//Gutsman

Gutsman::Gutsman(const Stage & stage, int x_map, int y_map, void * param) : Character(stage, mm_spritefiles::GUTSMAN_SPRITES)
{
  EnergyBar::m_boss = this;
  this->x = x_map;
  this->y = y_map;

  this->vely = 0;
  this->velx = 3;

  overstair     = false;
  isFacingRight = false;

  jumping       = false;
  jumping_right = false;

  curState = Gutsman::SHOW_OFF;
  setAnimSeq(Gutsman::SHOW_OFF);
  h = getFrameH();
  w = getFrameW();

  life  = 1;
  alive = true;

  bFirstTime = true;

  cycleCount = 0;

  GutsmanGun::firstOne = true;
  canCollideBullet = true;

  dyingTimer = 0;
  dieStep = 0;
  bDying = false;

  isHitAnimOn = false;
  bDraw = true;
  blinkBegin = 0;
  blinkCount = 0;
}

bool Gutsman::handleAnimation(bool * bAnimationEnded)
{
  return false;
}

void Gutsman::touchGround()
{
  if (bFirstTime == true)
  {
    bFirstTime = false;
    return;
  }

  if (GlobalGameState::earthquake == false)
  {
    Sounds::mm_sounds->play(EARTHQUAKE);
    GlobalGameState::earthquake = true;
    GlobalGameState::earthquakecount = 360;
  }
}

void Gutsman::doLogic()
{
  if (bDying == true)
  {
    die();
    return;
  }

  if (isHitAnimOn == true)
  {
    if (Clock::clockTicks - 3 > blinkBegin)
    {
      blinkBegin = Clock::clockTicks;

      bDraw = !bDraw;
      ++blinkCount;
      if (blinkCount == 16)
      {
        blinkCount = 0;
        bDraw = true;
        isHitAnimOn = false;
      }
    }
  }

  isFacingRight = (this->x > cur_stage->m_player->x) ? false : true;

  switch(curState)
  {
    case Gutsman::SHOW_OFF:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::DECIDING;
        setAnimSeq(Gutsman::DECIDING);
      }
    }
    break;
    case Gutsman::DECIDING:
    {
      int decision = rand() % 2;
      switch (decision)
      {
        case 0:
        {
          curState = Gutsman::PRE_JUMP;
          setAnimSeq(Gutsman::PRE_JUMP);
        }
        break;
        case 1:
        {
          curState = Gutsman::PRE_JUMP_ATTACK;
          setAnimSeq(Gutsman::PRE_JUMP_ATTACK);
        }
        break;
      }
    }
    break;
    case Gutsman::PRE_JUMP:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::JUMP;
        setAnimSeq(Gutsman::JUMP);
      }
    }
    break;
    case Gutsman::JUMP:
    {
      if (onground == true && jumping == false)
      {
        bool cycleDone = Character::handleAnimation();
        if (cycleDone == true)
        {
          this->vely = -13;
          jumping = true;
          int i = rand() % 2;
          jumping_right = (bool)i;
        }
      }
      else
      {
        int tilecoordx, tilecoordy, tiletype;

        // 10 offset avoid gutsman to stand too close of the wall and mess with the rock
        // cause it colide with the wall from the start.
        if (jumping_right == true)
        {
          if (collisionVer((x + utilX) + velx + utilXLen + 10.0f, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x += velx;
          }
        }
        else
        {
          if (collisionVer((x + utilX) - velx - 10.0f, y, tilecoordx, tilecoordy, tiletype) == false)
          {
            x -= velx;
          }
        }

        if (onground == true)
        {
          curState = Gutsman::DECIDING;
          setAnimSeq(Gutsman::DECIDING);
          jumping = false;
        }
      }
    }
    break;
    case Gutsman::PRE_JUMP_ATTACK:
    {
      bool cycleDone = false;
      Character::handleAnimation(&cycleDone);
      if (cycleDone == true)
      {
        curState = Gutsman::JUMP_ATTACK;
        setAnimSeq(Gutsman::JUMP_ATTACK);
      }
    }
    break;
    case Gutsman::JUMP_ATTACK:
    {
      if (onground == true && jumping == false)
      {
        bool cycleDone = Character::handleAnimation();
        if (cycleDone == true)
        {
          this->vely = -12;
          jumping = true;
        }
      }
      else
      {
        if (onground == true)
        {
          curState = Gutsman::ATTACK;
          setAnimSeq(Gutsman::ATTACK);
          jumping = false;

          rock = mm_weapons::createGutsmanRock(this, this->x + (isFacingRight ? -5.0f : 5.0f),
                                               GlobalCamera::mm_camera->y-64.0f, 
                                               0, 0, 0);
        }
      }
    }
    break;
    case Gutsman::ATTACK:
    {
      if (rock == NULL || rock->curState == GutsmanGun::ATTACHED_TO)
      {
        bool cycleDone = false;
        if (Character::handleAnimation(&cycleDone) && cycleCount < 2)
        {
          ++cycleCount;
          if (cycleCount == 2)
            fire();
        }
        if (cycleDone == true)
        {
          cycleCount = 0;
          curState = Gutsman::DECIDING;
          setAnimSeq(Gutsman::DECIDING);
        }
      }
    }
    break;
  }
}

void Gutsman::fire()
{
  if (rock->curState == GutsmanGun::ATTACHED_TO)
  {
    rock->launch();
    rock = NULL;
  }
}

void Gutsman::hit(mm_weapons::weapon_st * pWeapon)
{
  if (isHitAnimOn == true) return;

  switch(pWeapon->weapon)
  {
    case mm_weapons::W_ICEMAN_GUN:
    {
      freeze();
    }
    break;
    case mm_weapons::W_MEGA_BUSTER:
    default:
      life-=6;
    break;
  }
  pWeapon->alive = false;

  if (life <= 0)
  {
    bDying = true;
    dyingTimer = Clock::clockTicks;
    die();
  }

  isHitAnimOn = true;
  Sounds::mm_sounds->play(MM_HIT);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::HIT_EXPLOSION_CHAR, 
                                                                                   this->x, this->y - 5.0f));
}

void Gutsman::drawCharacter(BITMAP * bmp)
{
  int curSpriteFrame = bDraw ? anim_seqs[curAnimSeq][curAnimFrame].frameNumber : spriteSheet->frameNumber() - 1;

  if (isFacingRight == true)
  {
    draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, this->spriteSheet->getFrame(curSpriteFrame), this->sx, this->sy);
  }
}

void Gutsman::die()
{
  if (Clock::clockTicks - 7 > dyingTimer)
  {
    MM_Math::pt explosionPts[] = { {mm_player_defs::PLAYERWIDTH >> 1,mm_player_defs::PLAYERHEIGHT >> 1},
                                {0,0},
                                {mm_player_defs::PLAYERWIDTH,0} };
    dyingTimer = Clock::clockTicks;
    if (dieStep < 3)
    {
      Character * pExplosion = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR,
        this->x + explosionPts[dieStep].x, this->y + explosionPts[dieStep].y,
        0, 0);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion);
      ++dieStep;
    }
    else
    {
      bool bOneCicle = false;

      int iSize = 8;
      int xpos, ypos;
      xpos = this->x + explosionPts[0].x;
      ypos = this->y + explosionPts[0].y;

      Character * pExplosion01 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos - iSize, ypos, -10, 0, (void*)&bOneCicle);
      Character * pExplosion02 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos + iSize, ypos, 10, 0, (void*)&bOneCicle);
      Character * pExplosion03 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos, ypos - iSize, 0, -10, (void*)&bOneCicle);
      Character * pExplosion04 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos, ypos + iSize, 0, 10, (void*)&bOneCicle);

      iSize += 12;

      Character * pExplosion05 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos - iSize, ypos, -19, 0, (void*)&bOneCicle);
      Character * pExplosion06 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos + iSize, ypos, 19, 0, (void*)&bOneCicle);
      Character * pExplosion07 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos, ypos - iSize, 0, -19, (void*)&bOneCicle);
      Character * pExplosion08 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos, ypos + iSize, 0, 19, (void*)&bOneCicle);

      iSize -= 6;

      Character * pExplosion09 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos - iSize, ypos - iSize, -14.5, -14.5, (void*)&bOneCicle);
      Character * pExplosion10 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos - iSize, ypos + iSize, -14.5, 14.5, (void*)&bOneCicle);
      Character * pExplosion11 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos + iSize, ypos - iSize, 14.5, -14.5, (void*)&bOneCicle);
      Character * pExplosion12 = cur_stage->createCharacter(mm_tile_actions::EXPLOSION_LITTLE_CHAR, xpos + iSize, ypos + iSize, 14.5, 14.5, (void*)&bOneCicle);

      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion01);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion02);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion03);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion04);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion05);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion06);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion07);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion08);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion09);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion10);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion11);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion12);

      //Sounds::mm_sounds->play(MM_DIE);
      alive = false;
      EnergyBar::m_boss = NULL;
    }
  }
}
