/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Megaman
class Player : public Character
{
  public:
    Player(const Stage & stage);

    virtual void die();
    void normalLogic();
    void doGravitation();
    virtual void doLogic();
    virtual bool collisionStair(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing = false);
    virtual bool checkStair();
    //virtual bool handleAnimation(); // Avoid animation when over stair.
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void checkOnCamera(); // megaman never leaves the camera.
    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void hit(Character * pCharacter);
    virtual void drawCharacter(BITMAP * bmp);
    virtual void loadWeapon();
    virtual void fire();
    virtual void touchGround();
    virtual void touchCelling();
    virtual void firingOnJump();
    virtual void changeWeapon();
    virtual void setAnimSeq(int newAnimSeq, bool reset = true);
    
    void reset();
    void forceAnimation();

    int lives;

    bool lockJumpAccel;

    enum {
          SPAWNING, 
          STANDSTILL, 
          RUNNING, 
          JUMPING, 
          ONSTAIR, 
          UPDOWNSTAIR,
          FIRINGSTILL, 
          FIRINGJUMP, 
          FIRINGRUNNING, 
          FIRINGSTAIR, 
          ENTERINGSTAIR,
          GETHIT,
          STARTINGRUN,
          FIRINGSTAIRHAND,
          FIRINGSTILLHAND,
          FIRINGJUMPHAND
         };

    std::map<mm_weapons::WEAPONS, int> weapons; // WEAPON, ENERGY LEVEL
    mm_weapons::WEAPONS curWeapon;

  private:
    int handType();
    virtual bool canJumpAgain();

    // Stair related stuff.
    bool isGettingOut;
    bool isGettingIn;
    bool isHitAnimOn;

    // Hit animation
    bool bHide;
    bool bDraw;
    unsigned long blinkBegin;
    int blinkCount;
    int hitDir;

    // Dying animation
    bool bDying;
    unsigned long dyingTimer;
    int dieStep;
};
