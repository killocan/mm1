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
      bool defaultOpen;
      //bool cameraHack;
    };

    BossDoor(const Stage & stage, int x, int y, void * param);
    ~BossDoor();

    virtual void doLogic();

    virtual BITMAP * getFrame();
    virtual void drawCharacter(BITMAP * bmp);

    // Do nothing
    virtual void doGravitation();

    virtual void respawn();

    void openDoor();
    void closeDoor();
    // only for vertical orientation.
    void blockPassage();

    bool hasBeenUsed;

    enum {CLOSED, OPENED, OPENING, CLOSING, WAITING};
  private:

    BossDoorSetupParam config;
    //Stage * cur_stage; // current stage.
    BITMAP * sprite;
    int curDoorLevel;
    int xcamblock, ycamblock;
    unsigned long doorOpenDelay;
};
