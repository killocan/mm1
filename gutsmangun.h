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
  class GutsmanGunFragment : public Character
  {
  public:
    GutsmanGunFragment(const Stage & stage, int x, int y);
  };

  GutsmanGun(const Stage & stage, int x, int y);

  void launch();

  virtual void doLogic();
  virtual void die();
  virtual void doGravitation();
  virtual void checkOnCamera();

  enum { MOVING, ATTACHED_TO_MEGAMAN, LAUNCH };
private:
  unsigned long ticks;
};
