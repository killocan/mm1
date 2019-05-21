/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

#include <list>
#include <map>
#include <string>

#include "weapons.h"
#include "animsequence.h"

class Stage;
class Camera;

class Character
{
  public:
    const static int FREEZE = -1;

    Character(const Stage & stage, int TYPE);
    Character();
    virtual ~Character();

    // Process AI, internal state changes and stuff.
    virtual void doLogic();

    virtual bool collisionVer(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype);
    virtual bool collisionHor(int x, int y, int &tilecoordx, int &tilecoordy, bool going_down, int &tiletype);
    virtual bool collisionStair(int x, int y, int &tilecoordx, int &tilecoordy, int &tiletype, bool grabing = false);
    void calcScreenCoords();

    virtual bool checkStair();

    void goRight();
    void goLeft();
    virtual void die();

    virtual void doGravitation(); // flying, stationary or 'always on the floor' creatures dont care about it :)
    virtual void touchGround(); // What to do when touch ground?
    virtual void touchCelling(); // What to do when touch ground?
    virtual void firingOnJump(); // What to do firing during a jump?

    virtual void drawCharacter(BITMAP * bmp);
    virtual void drawCharacter(BITMAP * bmp, int rotation);

    // Return curr frame, frame_w & frame_h(logic size not the real one)
    virtual BITMAP * getFrame(); // Virtual, so subclasses could manipulate and deliver customized bitmaps for colision check.
    int getFrameW();
    int getFrameH();

    virtual bool handleAnimation(bool * bAnimationEnded = NULL);
    bool nextAnimFrame();
    int getCurrFrameDuration();
    virtual void setAnimSeq(int newAnimSeq, bool reset = true);
    void resetAnimSeq(int animSeq);

    virtual void loadWeapon();
    virtual void fire();
    virtual void changeWeapon();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void hit(Character * pCharacter);
    virtual void respawn();

    void handleFreeze();
    void freeze();
    virtual void defrost(); // Is there something to do the moment it leaves the freeze state?

    virtual void checkOnCamera();
    virtual bool canJumpAgain();

    int getCurAnimSeqState();

    float x, y;
    float old_x, old_y;
    int sx, sy;
    float h, w;
    float velx, vely;
    float utilX, utilXLen;

    int life;
    int weaponDamage[mm_weapons::WEAPONS_NUMBER]; // Everyone has different damage by different weapons.

    int curState;
    int last_state;

    int colorOffset;

    unsigned long lastShot;

    int curAnimFrame;
    int curAnimFrameDuration;
    int curAnimSeq;

    bool isFacingRight,isFacingDown;
    bool grabstair;
    bool overstair;
    bool lockjump;
    bool onground;
    bool firing;
    bool alive;
    bool onCamera;
    bool isInvincible;
    bool canCollidePlayer; // Only for temporary Characters
    bool canCollideBullet; // Only for temporary Characters
    bool collideWithPlayer;
    bool touchSpike;
    bool onPlatform;
    bool isPlatform;
    bool dropItem;

    Character    * conPlayer; // Player signals that this dude is, somehow, connected to the him.
    Stage        * cur_stage;
    AnimSequence * spriteSheet;
    std::vector<std::vector<AnimSequence::FrameInfoSt> > anim_seqs;
    bool animationFirstPass; // True every first iteration of a animation, false otherwise.


    int type; // So i can guess subclass without RTI.
    // Collision x,y
    int xcol, ycol;

  private:
    void loadAnimSeqs(int TYPE);
};
