/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "firemanstage.h"
#include "stage.h"
#include "defines.h"
#include "globals.h"
#include "soundpack.h"
#include "spritefiles.h"

FireManStage::FireManStage() : lastCycleTime(0)
{
  STAGE_PATH = "./fireman_stage";
}

void FireManStage::setupStage()
{
  stage->setOffset(mm_spritefiles::SCREWBOMBER_SPRITES, 8); // set which sprite color to use.

  bgTileWarning0 = 8;
  bgTileWarning1 = 35;

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
