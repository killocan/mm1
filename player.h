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

    virtual bool collisionVer(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype);
    virtual bool collisionStair(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing = false);

    virtual bool checkStair();
    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    virtual void checkOnCamera(); 
    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void hit(Character * pCharacter);
    virtual void drawCharacter(BITMAP * bmp);
    virtual void loadWeapon();
    virtual void fire();
    virtual void touchGround();
    virtual void touchCelling();
    virtual void firingOnJump();
    virtual void changeWeapon();
    virtual void setAnimSeq(int newAnimSeq, bool reset = true, bool jumpnloopframes=false);
    void getStunned();
    
    void reset();
    void forceAnimation();

    bool lockJumpAccel;
    bool fireKeyPressed;
    float accelx;

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
          FIRINGJUMPHAND,
          STARTINGRUNROCK,
          RUNNINGROCK,
          STANDSTILLROCK,
          JUMPINGROCK,
          STUNNED,
          HITGROUND,
          HITGROUNDFIRING,
         };

    std::map<mm_weapons::WEAPONS, int> weapons; // WEAPON, ENERGY LEVEL
    mm_weapons::WEAPONS curWeapon;
    bool holdingGutsmanRock;
    bool isStunned;
    bool isGettingOut;
    int handType();
    virtual bool canJumpAgain();

    // Stair related stuff.
    bool isGettingIn;
    // Hit animation
    bool isHitAnimOn;
    bool bHide;
    bool bDraw;
    unsigned long blinkBegin;
    int blinkCount;
    int hitDir;
    // Dying animation
    bool bDying;
    unsigned long dyingTimer;
    int dieStep;
    //boss stuff
    bool lockmoves;
    bool justLeaveStair;
    //control animation after warp animation
    bool justWarped;
};
