/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <stdio.h>

#include "player.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "spritefiles.h"
#include "energybar.h"

#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"

//Player::Player() : Character("game_data/megaman_seq.dat"), lives(2)
Player::Player(const Stage & stage) : Character(stage, 0), lives(2)
{
  lockJumpAccel = false;
  curWeapon     = mm_weapons::W_MEGA_BUSTER;
  curState      = 0;
  alive         = true;

  isGettingOut  = false;
  isGettingIn   = false;

  life          = 28;

  isInvincible  = false;
  isHitAnimOn   = false;
  bHide         = false;
  bDraw         = true;

  blinkBegin = 0;
  blinkCount = 0;

  bDying = false;
  dyingTimer = 0;
  dieStep = 0;
}

void Player::reset()
{
  sx = 0;
  sy = 0;

  x = old_x;
  y = old_y;

  velx = mm_player_defs::VELMOVING;
  vely = 0;

  lockJumpAccel = false;
  curWeapon     = mm_weapons::W_MEGA_BUSTER;
  curState      = 0;
  alive         = true;

  isGettingOut  = false;
  isGettingIn   = false;
  isFacingRight = true;

  life          = 28;

  isInvincible  = false;
  isHitAnimOn   = false;
  bHide         = false;
  bDraw         = true;

  blinkBegin = 0;
  blinkCount = 0;

  bDying = false;
  dyingTimer = 0;
  dieStep = 0;

  touchSpike = false;
}

void Player::forceAnimation()
{
  Character::handleAnimation();
}

bool Player::handleAnimation(bool * bAnimationEnded)
{
  if (bDying == true) return false;

  if (this->grabstair == false && isHitAnimOn == false)
  {
    return Character::handleAnimation(bAnimationEnded);
  }
  return false;
}

void Player::hit(mm_weapons::weapon_st * pWeapon)
{
  if (isInvincible == true)
  {
    return;
  }

  //fprintf(stderr,"> Player HIT! By bullet[%d]\n", pWeapon->weapon);
}

void Player::hit(Character * pCharacter)
{
  int iUnits = 2;
  switch(pCharacter->type)
  {
    case mm_spritefiles::BIG_LIFE_RECHARGE_SPRITES:
      iUnits = 10;
    case mm_spritefiles::LITTLE_LIFE_RECHARGE_SPRITES:
    {
      pCharacter->die();
      if (curWeapon != mm_weapons::W_MEGA_BUSTER)
      {
        iUnits = 0;
      }
      EnergyBar::updateEnergyBar(curWeapon, iUnits);
      return;
    }
    break;
    case mm_spritefiles::BIG_WEAPON_RECHARGE_SPRITES:
      iUnits = 10;
    case mm_spritefiles::LITTLE_WEAPON_RECHARGE_SPRITES:
    {
      pCharacter->die();
      if (curWeapon == mm_weapons::W_MEGA_BUSTER)
      {
        iUnits = 0;
      }
      EnergyBar::updateEnergyBar(curWeapon, iUnits);
      return;
    }
    break;
    case mm_spritefiles::NEW_LIFE_SPRITES:
    {
      ++lives;
      pCharacter->die();
      Sounds::mm_sounds->play(NEW_LIFE);
      return;
    }
    break;
    case mm_spritefiles::BONUS_POINT_SPRITES:
    {
      //TODO: Increase bonus points.
      pCharacter->die();
      Sounds::mm_sounds->play(BONUS_POINT);
      return;
    }
    break;
  }

  // Do not cause harm to megaman.
  if (pCharacter->type == mm_spritefiles::MOVING_PLATFORM_SPRITES ||
      pCharacter->type == mm_spritefiles::FOOTHOLDER_SPRITES)
  {
    bool bOverPlatform = abs((this->y+mm_player_defs::PLAYERHEIGHT) - pCharacter->y) < 15;
    if ((this->vely >= 0) && (pCharacter->isPlatform == true))
    {
      int offset = pCharacter->type == mm_spritefiles::MOVING_PLATFORM_SPRITES ? 4 : 3;
      if (bOverPlatform == true)// && pCharacter->isPlatform == true)
      {
        this->y = pCharacter->y-mm_player_defs::PLAYERHEIGHT + offset;
        onPlatform = true;
        this->conPlayer = pCharacter;
      }
      else
      {
        //pCharacter->hasPlayer = false;
      }
      return;
    }
    else
    {
      //pCharacter->hasPlayer = false;
      return;
    }
  }
  else if (pCharacter->type == mm_spritefiles::FIRE_V_WALL_SPRITES)
  {
    if (pCharacter->curState == Character::FREEZE)
    {
      return;
    }
  }
  else if (pCharacter->type == mm_spritefiles::BOSSDOOR_SPRITES)
  {
    // Let stage take control over megaman...
    this->conPlayer = pCharacter;
    return;
  }
  else if (pCharacter->type == mm_spritefiles::GUTSMANROCK_SPRITES)
  {
    //this->conPlayer = pCharacter;
    return;
  }

  if (isInvincible == true)
  {
    return;
  }

  Sounds::mm_sounds->play(MM_HIT); 
  setAnimSeq(Player::GETHIT);  
  isInvincible = true;
  isHitAnimOn  = true;

  grabstair = false;

  // If hit occur on the left, player is thrown X positive otherwise
  // it's thrown X negative.
  hitDir = (this->x < pCharacter->x) ? -1 : 1;
  // Face hit.
  isFacingRight = (hitDir < 0);

  // Add hit explosion character.
  TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::HIT_EXPLOSION_CHAR, this->x, this->y));
}

void Player::checkOnCamera()
{
  return;
}

void Player::setAnimSeq(int newAnimSeq, bool reset)
{
  if (isHitAnimOn == false)
  {
    Character::setAnimSeq(newAnimSeq, reset);
  }
}

void Player::doGravitation()
{
  if (bDying == false)
  {
    Character::doGravitation();
  }
}

int Player::handType()
{
  switch(curWeapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
    case mm_weapons::W_ICEMAN_GUN:
    case mm_weapons::W_ELECMAN_GUN:
    case mm_weapons::W_FIREMAN_GUN:
    case mm_weapons::W_PLATFORM_CREATOR:
      return 1;
    break;

    case mm_weapons::W_GUTSMAN_GUN:
    case mm_weapons::W_BOMBMAN_GUN:
    case mm_weapons::W_CUTMAN_GUN:
      return 0;
    break;

    default:
      return 1;
  }
}

void Player::normalLogic()
{
  if (bDying == true) return;

  if (touchSpike == true)
  {
    dyingTimer = Clock::clockTicks;
    bDying = true;
    return;
  }

  if (isHitAnimOn == true)
  {
    bool bAnimEnded = false;

    bool collide = false;
    int tilecoordx, tilecoordy, tiletype;
    if (hitDir < 0)
    {
      collide = collisionVer((x+utilX)-1, y, tilecoordx, tilecoordy, tiletype);
    }
    else
    {
      collide = collisionVer((x+utilX)+1+utilXLen, y, tilecoordx, tilecoordy, tiletype);
    }

    if (collide == false)
    {
      this->x += hitDir;
    }

    Character::handleAnimation(&bAnimEnded);
    if (bAnimEnded == true)
    {
      isHitAnimOn = false;
      bHide       = true;
      blinkBegin  = Clock::clockTicks;
      blinkCount  = 0;
    }

    return;
  }

  if (isInvincible == true && bHide == true)
  {
    if (Clock::clockTicks-3 > blinkBegin)
    {
      blinkBegin = Clock::clockTicks;

      bDraw = !bDraw;
      ++blinkCount;
      if (blinkCount == 28)
      {
        bDraw = true;
        bHide = false;
        isInvincible = false;
      }
    }
  }

  if (Clock::clockTicks-16 > lastShot && firing == true)
  {
    firing = false;

    if (curAnimSeq == Player::FIRINGSTAIR || curAnimSeq == Player::FIRINGSTILLHAND)
      setAnimSeq(Player::UPDOWNSTAIR);
  }

  if (firing == true)
  {
    if (grabstair == true)
    {
      if (handType())
        setAnimSeq(Player::FIRINGSTAIR);
      else
        setAnimSeq(Player::FIRINGSTAIRHAND);
    }
  }

  int tilecoordx = 0;
  int tilecoordy = 0;
  int tiletype  = 0;
  
  checkStair();

  if (cur_stage->horz_scroll == false)
  {
    if (key[KEY_RIGHT])
    {
      isFacingRight = true;

      if (grabstair == false)
      {
        if (onground == true)
        {
          if (firing == false)
          {
            if (curAnimSeq != Player::RUNNING)
            {
              if ((curAnimSeq != Player::STARTINGRUN) &&
                  (curAnimSeq != Player::FIRINGRUNNING))
                setAnimSeq(Player::STARTINGRUN);
              else
              {
                if ((curAnimFrameDuration == 0) ||
                    (curAnimSeq == Player::FIRINGRUNNING))
                  setAnimSeq(Player::RUNNING);
              }
            }
            else
              setAnimSeq(Player::RUNNING);
          }
          else
          {
            if (handType())
              setAnimSeq(Player::FIRINGRUNNING, false);
            else
              setAnimSeq(Player::FIRINGSTILLHAND);
          }
        }

        if(!collisionVer((x+utilX)+velx+utilXLen, y, tilecoordx, tilecoordy, tiletype))
        {
          if (curAnimSeq != Player::FIRINGSTILLHAND)
            x += velx;
        }
      }
    }
    else if (key[KEY_LEFT])
    {
      isFacingRight = false;

      if (grabstair == false)
      {
        if (onground == true)
        {
          if (firing == false)
          {
            if (curAnimSeq != Player::RUNNING)
            {
              if ((curAnimSeq != Player::STARTINGRUN) && 
                  (curAnimSeq != Player::FIRINGRUNNING))
                setAnimSeq(Player::STARTINGRUN);
              else
              {
                if ((curAnimFrameDuration == 0) || 
                    (curAnimSeq == Player::FIRINGRUNNING))
                  setAnimSeq(Player::RUNNING);
              }
            }
            else
              setAnimSeq(Player::RUNNING);
          }
          else
          {
            if (handType())
              setAnimSeq(Player::FIRINGRUNNING, false);
            else
              setAnimSeq(Player::FIRINGSTILLHAND);
          }
        }

        if(!collisionVer((x+utilX)-velx, y, tilecoordx, tilecoordy, tiletype))
        {
          if (curAnimSeq != Player::FIRINGSTILLHAND)
            x -= velx;
        }
      }
    }
    else
    {
      //if (lockjump == false && grabstair == false)
      if (onground == true && grabstair == false)
      {
        //TODO: only fire again after already still, so you cant stop and continue firing without stop.
        if (firing == false)
          setAnimSeq(Player::STANDSTILL);
        else
        {
          if (handType())
            setAnimSeq(Player::FIRINGSTILL);
          else
            setAnimSeq(Player::FIRINGSTILLHAND);
        }
      }
    }

    if (isGettingIn == true)
    {
      if (Character::handleAnimation() == true)
      {
        isGettingIn = false;
      }
    }
    else if (grabstair == true && firing == false)
    {
      if (isGettingOut == false)
        setAnimSeq(Player::UPDOWNSTAIR);
    }


    //TODO: Should group those two(up,down) under firing = false hehehe...
    if (key[KEY_UP] && firing==false)//!key[KEY_A])
    {
      if(collisionStair(x+utilX, y, tilecoordx, tilecoordy, tiletype, true) == true)
      {
        vely = 0;

        Character::handleAnimation(); // While on stairs auto animation is off
        grabstair = true;

        if (tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
        {
          // if next up tile is not an stair tile.
          if(cur_stage->tileActionUnnormalized(x+utilX, y+8) != mm_tile_actions::TILE_STAIR_BEGIN)
          {
            setAnimSeq(Player::ONSTAIR);
            isGettingOut = true;
            if ((( ((int)y) % mm_graphs_defs::TILE_SIZE) < 4) ||
                (cur_stage->tileActionUnnormalized(x+utilX, y+mm_graphs_defs::TILE_SIZE) != mm_tile_actions::TILE_STAIR_BEGIN))
            {
              y-= 6;
              y-= ((int)y)%mm_graphs_defs::TILE_SIZE;
              y = (y / mm_graphs_defs::TILE_SIZE) * mm_graphs_defs::TILE_SIZE;
              isGettingOut = false;
              onground  = true;
              grabstair = false;
              vely = 8;
              setAnimSeq(Player::STANDSTILL);
            }
          }
          else
          {
            isGettingOut = false;
          }
        }

        x    = (tilecoordx * mm_graphs_defs::TILE_SIZE+1) - utilX;
        y   -= 2;
        isFacingDown  = true;
      }
    }
    else if (key[KEY_DOWN] && firing==false)
    {
      // If player is colliding with something and it's a stair tile OR if player 
      // is on the last stair step tile(with nothing bellow) then keep holding on it.
      if((collisionHor(x+utilX+(isFacingRight?8:0), y+vely+h, tilecoordx, tilecoordy, true, tiletype) &&
        (tiletype == mm_tile_actions::TILE_STAIR || tiletype == mm_tile_actions::TILE_STAIR_BEGIN) &&
        (( (int)(x+utilX+(isFacingRight?8:0)) % mm_graphs_defs::TILE_SIZE) < (mm_graphs_defs::TILE_SIZE/2))) ||
        (overstair == true && tiletype == 0))
      {
        vely = 0;

        Character::handleAnimation(); // While on stairs auto animation is off
        grabstair    = true;
        isGettingOut = false;

        if (tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
        {
          y+=16;
          if (isGettingIn == false)
          {
            isGettingIn  = true;
            setAnimSeq(Player::ENTERINGSTAIR);
          }
        }
        else
        {
          if (isGettingIn == false) setAnimSeq(Player::UPDOWNSTAIR);
        }

        isFacingDown = true;
        collisionStair(x+utilX, y, tilecoordx, tilecoordy, tiletype, true);
        x = (tilecoordx * mm_graphs_defs::TILE_SIZE+1) - utilX;
        y += 2;
      }
      else if (grabstair == true)
      {
        grabstair = false;
      }
    }

    static bool spacePressed = false;
    if (key[KEY_SPACE])// && (!key[KEY_UP] && !key[KEY_DOWN]))
    {
      isGettingOut = isGettingIn = false;

      spacePressed = true;
      static int jumpHigh = 0;
      //Cur: 56 Shold be: 65??
      if (isFacingDown == false && lockJumpAccel == false && (jumpHigh-y < 65))
      {
        onPlatform = false;

        if (firing == false)
          setAnimSeq(Player::JUMPING);
        else
        {
          if (handType())
            setAnimSeq(Player::FIRINGJUMP);
          else
            setAnimSeq(Player::FIRINGJUMPHAND);
        }

        vely=-8;
      }
      else
      {
        if (lockjump == true) {lockJumpAccel = true;}
      }

      if (grabstair == true)
      {
        if (!key[KEY_UP] && !key[KEY_DOWN])
          grabstair = false;
      }
      else if (lockjump == false)
      {
        jumpHigh = y;

        if (firing == false)
          setAnimSeq(Player::JUMPING);
        else
        {
          if (handType())
            setAnimSeq(Player::FIRINGJUMP);
          else
            setAnimSeq(Player::FIRINGJUMPHAND);
        }

        vely     = -8;
        lockjump = true; 
      }
    }
    else
    {
      if (spacePressed == true)
      {
        lockJumpAccel = true;
        if (onground == true) spacePressed = false;
      }
    }

    static bool fireKeyPressed = false;
    if (key[KEY_A] && fireKeyPressed == false)
    {
      fireKeyPressed = true;
      fire();

      if (firing == true)
      {
        if (onground == true) 
        {
          if (curAnimSeq == Player::STANDSTILL)
          {
            if (handType())
              setAnimSeq(Player::FIRINGSTILL);
            else
              setAnimSeq(Player::FIRINGSTILLHAND);
          }
          else if (curAnimSeq == Player::RUNNING)
          {
            if (handType())
              setAnimSeq(Player::FIRINGRUNNING, false);
            else
              setAnimSeq(Player::FIRINGSTILLHAND);
          }
        }
        else if (grabstair == false)
        {
          if (handType())
            setAnimSeq(Player::FIRINGJUMP);
          else
            setAnimSeq(Player::FIRINGJUMPHAND);
        }
      }
    }
    else if ((!key[KEY_A]) && (fireKeyPressed == true))
    {
      fireKeyPressed = false;
    }
  }
}

struct pt {int x; int y;};
void Player::die()
{
  if (Clock::clockTicks-7 > dyingTimer)
  {
    static pt explosionPts[] = {{mm_player_defs::PLAYERWIDTH>>1,mm_player_defs::PLAYERHEIGHT>>1},
                                {0,0},
                                {mm_player_defs::PLAYERWIDTH,0}};
    dyingTimer = Clock::clockTicks;
    if (dieStep < 3)
    {
      bool bOneCicle = true;
      Character * pExplosion = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, 
                                                          this->x+explosionPts[dieStep].x, this->y+explosionPts[dieStep].y,
                                                          0,0,(void*)&bOneCicle);
      TemporaryCharacterList::mm_tempCharacterLst.push_back(pExplosion);
      ++dieStep;
    }
    else
    {
      int iSize = 8;
      int xpos, ypos;
      xpos = this->x+explosionPts[0].x;
      ypos = this->y+explosionPts[0].y;

      Character * pExplosion01 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos, -10, 0);
      Character * pExplosion02 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos,  10, 0);
      Character * pExplosion03 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos-iSize,  0, -10);
      Character * pExplosion04 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos+iSize,  0, 10);

      iSize+=12;

      Character * pExplosion05 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos, -16, 0);
      Character * pExplosion06 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos,  16, 0);
      Character * pExplosion07 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos-iSize,  0, -16);
      Character * pExplosion08 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos+iSize,  0, 16);

      iSize-=6;
      
      Character * pExplosion09 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos-iSize, -14, -14);
      Character * pExplosion10 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos+iSize, -14, 14);
      Character * pExplosion11 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos-iSize, 14, -14);
      Character * pExplosion12 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos+iSize, 14, 14);

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

      Sounds::mm_sounds->play(MM_DIE);
      alive = false;
    }
  }
}

void Player::doLogic()
{
  if (bDying == true)
  {
    die();
  }
  else
  {
    normalLogic();
    onPlatform = false;
  }
}

bool Player::collisionStair(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing)
{
  int realFrameHeight = getFrameH();
  y = (y + (h-realFrameHeight));

  int tileypixels = y-(y%mm_graphs_defs::TILE_SIZE);
  int testend     = y + realFrameHeight;//y + getFrameH();

  tilecoordx = x + (utilXLen / 2.0f);
  tilecoordx /= mm_graphs_defs::TILE_SIZE;

  tilecoordy = tileypixels/mm_graphs_defs::TILE_SIZE;
  while(tileypixels <= testend)
  {
    tiletype = cur_stage->tileAction(tilecoordx, tilecoordy);
    if(tiletype == mm_tile_actions::TILE_STAIR || tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
    {
      return true;
    }

    tilecoordy++;
    tileypixels += mm_graphs_defs::TILE_SIZE;
  }

  return false;
}

bool Player::checkStair()
{
  static int tilecoordx, tilecoordy, tiletype;
  return (this->overstair = collisionStair(x+utilX, y, tilecoordx, tilecoordy, tiletype));
}

bool Player::canJumpAgain()
{
  bool bCanJumpAgain = (!key[KEY_SPACE]);

  if (bCanJumpAgain == true && onground == true)
  {
    lockJumpAccel = false;
  }

  return (bCanJumpAgain);
}

void Player::fire()
{
  //TODO: Check if we are shoting in the air (sync animation and bullet)
  switch(curWeapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
    {
      if (GlobalGameState::playerShots.size() < 3)
      {
        Sounds::mm_sounds->play(MEGA_BUSTER);

        mm_weapons::createMegaBuster(this);

        firing = true;
        lastShot = Clock::clockTicks; 
      }
    }
    break;
    case mm_weapons::W_ICEMAN_GUN:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_ICEMAN_GUN] > 0))
      {
        weapons[mm_weapons::W_ICEMAN_GUN]--;
        Sounds::mm_sounds->play(ICE_SLASHER);

        mm_weapons::createIceSlasher(this);

        firing = true;
        lastShot = Clock::clockTicks; 
      }
    }
    break;
    case mm_weapons::W_GUTSMAN_GUN:
    {
      if ((conPlayer != NULL) && (conPlayer->type == mm_spritefiles::GUTSMANROCK_SPRITES))
      {
        conPlayer->life = 0;
      }
    }
    break;
    case mm_weapons::W_BOMBMAN_GUN:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_BOMBMAN_GUN] > 0))
      {
        weapons[mm_weapons::W_BOMBMAN_GUN]--;

        mm_weapons::createHyperBomb(this);

        firing = true;
        lastShot = Clock::clockTicks;
      }
    }
    break;
    case mm_weapons::W_ELECMAN_GUN:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_ELECMAN_GUN] > 0))
      {
        weapons[mm_weapons::W_ELECMAN_GUN]--;
        Sounds::mm_sounds->play(THUNDER_BEAM);

        mm_weapons::createThunderBeam(this);

        firing = true;
        lastShot = Clock::clockTicks;
      }
    }
    break;
    case mm_weapons::W_CUTMAN_GUN:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_CUTMAN_GUN] > 0))
      {
        weapons[mm_weapons::W_CUTMAN_GUN]--;
        Sounds::mm_sounds->play(ROLLING_CUTTER);

        mm_weapons::createRollingCutter(this);

        firing = true;
        lastShot = Clock::clockTicks;
      }
    }
    break;
    case mm_weapons::W_FIREMAN_GUN:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_FIREMAN_GUN] > 0))
      {
        weapons[mm_weapons::W_FIREMAN_GUN]--;
        Sounds::mm_sounds->play(FIRE_STORM);

        mm_weapons::createFireStorm(this);

        firing = true;
        lastShot = Clock::clockTicks;
      }
    }
    break;
    case mm_weapons::W_PLATFORM_CREATOR:
    {
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_PLATFORM_CREATOR] > 0))
      {
        weapons[mm_weapons::W_PLATFORM_CREATOR]--;
        Sounds::mm_sounds->play(LIGHTNING);

        mm_weapons::createMagnetBeam(this);

        firing = true;
        lastShot = Clock::clockTicks;
      }
    }
    break;
    default:
      if (GlobalGameState::playerShots.size() == 0)
      {
        firing = true;
        lastShot = Clock::clockTicks; 
      }
    break;
  }

  //if ((firing == false)) //&& (Clock::clockTicks-30 > lastShot))
  //{
  //  firing = true;
  //}
}

void Player::drawCharacter(BITMAP * bmp)
{
  if (bDraw == false)
  {
    return;
  }

  int curSpriteFrame = anim_seqs[curAnimSeq][curAnimFrame].frameNumber;

  int xpos = this->sx;

  switch(curAnimSeq)
  {
    case Player::FIRINGRUNNING:
    case Player::FIRINGSTILL:
    case Player::FIRINGSTILLHAND:
    {
      xpos += isFacingRight ? 10 : -10;
    }
    break;
  }

  if (isFacingRight == true)
  {
    draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), xpos, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, this->spriteSheet->getFrame(curSpriteFrame), xpos, this->sy);
  }

#ifdef DEBUG
  int xrect = (xpos + this->utilX);

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

  rect(bmp, xpos, this->sy, xpos + getFrameW(), this->sy + mm_player_defs::PLAYERHEIGHT, makecol(255,0,0));
  rectfill(bmp, xrect, this->sy, xrect + this->utilXLen, this->sy + mm_player_defs::PLAYERHEIGHT, makecol(0,255,0));

  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
#endif
}

void Player::loadWeapon()
{
  // Do nothing, stage take care of this for player(megaman).
  return;
}

void Player::firingOnJump()
{
  if (firing == false)
    setAnimSeq(Player::JUMPING);
  else
  {
    if (handType())
      setAnimSeq(Player::FIRINGJUMP);
    else
      setAnimSeq(Player::FIRINGJUMPHAND);
  }
}

void Player::touchGround()
{
  setAnimSeq(Player::RUNNING);
  if (onPlatform == false)
  {
    Sounds::mm_sounds->play(MM_JUMP);
  }
}

void Player::touchCelling()
{
  lockJumpAccel = true;
}

void Player::changeWeapon()
{
  spriteSheet->changeSprite(curWeapon);
}
