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
  class MovingFlameFragment : public Character
  {
  public:
    MovingFlameFragment(const Stage & stage, int x, int y, void * param);

    virtual void doGravitation();
    virtual void doLogic();
    virtual void checkOnCamera();
    virtual void respawn();
    virtual void drawCharacter(BITMAP * bmp);

    void resetState();
    void logicType0();
    void logicType1();

    enum {ACTIVE, DEAD, DELAY};

    const int target_x[4] = {106, 100, 100, 109};
    const int target_y[4] = {21,  20,  25,  24};
    const int new_x[4] = {107*32-15, 100*32, 100*32, 108*32+2};
    const int new_y[4] = {20*32+16,  20*32+16,  24*32+16,  24*32+16};
    const int target_rotation[4] = {64, 0, 192, 0};
  private:
    int rotation;
    unsigned long delay;
    unsigned long old_delay;
    unsigned long ticks;

    int current_target;
  };

  MovingFlame(const Stage & stage, int x, int y, void * param);
  virtual ~MovingFlame();

  virtual void doLogic();
  virtual void drawCharacter(BITMAP * bmp);
  virtual void doGravitation();
  virtual BITMAP * getFrame();
  virtual void respawn();

  enum {ACTIVE};
private:
  int type;
  BITMAP * internalSprite;
  Character * fragments[4];
};
