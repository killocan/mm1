/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//MovingFlame
class MovingFlame : public Character
{
public:
  MovingFlame(const Stage & stage, int x, int y, void * param);

  virtual void doLogic();

  virtual void doGravitation();

  enum {ACTIVE};
};
