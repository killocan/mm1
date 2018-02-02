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
  protected:
    SceneSoundManager * CreateSoundManager();
};
