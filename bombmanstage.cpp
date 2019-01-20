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

  bossWarningTiles.push_back(16);
  bossWarningTiles.push_back(17);
  bossWarningColors[0x00bd00] = 0xffffff;
  bossWarningColors[0xffffff] = 0x00bd00;

  BossDoor::BossDoorSetupParam door1;
  door1.size = 2;
  door1.totalSize = 4;
  door1.orientation = 1;
  door1.blockedSide = 1;
  door1.defaultOpen = true;

  Character * pDoor1C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 1*32, 15*32, 0,0, (void*)&door1);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor1C);
  special_chars_vec.push_back(pDoor1C);
}

SceneSoundManager * BombManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::BOMBMAN_BG_MUSIC], 341100, 1657336);
}

void BombManStage::doStageSpecifics()
{
  StageManager::doStageSpecifics();

  Player * player = stage->m_player;
  if (cur_stage_state == BOSS_WARNING)
  {
    player->doGravitation();
    if (player->onground)
    {
      player->setAnimSeq(Player::STANDSTILL);
    }
  }
}
