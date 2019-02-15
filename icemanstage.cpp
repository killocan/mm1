/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <vector>

#include "icemanstage.h"
#include "stage.h"
#include "defines.h"
#include "globals.h"
#include "soundpack.h"
#include "tileactions.h"
#include "spritefiles.h"

#include "disappearingblock.h"

IceManStage::IceManStage()
{
  stage_number = 2;
  STAGE_PATH = "./iceman_stage";
}

void IceManStage::setupStage(bool hardStart)
{
  if (hardStart)
  {
    stage->runtimeLoadCharacter(mm_spritefiles::TIMER_PLATFORM_SPRITES);
    stage->setOffset(mm_spritefiles::TIMER_PLATFORM_SPRITES, 0);

    stage->setOffset(mm_spritefiles::GUTSMANROCK_SPRITES, 6);
  }

  DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam block1;
  block1.x = 24<<5;
  block1.y = 10<<5;
  block1.delayFirstShow = 10;
  block1.delayOnScreen = 120;
  block1.delayNextShow = 120;

  DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam block2;
  block2.x = 22<<5;
  block2.y = 7<<5;
  block2.delayFirstShow = 100;
  block2.delayOnScreen = 120;
  block2.delayNextShow = 120;

  std::vector<DisappearingBlock::DisappearingBlockFragment::DisappearingBlockSetupParam> blocks;
  blocks.push_back(block1);
  blocks.push_back(block2);

  Character * pDisappearingBlockSet = stage->createCharacter(mm_tile_actions::TILE_TIMER_PLATFORM,
                                                             24*32, 13*32, 0,0, (void*)&blocks);
  characters_vec.push_back(pDisappearingBlockSet);
}

SceneSoundManager * IceManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::ICEMAN_BG_MUSIC], 18500*44, 44124*44);
}
