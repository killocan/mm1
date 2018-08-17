/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class GutsManStage : public StageManager
{
  public:
    GutsManStage();
    ~GutsManStage();

    virtual void setupStage();
    virtual void doStageSpecifics();
    virtual void createBoss();

  protected:
    SceneSoundManager * CreateSoundManager();
    bool bPlayingPlatformSound;
};
