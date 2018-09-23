/*
 * Killocan 2018 oO started in 2013... jesus... :(
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//FootHolder
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

  enum {MOVING, FIRING};
private:
  unsigned long fire_pause;
  int xmin;
  float xmax;
  float ymin;
  float ymax;
  float midx;
  bool crossed_min,crossed_max;
};
