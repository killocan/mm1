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
#include "defines.h"

BombManStage::BombManStage()
{
  stage_number = 3;
  STAGE_PATH = "./bombman_stage";
}

void BombManStage::setupStage(bool hardStart)
{
  if (hardStart)
  {
    stage->runtimeLoadCharacter(mm_spritefiles::BOMBMAN_SPRITES);

    stage->runtimeLoadCharacter(mm_spritefiles::KILLERBULLET_SPRITES);
    stage->runtimeLoadCharacter(mm_spritefiles::FLYINGSHELL_SPRITES);

    stage->setOffset(mm_spritefiles::BOMBOMB_SPRITES, 0); // set which sprite color to use.

    bossWarningTiles.push_back(16);
    bossWarningTiles.push_back(17);
    bossWarningColors[0x00bd00] = 0xffffff;
    bossWarningColors[0xffffff] = 0x00bd00;

    int * sectors1 = new int[5];
    sectors1[0] = 42;
    sectors1[1] = 43;
    sectors1[2] = 44;
    sectors1[3] = 45;
    sectors1[4] = -1;
    Character * pKillerBullet1 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_KILLERBULLET,
                                                        137 * 32, 36 * 32, 0, 0, (void*)sectors1);

    int * sectors2 = new int[2];
    sectors2[0] = 28;
    sectors2[1] = -1;
    Character * pKillerBullet2 = stage->createCharacter(mm_tile_actions::TILE_ENEMY_KILLERBULLET,
                                                        191 * 32, 22 * 32, 0, 0, (void*)sectors2);

    int * sectors3 = new int[4];
    sectors3[0] = 11;
    sectors3[1] = 12;
    sectors3[2] = 13;
    sectors3[3] = -1;
    Character * pFlyingShell = stage->createCharacter(mm_tile_actions::TILE_ENEMY_FLYINGSHELL,
                                                      191 * 32, 6 * 32, 0, 0, (void*)sectors3);

    characters_vec.push_back(pKillerBullet1);
    characters_vec.push_back(pKillerBullet2);
    characters_vec.push_back(pFlyingShell);
  }

  BossDoor::BossDoorSetupParam door1;
  door1.size = 2;
  door1.totalSize = 4;
  door1.orientation = 0;
  door1.defaultOpen = false;
  door1.type = 0;

  BossDoor::BossDoorSetupParam door2;
  door2.size = 2;
  door2.totalSize = 4;
  door2.orientation = 1;
  door2.defaultOpen = true;
  door2.type = 1;

  Character * pDoor1C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 254 * 32, 6 * 32, 0, 0, (void*)&door1);
  Character * pDoor2C = stage->createCharacter(mm_tile_actions::TILE_DOOR, 262 * 32, 45 * 32, 0, 0, (void*)&door2);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor1C);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pDoor2C);
  special_chars_vec.push_back(pDoor1C);
  special_chars_vec.push_back(pDoor2C);
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
    if (player->grabstair)
    {
      int tilecoordx = 0;
      int tilecoordy = 0;
      int tiletype = 0;
      if ((player->collisionHor(player->x + player->utilX + (player->isFacingRight ? 8 : 0), player->y + player->vely + player->h, tilecoordx, tilecoordy, true, tiletype) &&
        (tiletype == mm_tile_actions::TILE_STAIR || tiletype == mm_tile_actions::TILE_STAIR_BEGIN) &&
        (((int)(player->x + player->utilX + (player->isFacingRight ? 8 : 0)) % mm_graphs_defs::TILE_SIZE) < (mm_graphs_defs::TILE_SIZE / 2))) ||
        (player->overstair == true && tiletype == 0))
      {
        player->forceAnimation();

        player->vely = 0;

        player->grabstair = true;
        player->isGettingOut = false;

        if (tiletype == mm_tile_actions::TILE_STAIR_BEGIN)
        {
          player->y += 16;
          if (player->isGettingIn == false)
          {
            player->isGettingIn = true;
            player->setAnimSeq(Player::ENTERINGSTAIR);
          }
        }
        else
        {
          if (player->isGettingIn == false) player->setAnimSeq(Player::UPDOWNSTAIR);
        }

        player->isFacingDown = true;
        player->collisionStair(player->x + player->utilX, player->y, tilecoordx, tilecoordy, tiletype, true);
        player->x = (tilecoordx * mm_graphs_defs::TILE_SIZE + 1) - player->utilX;
        player->y += 2;
      }
      else if (player->grabstair == true)
      {
        player->grabstair = false;
      }
    }
    else
    {
      player->doGravitation();
      if (player->onground)
      {
        player->setAnimSeq(Player::STANDSTILL);
      }
    }
  }
}

void BombManStage::createBoss()
{
  Character * pGutsman = stage->createCharacter(mm_tile_actions::BOMBMAN, 266 * 32, 55 * 32, 0, 0, (void*)NULL);
  TemporaryCharacterList::mm_tempCharacterLst.push_back(pGutsman);
}
