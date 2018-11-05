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

    enum {ACTIVE, DEAD};

  private:
    int rotation;
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
