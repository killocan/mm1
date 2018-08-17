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
}

void FireManStage::doStageSpecifics()
{
  static Point2D pts[] = { { 2, 2 }, { 2, 36 }, { 138, 2 }, { 104, 36 }, { 138, 36 } };
 
  // Let dad take care of doors and stuff.
  StageManager::doStageSpecifics();

  if ((Clock::clockTicks > 10) && (Clock::clockTicks - 10 > lastCycleTime))
  {
    lastCycleTime = Clock::clockTicks;

    BITMAP * tileset = stage->getTileSet();
    unsigned int color;
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
{//30766
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::FIREMAN_BG_MUSIC], 5107*44, 30737*44);
}
