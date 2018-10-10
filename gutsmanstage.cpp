/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "gutsmanstage.h"
#include "globals.h"
#include "stage.h"
#include "soundpack.h"
#include "spritefiles.h"
#include "sfx.h"
#include "tileactions.h"
#include "defines.h"

GutsManStage::GutsManStage() : bPlayingPlatformSound(false)
{
  STAGE_PATH = "./gutsman_stage";
}

GutsManStage::~GutsManStage()
{
  Sounds::mm_sounds->stopSample(PLATFORM_MOVE);
}

void GutsManStage::setupStage()
{
  stage->runtimeLoadCharacter(mm_spritefiles::GUTSMAN_SPRITES);

  stage->setOffset(mm_spritefiles::BIGEYE_SPRITES, 0); // set which sprite color to use.
  stage->setOffset(mm_spritefiles::FLEA_SPRITES,   0); // set which sprite color to use.
  stage->setOffset(mm_spritefiles::FOOTHOLDER_SPRITES, 0);

  bossWarningTiles.push_back(8);

  bossWarningColors[0x8c1800] = 0xffffff;
  bossWarningColors[0xffffff] = 0x8c1800;

  stage->hasMovingPlatform = true;
  stage->platformTiles.push_back(21); // normal moviment
  stage->platformTiles.push_back(20); // bounce back
  stage->platformTiles.push_back(22); // hole

  BossDoor::BossDoorSetupParam door1;
  door1.size = 2;
  door1.totalSize = 4;
  door1.orientation = 0;
  door1.defaultOpen = false;
  //door1.cameraHack = true;

  BossDoor::BossDoorSetupParam door2;
  door2.size = 1;
  door2.totalSize = 4;
  door2.orientation = 0;
  door2.defaultOpen = false;
  //door2.cameraHack = false;

  BossDoor::BossDoorSetupParam door3;
  door3.size = 1;
  door3.totalSize = 4;
  door3.orientation = 0;
  door3.defaultOpen = true;
  //door3.cameraHack = false;

  Character * pDoor1C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 174*32, 66*32, 0,0, (void*)&door1);
  Character * pDoor2C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 223*32, 66*32, 0,0, (void*)&door2);
  Character * pDoor3C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 224*32, 66*32, 0,0, (void*)&door3);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor1C);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor2C);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor3C);
  special_chars_vec.push_back(pDoor1C);
  special_chars_vec.push_back(pDoor2C);
  special_chars_vec.push_back(pDoor3C);

  bool bSpecialBlader = true;
  Character * pBlader1 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 126*32, 18*32, 0,0, (void*)&bSpecialBlader);
  Character * pBlader2 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 134*32, 18*32, 0,0, (void*)&bSpecialBlader);
  Character * pBlader3 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 130*32, 20*32, 0,0, (void*)&bSpecialBlader);
  Character * pBlader4 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 126*32, 34*32, 0,0, (void*)&bSpecialBlader);
  Character * pBlader5 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 134*32, 34*32, 0,0, (void*)&bSpecialBlader);
  Character * pBlader6 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_BLADER, 130*32, 36*32, 0,0, (void*)&bSpecialBlader);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader1);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader2);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader3);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader4);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader5);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pBlader6);
}

void GutsManStage::doStageSpecifics()
{
  StageManager::doStageSpecifics();
  
  if (stage->m_player->x > 840)
  {
    if (bPlayingPlatformSound == false && stage->m_player->x < 2350)
    {
      Sounds::mm_sounds->play(PLATFORM_MOVE, true);
      bPlayingPlatformSound = true;
    }
    else
    {
      if (stage->m_player->x > 2350)
      {
        Sounds::mm_sounds->stopSample(PLATFORM_MOVE);
        bPlayingPlatformSound = false;
      }
    }
  }
  else
  {
    if (bPlayingPlatformSound == true)
    {
      Sounds::mm_sounds->stopSample(PLATFORM_MOVE);
      bPlayingPlatformSound = false;
    }
  }
}

SceneSoundManager * GutsManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::GUTSMAN_BG_MUSIC], 3147*44, 28754*44);
}

void GutsManStage::createBoss()
{
  Character * pGutsman = stage->createCharacter(mm_tile_actions::GUTSMAN, 235 * 32, 70 * 32, 0, 0, (void*)NULL);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pGutsman);
}
