/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "cutmanstage.h"
#include "stage.h"
#include "soundpack.h"
#include "spritefiles.h"

CutManStage::CutManStage()
{
  stage_number = 0;
  STAGE_PATH = "./cutman_stage";
}

void CutManStage::setupStage(bool hardStart)
{
  if (hardStart)
  {
    stage->setOffset(mm_spritefiles::BLADER_SPRITES, 1); // set which sprite color to use.
    stage->setOffset(mm_spritefiles::BEAK_SPRITES, 4); // set which sprite color to use.
    stage->setOffset(mm_spritefiles::SPINE_SPRITES, 0); // set which sprite color to use.
    stage->setOffset(mm_spritefiles::OCTOPUSBATTERY_SPRITES, 0); // set which sprite color to use. NOTE: default
  }
}

SceneSoundManager * CutManStage::CreateSoundManager()
{
  return (new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::CUTMAN_BG_MUSIC], 150612, 1840124));
}
