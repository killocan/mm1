/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

class FootHolder : public Character
{
public:
  FootHolder(const Stage & stage, int x, int y);

  virtual void fire();
  virtual void respawn();
  virtual void doLogic();

#ifdef DEBUG
  virtual void drawCharacter(BITMAP * bmp);
#endif

  virtual void doGravitation();

  enum {MOVING};
  enum direction {RIGHT, LEFT};

private:
  unsigned long fire_pause;
  unsigned long logic_timer;

  FootHolder::direction current_direction;
  int displacement_x;
  int x_quadrant;
  int y_line;
  int current_decision;
  int xstep;
  static int decisions[6][3];
};
