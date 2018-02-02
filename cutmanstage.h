/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class CutManStage : public StageManager
{
  public:
    CutManStage();

    virtual void setupStage();

  protected:
    SceneSoundManager * CreateSoundManager();
};
