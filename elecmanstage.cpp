/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "elecmanstage.h"
#include "soundpack.h"

ElecManStage::ElecManStage()
{
  STAGE_PATH = "./elecman_stage";
}

SceneSoundManager * ElecManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::ELECMAN_BG_MUSIC], 151668, 1844128);
}
