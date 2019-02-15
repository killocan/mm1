/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class ElecManStage : public StageManager
{
  public:
    ElecManStage();

    virtual void setupStage(bool hardStart);
    virtual void createBoss();
    virtual void doStageSpecifics();

  protected:
    SceneSoundManager * CreateSoundManager();
};
