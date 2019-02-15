/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "elecmanstage.h"
#include "soundpack.h"
#include "tileactions.h"
#include "globals.h"
#include "stage.h"
#include "defines.h"

ElecManStage::ElecManStage()
{
  stage_number = 5;
  STAGE_PATH = "./elecman_stage";
}

void ElecManStage::setupStage(bool hardStart)
{
  if (hardStart)
  {
    stage->runtimeLoadCharacter(mm_spritefiles::ELECMAN_SPRITES);

    bossWarningTiles.push_back(20);
    bossWarningColors[0x445000] = 0xffffff;
    bossWarningColors[0xffffff] = 0x445000;
  }

  BossDoor::BossDoorSetupParam door;
  door.size = 2;
  door.totalSize = 4;
  door.orientation = 1;
  door.blockedSide = 0;
  door.defaultOpen = true;

  Character * pDoor = stage->createCharacter(mm_tile_actions::TILE_DOOR, 5 * 32, 12 * 32, 0, 0, (void*)&door);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor);
  special_chars_vec.push_back(pDoor);
}

SceneSoundManager * ElecManStage::CreateSoundManager()
{
  return new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::ELECMAN_BG_MUSIC], 151668, 1844128);
}

void ElecManStage::createBoss()
{
  Character * pElecman = stage->createCharacter(mm_tile_actions::ELECMAN, 12 * 32, 10 * 32, 0, 0, (void*)NULL);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pElecman);
}

void ElecManStage::doStageSpecifics()
{
  StageManager::doStageSpecifics();

  Player * player = stage->m_player;
  if (cur_stage_state == BOSS_WARNING && player->grabstair == true)
  {
    int tilecoordx = 0;
    int tilecoordy = 0;
    int tiletype = 0;
    player->forceAnimation();
    if (player->collisionStair(player->x + player->utilX, player->y, tilecoordx, tilecoordy, tiletype, true) == true)
    {
      player->vely = 0;

       // While on stairs auto animation is off
      player->grabstair = true;

      if (tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
      {
        // if next up tile is not an stair tile.
        if (stage->tileActionUnnormalized(player->x + player->utilX, player->y + 8) != mm_tile_actions::TILE_STAIR_BEGIN)
        {
          player->setAnimSeq(Player::ONSTAIR);
          player->isGettingOut = true;
          if (((((int)player->y) % mm_graphs_defs::TILE_SIZE) < 4) ||
            (stage->tileActionUnnormalized(player->x + player->utilX, player->y + mm_graphs_defs::TILE_SIZE) != mm_tile_actions::TILE_STAIR_BEGIN))
          {
            player->y -= 6;
            player->y -= ((int)player->y) % mm_graphs_defs::TILE_SIZE;
            player->y = (player->y / mm_graphs_defs::TILE_SIZE) * mm_graphs_defs::TILE_SIZE;
            player->isGettingOut = false;
            player->onground = true;
            player->grabstair = false;
            player->vely = 8;
            player->setAnimSeq(Player::STANDSTILL);
          }
        }
        else
        {
          player->isGettingOut = false;
        }
      }

      player->x = (tilecoordx * mm_graphs_defs::TILE_SIZE + 1) - player->utilX;
      player->y -= 2.0f;
      player->isFacingDown = true;
    }
  }
}
