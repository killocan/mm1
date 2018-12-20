/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

class MagneticBeam : public Character
{
public:
  MagneticBeam(const Stage & stage, int x, int y);

  virtual void doLogic();
  virtual void drawCharacter(BITMAP * bmp);
  virtual BITMAP * getFrame();
  virtual void checkOnCamera();
  virtual void doGravitation();

  void commit();

  enum {STATIC, BLINKING};
private:
  void calculateXY();

  unsigned long lifetime;
  bool commited;
  int pos;
};
