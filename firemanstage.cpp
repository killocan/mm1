/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "firemanstage.h"
#include "stage.h"
#include "defines.h"
#include "globals.h"
#include "soundpack.h"
#include "tileactions.h"
#include "spritefiles.h"

FireManStage::FireManStage() : lastCycleTime(0)
{
  stage_number = 4;
  STAGE_PATH = "./fireman_stage";
}

void FireManStage::setupStage(bool hardStart)
{
  if (hardStart)
  {
    stage->runtimeLoadCharacter(mm_spritefiles::KILLERBULLET_SPRITES);
    stage->runtimeLoadCharacter(mm_spritefiles::MOVING_FLAME_SPRITES);

    stage->setOffset(mm_spritefiles::SCREWBOMBER_SPRITES, 8); // set which sprite color to use.

    bossWarningTiles.push_back(8);
    bossWarningTiles.push_back(18);

    bossWarningColors[0x006000] = 0xbababa;
    bossWarningColors[0x00a040] = 0xefefef;
    bossWarningColors[0xbababa] = 0x006000;
    bossWarningColors[0xefefef] = 0x00a040;
  }

  BossDoor::BossDoorSetupParam door1;
  door1.size = 2;
  door1.totalSize = 4;
  door1.orientation = 0;
  door1.defaultOpen = false;

  BossDoor::BossDoorSetupParam door2;
  door2.size = 1;
  door2.totalSize = 4;
  door2.orientation = 0;
  door2.defaultOpen = false;

  BossDoor::BossDoorSetupParam door3;
  door3.size = 1;
  door3.totalSize = 4;
  door3.orientation = 0;
  door3.defaultOpen = true;

  Character * pDoor1C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 158*32, 6*32, 0,0, (void*)&door1);
  Character * pDoor2C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 207*32, 6*32, 0,0, (void*)&door2);
  Character * pDoor3C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 208*32, 6*32, 0,0, (void*)&door3);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor1C);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor2C);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor3C);
  special_chars_vec.push_back(pDoor1C);
  special_chars_vec.push_back(pDoor2C);
  special_chars_vec.push_back(pDoor3C);

  int * sectors = new int[4];
  sectors[0] = 6;
  sectors[1] = 7;
  sectors[2] = 8;
  sectors[3] = -1;
  Character * pKillerBullet = stage->createCharacter(mm_tile_actions::TILE_ENEMY_KILLERBULLET,
                                                     110*32, 9*32, 0,0, (void*)sectors);
  characters_vec.push_back(pKillerBullet);

  Character * pMovingFlame0 = stage->createCharacter(mm_tile_actions::MOVING_FLAME,
                                                    110*32, 31*32, 0,0, (void*)NULL);
  Character * pMovingFlame1 = stage->createCharacter(mm_tile_actions::MOVING_FLAME,
                                                    110*32, 16*32, 0,0, (void*)1);
  characters_vec.push_back(pMovingFlame0);
  characters_vec.push_back(pMovingFlame1);
}

void FireManStage::doStageSpecifics()
{
  static Point2D pts[] = { { 2, 2 }, { 2, 36 }, { 138, 2 }, { 104, 36 }, { 138, 36 } };
 
  StageManager::doStageSpecifics();

  if ((Clock::clockTicks > 10) && (Clock::clockTicks - 10 > lastCycleTime))
  {
    lastCycleTime = Clock::clockTicks;

    BITMAP * tileset = stage->getTileSet();
    int color;
    for (int i = 0; i < 5; ++i)
    {
      Point2D * pt = &pts[i];
      for (int x = pt->x; x < pt->x + mm_graphs_defs::TILE_SIZE; ++x)
      {
        for (int y = pt->y; y < pt->y + mm_graphs_defs::TILE_SIZE; ++y)
        {
          color = _getpixel32(tileset, x, y);
          switch (color)
          {
          case 0xA02000:
            color = 0xE08040;
            break;
          case 0xE04000:
            color = 0xA02000;
            break;
          case 0xE08040:
            color = 0xE04000;
            break;
          }
          _putpixel32(tileset, x, y, color);
        }
      }
    }
  }
}

SceneSoundManager * FireManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::FIREMAN_BG_MUSIC], 5107*44, 30737*44);
}
