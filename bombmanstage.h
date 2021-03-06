/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class BombManStage : public StageManager
{
  public:
    BombManStage();

    virtual void setupStage(bool hardStart);
    virtual void doStageSpecifics();
    virtual void createBoss();

  protected:
    SceneSoundManager * CreateSoundManager();
};
