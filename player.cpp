/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <cstdio>

#include "player.h"
#include "stage.h"
#include "tileactions.h"
#include "defines.h"
#include "spritefiles.h"
#include "energybar.h"
#include "magneticbeamhandler.h"
#include "gutsmangunmanager.h"
#include "scenesoundmanager.h"
#include "globals.h"
#include "globalgamestate.h"
#include "sfx.h"
#include "mm_math.h"

Player::Player(const Stage & stage) : Character(stage, 0)
{
  lockJumpAccel = false;
  curWeapon     = mm_weapons::W_MEGA_BUSTER;
  curState      = Player::SPAWNING;
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

  fireKeyPressed = false;

  conPlayer = NULL;

  this->sx = 0;
  this->sy = 0;
  this->h = mm_player_defs::PLAYERHEIGHT;
  this->w = mm_player_defs::PLAYERWIDTH;
  this->velx = 0; //mm_player_defs::VELMOVING;
  this->vely = 0;
  this->accelx = 0.0f;
  this->isFacingRight  = true;
  this->isFacingDown   = false;
  this->lockjump   = false;
  this->grabstair  = false;
  this->overstair  = false;
  this->holdingGutsmanRock = false;
  this->isStunned = false;
  this->lockmoves = false;
  this->justLeaveStair = false;
  this->onIce = false;
}

void Player::reset()
{
  lockmoves = false;

  sx = 0;
  sy = 0;

  x = old_x;
  y = old_y;

  velx = 0; //mm_player_defs::VELMOVING;
  vely = 0;
  accelx = 0.0f;

  lockJumpAccel = false;
  curWeapon     = mm_weapons::W_MEGA_BUSTER;
  curState      = Player::SPAWNING;
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

  fireKeyPressed = false;

  conPlayer = NULL;

  holdingGutsmanRock = false;
  isStunned = false;
  
  justLeaveStair = false;
  onIce = false;
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

#ifdef DEBUG
  fprintf(stderr,"> Player HIT! By bullet[%d]\n", pWeapon->weapon);
#endif
}

void Player::hit(Character * pCharacter)
{
  int iUnits = 2;
  switch(pCharacter->type)
  {
    case mm_spritefiles::STAGE_ORB_SPRITES:
    {
      Sounds::mm_sounds->play(BONUS_POINT);
      cur_stage->finished = true;
      pCharacter->alive = false;
      return;
    }
    break;

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
      ++GlobalGameState::lives;
      pCharacter->die();
      Sounds::mm_sounds->play(NEW_LIFE);
      return;
    }
    break;
    case mm_spritefiles::BONUS_POINT_SPRITES:
    {
      ++GlobalGameState::bonus_points;

      pCharacter->die();
      Sounds::mm_sounds->play(BONUS_POINT);
      return;
    }
    break;
  }

  // Do not cause harm to megaman.
  if (pCharacter->type == mm_spritefiles::MOVING_PLATFORM_SPRITES ||
      pCharacter->type == mm_spritefiles::FOOTHOLDER_SPRITES ||
      pCharacter->type == mm_spritefiles::TIMER_PLATFORM_SPRITES ||
        pCharacter->type == mm_spritefiles::WEAPONS_MAGNETIC)
  {
      float diff = 15.0f;
    if (pCharacter->type == mm_spritefiles::WEAPONS_MAGNETIC)
      diff = pCharacter->h*2.0f;

    bool bOverPlatform = (y < pCharacter->y && vely > 0.0f) && abs((this->y+mm_player_defs::PLAYERHEIGHT) - pCharacter->y) < diff;
    if (pCharacter->type == mm_spritefiles::WEAPONS_MAGNETIC)
      if (this->x + this->utilX + this->utilXLen < pCharacter->x)
        bOverPlatform = false;

    if (pCharacter->type == mm_spritefiles::TIMER_PLATFORM_SPRITES)
    {
      if (this->y+10.0f > pCharacter->y)
      {
        if (this->x < pCharacter->x) //isFacingRight)
        {
          this->x = (pCharacter->x - this->w)-5.0f;
          return;
        }
        else
        {
          this->x = ((pCharacter->x + pCharacter->w) - this->utilX)+2;
          return;
        }
      }
    }

    if ((this->vely >= 0) && (pCharacter->isPlatform == true))
    {
      int offset = (pCharacter->type == mm_spritefiles::MOVING_PLATFORM_SPRITES) ? 4 : 3;

      if (bOverPlatform == true)
      {
        this->y = pCharacter->y-mm_player_defs::PLAYERHEIGHT + offset;
        onPlatform = true;
        this->conPlayer = pCharacter;
      }
      return;
    }
    else
    {
      return;
    }
  }
  else if (pCharacter->type == mm_spritefiles::GUTSMANROCK_SPRITES)
  {
    if (this->holdingGutsmanRock == false)
      this->conPlayer = pCharacter;

    return;
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

  this->life -= 2;

  // Add hit explosion character.
  TemporaryCharacterList::mm_tempCharacterLst.push_back(cur_stage->createCharacter(mm_tile_actions::HIT_EXPLOSION_CHAR, this->x, this->y));
}

void Player::getStunned()
{
  if (!isStunned)
  {
    isStunned = true;
    setAnimSeq(Player::STUNNED);
  }
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

  bool outsideMap = false;
  int tilex = int(x) / mm_graphs_defs::TILE_SIZE;
  int tiley = int(y) / mm_graphs_defs::TILE_SIZE;
  if (tilex >= cur_stage->max_x || tiley >= cur_stage->max_y)
    outsideMap = true;
  
  if (touchSpike == true || this->life <= 0 || outsideMap)
  {
    dyingTimer = Clock::clockTicks;
    bDying = true;
    return;
  }

  if (isStunned == true)
  {
    float step;
    float offset;
    if (isFacingRight)
    {
       step = -.5f;
       offset = (x+utilX)+step;
    }
    else
    {
       step = .5f;
       offset = (x+utilX)+step+utilXLen;
    }

    int tilecoordx, tilecoordy, tiletype;
    bool collide = collisionVer(offset, y, tilecoordx, tilecoordy, tiletype);

    if (collide == false)
    {
      this->x += step;
    }
    this->y += rand() % 2 ? 0 : 2;

    bool bAnimEnded = false;
    Character::handleAnimation(&bAnimEnded);
    isStunned = !bAnimEnded;

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

  float coloffset = 0.0f;
  if (isFacingRight)
    coloffset = (x+utilX)+velx+utilXLen;
  else
    coloffset = (x+utilX)-velx;

  if(!collisionVer((int)coloffset, y, tilecoordx, tilecoordy, tiletype))
  {
    if (curAnimSeq != Player::FIRINGSTILLHAND)
    {
      if (isFacingRight)
        x += velx;
      else 
        x -= velx;

      velx += accelx;
      if (velx >= mm_player_defs::VELMOVING) velx = mm_player_defs::VELMOVING;
      else if (velx <= 0.0f) velx = 0.0f;
    }
  }
  else
  {
    velx = accelx = 0.0f;
  }

  if (cur_stage->horz_scroll == false)
  {
    if (key[KEY_RIGHT])
    {
      if (isFacingRight == false) velx = accelx = 0.0f;

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

        if (accelx < 1.0f) accelx += onIce ? 0.005 : 0.1f;
        else accelx = 1.0f;
      }
    }
    else if (key[KEY_LEFT] )
    {
      if (isFacingRight == true) velx = accelx = 0.0f;

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

        if (accelx < 1.0f) accelx += onIce ? 0.005 : 0.1f;
        else accelx = 1.0f;
      }
    }
    else
    {
      if (velx > 0.0f) accelx = onIce ? -.05f : -.35f;

      //if (lockjump == false && grabstair == false)
      if (onground == true && grabstair == false)
      {
        //TODO: only fire again after already still, so you cant stop and continue firing without stop.
        //This makes megaman kind of pose like it's starting to run after hit the ground,
        //the original game has this, but it's not almost not noticiable and i think 
        //it is a side effect of the original game control logic. keep turned off.
        // curAnimSeq == Player::HITGROUND) //true) //fallRuning == 5)
        static int fallRuning = 0;
        if (curAnimSeq != Player::HITGROUND || fallRuning == 6)
        {
          fallRuning = 0;
          if (firing == false)
          {
            setAnimSeq(Player::STANDSTILL);
            if (justLeaveStair) { animationFirstPass = false; justLeaveStair = false; }
          }
          else
          {
            if (handType())
              setAnimSeq(Player::FIRINGSTILL);
            else
              setAnimSeq(Player::FIRINGSTILLHAND);
          }
        }
        else
        {
          fallRuning++;
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
    if (firing==false && holdingGutsmanRock==false)
    {
      if (key[KEY_UP])
      {
        if(collisionStair(x+utilX, y, tilecoordx, tilecoordy, tiletype, true) == true)
        {
          vely = 0.0f;

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
                y-= 6.0f;
                y-= ((int)y)%mm_graphs_defs::TILE_SIZE;
                y = (y / mm_graphs_defs::TILE_SIZE) * mm_graphs_defs::TILE_SIZE;
                
                isGettingOut = false;
                onground  = true;
                grabstair = false;
                justLeaveStair = true;
                
                vely = 8.0f;
                setAnimSeq(Player::STANDSTILL);
                animationFirstPass = false;
              }
            }
            else
            {
              isGettingOut = false;
            }
          }

          x    = (tilecoordx * mm_graphs_defs::TILE_SIZE+1) - utilX;
          y   -= 2.0f;
          isFacingDown  = true;
        }
      }
      else if (key[KEY_DOWN])
      {
        // If player is colliding with something and it's a stair tile OR if player
        // is on the last stair step tile(with nothing bellow) then keep holding on it.
        if((collisionHor(x+utilX+(isFacingRight?8:0), y+vely+h, tilecoordx, tilecoordy, true, tiletype) &&
          (tiletype == mm_tile_actions::TILE_STAIR || tiletype == mm_tile_actions::TILE_STAIR_BEGIN) &&
          (( (int)(x+utilX+(isFacingRight?8:0)) % mm_graphs_defs::TILE_SIZE) < (mm_graphs_defs::TILE_SIZE/2))) ||
          (overstair == true && tiletype == 0))
        {
          vely = 0.0f;

          Character::handleAnimation(); // While on stairs auto animation is off
          grabstair    = true;
          isGettingOut = false;

          if (tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
          {
            y+=16.0f;
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
          y += 2.0f;
        }
        else if (grabstair == true)
        {
          justLeaveStair = true;
          grabstair = false;
        }
      }
    }

    static bool spacePressed = false;
    if (key[KEY_SPACE])
    {
      if (grabstair == true)
      {
        if (!key[KEY_UP] && !key[KEY_DOWN])
        {
          grabstair = false;
          lockjump = true;
          spacePressed = true;
          lockJumpAccel = true;
          if (onground == true) spacePressed = false;

          goto letgo;
        }
      }

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

        vely = -8.0f;
      }
      else
      {
        if (lockjump == true) {lockJumpAccel = true;}
      }

      if (!grabstair && lockjump == false)
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

        vely     = -8.0f;
        onIce    = false;
        onground = false;
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
  letgo:

    if (key[KEY_A] &&
       (fireKeyPressed == false || (curWeapon == mm_weapons::W_PLATFORM_CREATOR && MagneticBeamHandler::instance()->canCreateAgain())))
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
      MagneticBeamHandler::instance()->newBeams();
      fireKeyPressed = false;
    }
  }
}

void Player::die()
{
  if (Clock::clockTicks-7 > dyingTimer)
  {
    MM_Math::pt explosionPts[] = {{mm_player_defs::PLAYERWIDTH>>1,mm_player_defs::PLAYERHEIGHT>>1},
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

      Character * pExplosion05 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos, -19, 0);
      Character * pExplosion06 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos,  19, 0);
      Character * pExplosion07 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos-iSize,  0, -19);
      Character * pExplosion08 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos, ypos+iSize,  0, 19);

      iSize-=6;
      
      Character * pExplosion09 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos-iSize, -14.5, -14.5);
      Character * pExplosion10 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos-iSize, ypos+iSize, -14.5, 14.5);
      Character * pExplosion11 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos-iSize, 14.5, -14.5);
      Character * pExplosion12 = cur_stage->createCharacter(mm_tile_actions::MEGAMAN_EXPLOSION_CHAR, xpos+iSize, ypos+iSize, 14.5, 14.5);

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

bool Player::collisionVer(int x, int yp, int &tilecoordx, int &tilecoordy, int &tiletype)
{
  int realFrameHeight = getFrameH();

  if (onground == true)
    yp = (yp + (h-realFrameHeight));

  int tileypixels = yp - (yp%mm_graphs_defs::TILE_SIZE);
  int testend = yp + realFrameHeight;

  tilecoordx = x / mm_graphs_defs::TILE_SIZE;

  tilecoordy = tileypixels / mm_graphs_defs::TILE_SIZE;
  while (tileypixels <= testend)
  {
    tiletype = cur_stage->tileAction(tilecoordx, tilecoordy);

    if (tiletype == mm_tile_actions::TILE_SOLID)
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

bool Player::collisionStair(int px, int py, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing)
{
  int realFrameHeight = getFrameH();
  int highDiff = (h-realFrameHeight);
  py += highDiff;

  int tileypixels = py-(py%mm_graphs_defs::TILE_SIZE);
  int testend     = py + (realFrameHeight-highDiff);

  tilecoordx = px + (utilXLen / 2.0f);
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
      if ((GlobalGameState::playerShots.size() == 0) && (weapons[mm_weapons::W_GUTSMAN_GUN] > 0))
      {
        if ((conPlayer != NULL) && (conPlayer->type == mm_spritefiles::GUTSMANROCK_SPRITES))
        {
          conPlayer->life = 0;
          this->holdingGutsmanRock = true;
          mm_weapons::createGutsmanRock(this, conPlayer->x, conPlayer->y, 0, 0, 0);
        }
        else
        {
          if (GutsmanGunManager::instance()->launchRock())
          {
            this->holdingGutsmanRock = false;
            firing = true;
            lastShot = Clock::clockTicks;
          }
        }
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
      if (weapons[mm_weapons::W_PLATFORM_CREATOR] > 0)
      {
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
}

void Player::drawCharacter(BITMAP * bmp)
{
  if (bDraw == false)
  {
    return;
  }

  int tempCurAnimSeq = curAnimSeq;

  if (this->holdingGutsmanRock)
  {
    if (tempCurAnimSeq == RUNNING) tempCurAnimSeq = RUNNINGROCK;
    else if (tempCurAnimSeq == STARTINGRUN) tempCurAnimSeq = STARTINGRUNROCK;
    else if (tempCurAnimSeq == STANDSTILL) tempCurAnimSeq = STANDSTILLROCK;
    else if (tempCurAnimSeq == JUMPING) tempCurAnimSeq = JUMPINGROCK;
  }

  int curSpriteFrame = anim_seqs[tempCurAnimSeq][curAnimFrame].frameNumber;

  int xpos = this->sx;

/*
  // gato para alinhar sprite quando correndo, nao precisa mais.
  switch(tempCurAnimSeq)
  {
    case Player::FIRINGRUNNING:
    case Player::FIRINGSTILL:
    case Player::FIRINGSTILLHAND:
    {
      xpos += isFacingRight ? 9 : -9;
    }
    break;
  }
*/

  if (isFacingRight == true)
  {
    draw_sprite(bmp, this->spriteSheet->getFrame(curSpriteFrame), xpos, this->sy);
  }
  else
  {
    draw_sprite_h_flip(bmp, this->spriteSheet->getFrame(curSpriteFrame), xpos, this->sy);
  }

#ifdef DEBUG
  //int xrect = (xpos + this->utilX);
  rect(bmp, this->sx, this->sy, this->sx + getFrameW(), this->sy + mm_player_defs::PLAYERHEIGHT, makecol(255,0,0));
  //rectfill(bmp, xrect, this->sy, xrect + this->utilXLen, this->sy + mm_player_defs::PLAYERHEIGHT, makecol(0,255,0));
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
  if (lockmoves == false)
    setAnimSeq(Player::RUNNING);
  else
  {
    setAnimSeq(Player::HITGROUND);
  }

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
