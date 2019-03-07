/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Bombman
class Bombman : public Character
{
public:
  Bombman(const Stage & stage, int x, int y, void * param);

  virtual void doLogic();
  virtual void touchGround();
  virtual void die();
  virtual void fire();
  virtual void drawCharacter(BITMAP * bmp);
  virtual bool handleAnimation(bool * bAnimationEnded = NULL);

  virtual void hit(mm_weapons::weapon_st * pWeapon);

  enum { SHOW_OFF, DECIDING, JUMP, JUMP_ATTACK, ATTACK, };
  int cycleCount;

private:
  bool jumping, jumping_right;

  bool bDying;
  unsigned long dyingTimer;
  int dieStep;

  bool isHitAnimOn;
  bool bDraw;
  unsigned long blinkBegin;
  int blinkCount;
};
