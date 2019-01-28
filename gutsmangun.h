/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Gutsman Rock Gun
class GutsmanGun : public Character
{
public:
  GutsmanGun(const Stage & stage, int x, int y, void * param);

  void launch();

  virtual void doLogic();
  virtual void die();
  virtual void doGravitation();
  virtual void checkOnCamera();

  void moveToMegaman();
  void moveToThrower();
  void calcAcceleration();

  Character * thrower;

  static bool firstOne;
  static float firstX;

  enum { MOVING, ATTACHED_TO, LAUNCH, FRAGMENT, DEAD };
private:
  float getXDest();
  float getYDest();
  bool checkCollision();

  unsigned long ticks;
};
