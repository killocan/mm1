/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//DisappearingBlock
class DisappearingBlock : public Character
{
public:
  class DisappearingBlockFragment : public Character
  {
  public:
    DisappearingBlockFragment(const Stage & stage, int x, int y, void * param);

    virtual void doGravitation();
    virtual void doLogic();
    virtual void checkOnCamera();
    virtual void respawn();

    void resetState();

    enum {WAITING1, SHOWING, ALIVE, WAITING2, DEAD};

  private:
    unsigned long delay1;
    unsigned long delay2;
    unsigned long delay;
    unsigned long ticks;
  };

  DisappearingBlock(const Stage & stage, int x, int y, void * param);
  virtual ~DisappearingBlock();

  virtual void doLogic();
  virtual void drawCharacter(BITMAP * bmp);
  virtual void doGravitation();
  virtual BITMAP * getFrame();
  virtual void respawn();

  enum {ACTIVE};
private:
  BITMAP * internalSprite;
  std::vector<Character *> fragments;
};
