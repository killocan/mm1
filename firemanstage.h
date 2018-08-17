/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class FireManStage : public StageManager
{
  public:
    FireManStage();

    virtual void setupStage();
    virtual void doStageSpecifics();

  protected:
    SceneSoundManager * CreateSoundManager();
  private:
    unsigned long lastCycleTime;
    struct Point2D
    {
      int x, y;
    };
};
