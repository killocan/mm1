/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "icemanstage.h"
#include "soundpack.h"

IceManStage::IceManStage()
{
  stage_number = 2;
  STAGE_PATH = "./iceman_stage";
}

SceneSoundManager * IceManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::ICEMAN_BG_MUSIC], 18500*44, 44124*44);
}
