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

  virtual void doLogic();

  // Do nothing
  virtual void doGravitation();

  enum {MOVING, FIRING};
private:
  unsigned long fire_pause;
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  float midx, midy;
  bool crossed_min,crossed_max;
};
