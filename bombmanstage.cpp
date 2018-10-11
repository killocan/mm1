/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "bombmanstage.h"
#include "stage.h"
#include "soundpack.h"
#include "spritefiles.h"
#include "tileactions.h"
#include "globals.h"

BombManStage::BombManStage()
{
  stage_number = 3;
  STAGE_PATH = "./bombman_stage";
}

void BombManStage::setupStage()
{
  stage->setOffset(mm_spritefiles::BOMBOMB_SPRITES, 0); // set which sprite color to use.

  BossDoor::BossDoorSetupParam door1;
  door1.size = 2;
  door1.totalSize = 4;
  door1.orientation = 0;
  door1.defaultOpen = false;

  Character * pDoor1C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 14*32, 10*32, 0,0, (void*)&door1);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor1C);
  special_chars_vec.push_back(pDoor1C);
}

SceneSoundManager * BombManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::BOMBMAN_BG_MUSIC], 341100, 1657336);
}
