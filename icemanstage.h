/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include "stagemanager.h"

class IceManStage : public StageManager
{
  public:
    IceManStage();
  protected:
    SceneSoundManager * CreateSoundManager();
};
