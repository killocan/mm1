/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//BossDoor
class BossDoor : public Character
{
  public:
    struct BossDoorSetupParam
    {
      int size;
      int totalSize;
      int orientation;
      int blockedSide; // 0 - UP, 1 - DOWN, 2 - LEFT, 3 - RIGHT
      // On the first 6 levels: 0 - free megaman moviment after block passage, 2 - wait until boss show to end
      // Used to calculate how far to move megaman.
      int type;
      bool defaultOpen;
    };

    BossDoor(const Stage & stage, int x, int y, void * param);
    ~BossDoor();

    virtual void doLogic();

    virtual BITMAP * getFrame();
    virtual void drawCharacter(BITMAP * bmp);

    // Do nothing
    virtual void doGravitation();
    virtual void checkOnCamera();
    virtual void respawn();

    void openDoor();
    void closeDoor();
    // only for vertical orientation.
    void blockPassage();
    void block();

    bool hasBeenUsed;
    BossDoorSetupParam config;

    enum {CLOSED, OPENED, OPENING, CLOSING, WAITING};
  private:
    BITMAP * sprite;
    int curDoorLevel;
    int xcamblock, ycamblock;
    unsigned long doorOpenDelay;
};
