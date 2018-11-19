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
    struct DisappearingBlockSetupParam
    {
      int x,y;
      unsigned long delayFirstShow;
      unsigned long delayOnScreen;
      unsigned long delayNextShow;
    };

    DisappearingBlockFragment(const Stage & stage, int x, int y, void * param);

    virtual void drawCharacter(BITMAP * bmp);
    virtual void doGravitation();
    virtual void doLogic();
    virtual void checkOnCamera();
    virtual void respawn();

    void resetState();

    enum {WAITING1, SHOWING, ALIVE, WAITING2, DEAD};

  private:
    unsigned long delayFirstShow;
    unsigned long delayOnScreen;
    unsigned long delayNextShow;
    unsigned long ticks;
    bool firstShow;
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
