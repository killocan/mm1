/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cstdio>

#include "stagemanager.h"

#include "soundpack.h"
#include "sfx.h"

#include "stage.h"
#include "camera.h"
#include "player.h"
#include "stageendscreen.h"
#include "weapons.h"
#include "weaponmenu.h"
#include "energybar.h"
#include "gameovermenu.h"
#include "tileactions.h"
#include "defines.h"

#include "globals.h"
#include "globalgamestate.h"

#include "collision.h"

StageManager::StageManager()
{
  camera = new Camera;
  GlobalCamera::mm_camera = camera;

  cur_stage_state = StageManager::SEARCHING_BOSS_DOOR;

  player = NULL;
  stage  = NULL;
  ssm    = NULL;

  game_pause  = false;
  weapon_menu = false;

  // Reset global timer so we dont warp any time soon
  Clock::clockTicks = 0;

  handlingDoor   = false;
  stopAnimations = false;
  initBossFight  = false;

  dyingTime = 0L;
  dyingSequece = false;

  GlobalGameState::earthquake = false;
  game_over = false;
  playing   = false;

  GameplayGlobals::bDontDrawBars = false;
  stageEndScreen = NULL;
}

StageManager::~StageManager()
{
  Character * curr_character = NULL;
  std::vector<Character *>::iterator it;
  for (it = characters_vec.begin(); it != characters_vec.end(); ++it)
  {
    curr_character = *it;
    delete curr_character;
  }
  //characters_vec.clear();

  delete stage;
  //delete player;
  delete camera;
  GlobalCamera::mm_camera = NULL;

  delete weaponMenu;
}

SceneSoundManager * StageManager::CreateSoundManager()
{
  return NULL;
}

const std::string & StageManager::getStageFilePath() const
{
  return STAGE_PATH;
}

void StageManager::doMegamanSpawning(BITMAP * buffer, FONT * mm_font)
{
  bool played_teletransport_sound = false;

  int y_goal = player->y;
  player->y = ((y_goal / mm_graphs_defs::UTIL_H) * mm_graphs_defs::UTIL_H)- (int)player->h;

  int animCount = 0;

  //These are the default values reseting here just to be clear about it.
  player->setAnimSeq(Player::SPAWNING);
  player->curAnimFrame = 0;

  clear_bitmap(buffer);
  stage->draw(buffer, true, false, true);
  player->calcScreenCoords();
  player->drawCharacter(buffer);
  stage->draw(m_buffer, true, false, false);
  blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
#ifndef DEBUG
  textout_centre_ex(screen, mm_font, "READY",
                    SCREEN_W/2, SCREEN_H/2,
                    makecol(255,255,255), -1);
  rest(3000);
#endif

  bool goingToCheckpoint = true;
  while(goingToCheckpoint == true && animCount < 4)
  {
    if (GlobalGameState::game_logic_tick == 1)
    {
      player->y+=8.0f;
      if (player->y >= y_goal)
      {
        if (played_teletransport_sound == false)
        {
          played_teletransport_sound = true;
          Sounds::mm_sounds->play(TELE);
          player->onground = true;
        }

        player->y = y_goal;

        if (player->curAnimFrameDuration == 0)
        {
          player->nextAnimFrame();
          animCount++;
        }
        else
        {
          player->curAnimFrameDuration--;
        }
      }

      GlobalGameState::game_logic_tick = 0;
    }

    if (stage->has_fg_tiles == false)
    {
      stage->draw(buffer, false);
      player->calcScreenCoords();//*camera);
      player->drawCharacter(buffer);
    }
    else
    {
      stage->draw(buffer, true, false, true);
      player->calcScreenCoords();//*camera);
      player->drawCharacter(buffer);
      stage->draw(m_buffer, true, false, false);
    }

    Vsync::Sync();
    blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
    clear_bitmap(buffer);
  }

  player->y = y_goal;
  //Stand still sequence.
  player->setAnimSeq(Player::STANDSTILL);
}

/**
 * Check for collisions between player bullets and enemies AND
 * between enemies and the player
 */
void StageManager::checkColissionAgainstEnemy()
{
  Character * curr_character = NULL;
  std::vector<Character *>::iterator it;
  for (it = characters_vec.begin(); it != characters_vec.end(); ++it)
  {
    curr_character = *it;
    if (curr_character != player)
    {
      if (curr_character->alive == true)
      {
        for (std::list<mm_weapons::weapon_st>::iterator it_w = GlobalGameState::playerShots.begin();
             it_w != GlobalGameState::playerShots.end();
             ++it_w)
        {
          mm_weapons::weapon_st * pWeapon = &(*it_w);

          if (pWeapon->can_hurt)
          {
            if (Collision::pixelCollision((int) pWeapon->x, (int) pWeapon->y,
                                          pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset),
                                          curr_character->x, curr_character->y, curr_character->getFrame(),
                                          &pWeapon->xcol, &pWeapon->ycol) == true)
            {
              curr_character->hit(pWeapon);
            }
          }
        }

        if (player->isInvincible == false || curr_character->type == mm_spritefiles::GUTSMANROCK_SPRITES)
        {
          if (Collision::pixelCollision((int)player->x,    (int)player->y,   player->getFrame(),
                                         curr_character->x, curr_character->y, curr_character->getFrame(),
                                         &player->xcol, &player->ycol) == true)
          {
            player->hit(curr_character);
            curr_character->collideWithPlayer = true;
          }
        }
      }
    }
  }
}

void StageManager::checkColissionPlayerEnemyBullets()
{
  if (player->isInvincible == false)
  {
    for (std::list<mm_weapons::weapon_st>::iterator it_w = GlobalGameState::enemyShots.begin();
         it_w != GlobalGameState::enemyShots.end();
         ++it_w)
    {
      mm_weapons::weapon_st * pWeapon = &(*it_w);

      if (Collision::pixelCollision((int)pWeapon->x, (int)pWeapon->y,
                                    pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset),
                                    player->x, player->y, player->getFrame()) == true)
      {
        player->hit(pWeapon);
      }
    }
  }
}

#ifdef DEBUG
static void bhmode(BITMAP * bmp)
{
  // Black and White MODE
  uintptr_t pxr = 0;
  uintptr_t pxw = 0;
  unsigned long color;
  uint32_t wsize = (uint32_t) (bmp->w * sizeof(uint32_t));
  for (int y = 0; y < bmp->h; ++y)
  {
    pxr = bmp_read_line(bmp,  y);
    pxw = bmp_write_line(bmp, y);

    for (uint32_t x = 0; x < wsize; x+=sizeof(uint32_t))
    {
      color = bmp_read32(pxr+x);
      // Should be divided by 3, but i'm doing it divided by 4(bit shiffting),
      // just to avoid a division (big time on fps usage +/- 70fps gain)
      color = (((unsigned char)(color)) + ((unsigned char)(color>>8)) + ((unsigned char)(color>>16))) >> 2;
      color = color | (color << 8) | (color << 16);
      bmp_write32(pxw+x, color);
    }
  }
  bmp_unwrite_line(bmp);
}

static void tvmode(BITMAP * bmp)
{
  int mask[3] = {0xFF3F3F, 0x3FFF3F, 0x3F3FFF};
  int maskcounter = 0;

  // CRT TV MODE
  uintptr_t pxr = 0;
  uintptr_t pxw = 0;
  unsigned long color;
  uint32_t wsize = (uint32_t) (bmp->w * sizeof(uint32_t));
  for (int y = 0; y < bmp->h; ++y)
  {
    maskcounter = 0;
    pxr = bmp_read_line(bmp,  y);
    pxw = bmp_write_line(bmp, y);

    for (uint32_t x = 0; x < wsize; x+=sizeof(uint32_t))
    {
      color = bmp_read32(pxr+x) & mask[maskcounter];
      bmp_write32(pxw+x, color);

      ++maskcounter;
      if (maskcounter == 3) maskcounter = 0;
    }
  }
  bmp_unwrite_line(bmp);
}
#endif

// Remove character if its dead.
static bool tempCharacterDone(const Character* value)
{
  if (value == NULL || value->alive == false)
  {
    delete value;
    return true;
  }

  return false;
}

// Kill everyone
static bool tempCharacterKill(const Character* value)
{
  delete value;
  return true;
}

void StageManager::play()
{
  stage  = new Stage(getStageFilePath(), *camera, &player);
  setupStage(true);
  stage->stageNumber = stage_number;
  CurrentCharacterList::mm_characterLst = &characters_vec;

  stage->createEnemies(characters_vec); // Load all enemys.

  ssm        = CreateSoundManager();
  sboos      = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::BOSS_FIGHT_BG_MUSIC], 3147*44, 9543*44);

  weaponMenu = new WeaponMenu(stage_number, player);
  EnergyBar::m_player = player;

  characters_vec.push_back(player);

  m_buffer         = Buffer::buffer;
  FONT   * mm_font = Font::mm_font;

#ifdef DEBUG
  fprintf(stderr, "Tem tiles foreground = [%d]\n", (int)stage->has_fg_tiles);
#endif

  clear_bitmap(m_buffer);
  stage->doCamera(*camera);
  stage->draw(m_buffer, stage->has_fg_tiles);
  blit(m_buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);

  stage->resetReachMaxX();

  Character * curr_character = NULL;
  std::vector<Character *>::iterator it;

  playing = true;
  while(game_over == false)
  {
    // Maybe it resets every life...
    GlobalGameState::bonus_points = 0;

    EnergyBar::m_boss = NULL;

    GlobalGameState::playerShots.clear();
    GlobalGameState::enemyShots.clear();

    Sounds::mm_sounds->stopAll();
    ssm->play(0, true);

    player->reset();
    stage->setWaypoint(player);

    camera->y = ((int)player->y / mm_graphs_defs::UTIL_H) * mm_graphs_defs::UTIL_H;
    stage->doCamera(*camera);
    stage->resetReachMaxX();

    doMegamanSpawning(m_buffer, mm_font);
    //TemporaryCharacterList::mm_tempCharacterLst.remove_if(tempCharacterClean);

    playing = true;
    while(playing == true)
    {
#ifdef DEBUG
      if (key[KEY_Q])
      {
        playing = !(game_over = true);
        break;
      }
#endif

      if (key[KEY_P] && weapon_menu == false)
      {
        if (game_pause == false)
        {
          Sounds::mm_sounds->play(PAUSE);
        }

        game_pause = !game_pause;
        while(key[KEY_P]);
      }

      if (key[KEY_ENTER] && (game_pause == false) && (GlobalGameState::playerShots.size() == 0) && (player->holdingGutsmanRock==false))
      {
        if (findMagneticbeam() == false && stage->finished == false)
        {
          Sounds::mm_sounds->play(PAUSE);

          game_pause = true;
          weapon_menu = true;
        }
      }

      if (GlobalGameState::game_logic_tick == 1)
      {
        GlobalGameState::game_logic_tick = 0;
        ++GlobalGameState::game_logic_lps_count;

        if ((game_pause == false) && (GameplayGlobals::bHoldLogic == false))
        {
          for (it = characters_vec.begin(); it != characters_vec.end(); ++it)
          {
            curr_character = *it;

            if (stage->horz_scroll == false)
              curr_character->checkOnCamera();

            if (curr_character->alive == true)
            {
              if (stopAnimations == false)
                curr_character->handleAnimation();

              if (handlingDoor == false)
              {
                if (stage->horz_scroll == false)
                {
                  curr_character->doGravitation();
                }

                curr_character->doLogic();
              }
            }
          }

          for (std::list<Character *>::iterator i = TemporaryCharacterList::mm_tempCharacterLst.begin();
               i != TemporaryCharacterList::mm_tempCharacterLst.end();
               ++i)
          {
            curr_character = *i;
            if (curr_character == NULL) continue;

            curr_character->checkOnCamera();
            curr_character->doGravitation();
            curr_character->doLogic();

            //TODO: ver se tah colidindo com o megaman em blinking... e mover isso pra um lugar so :D
            if (curr_character->canCollidePlayer == true)
            {
              if (Collision::pixelCollision(curr_character->x, curr_character->y, curr_character->getFrame(),
                                            player->x, player->y, player->getFrame()) == true)
              {
                player->hit(curr_character);
                curr_character->collideWithPlayer = true;
              }
            }
            if (curr_character->canCollideBullet == true)
            {
              for (std::list<mm_weapons::weapon_st>::iterator it_w = GlobalGameState::playerShots.begin();
                   it_w != GlobalGameState::playerShots.end();
                   ++it_w)
              {
                mm_weapons::weapon_st * pWeapon = &(*it_w);
                if (Collision::pixelCollision((int)pWeapon->x, (int)pWeapon->y,
                                               pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset),
                                               curr_character->x, curr_character->y, curr_character->getFrame(),
                                               &curr_character->xcol, &curr_character->ycol) == true)
                {
                  curr_character->hit(pWeapon);
                }
              }
            }
          }
          TemporaryCharacterList::mm_tempCharacterLst.remove_if(tempCharacterDone);

          checkColissionAgainstEnemy();
          checkColissionPlayerEnemyBullets();

          if (handlingDoor == false)
            stage->doCamera(*camera);

          mm_weapons::updateWeapons(stage);

          doStageSpecifics();
        }

        if (GameplayGlobals::bWeaponUpdateRunning == true)
        {
          EnergyBar::updateEnergyBar(player->curWeapon, 0, EnergyBar::m_boss);
        }

        ++Clock::clockTicks;
      }

      if (player->alive == false && dyingSequece == false)
      {
        ssm->stopAll();

        dyingTime = Clock::clockTicks;
        dyingSequece = true;
      }

      if (dyingSequece)
      {
        if (Clock::clockTicks - dyingTime > 220)
        {
          playing = false;
          dyingSequece = false;
          GlobalGameState::lives--;

          if (GlobalGameState::lives < 0)
          {
            game_over = true;
          }
        }
      }

      // Save camera for gutsman earth quake
      int old_camera_x = GlobalCamera::mm_camera->x;
      int old_camera_y = GlobalCamera::mm_camera->y;

      if (GlobalGameState::earthquake == true)
      {
        if (player->onground)
          player->getStunned();

        static int eq_x=-4, eq_y=4;
        static int quake_values[4] = {-5,-3,3,5};

        int old_eq = eq_x;
        while (eq_x == old_eq) eq_x = quake_values[rand()%4];
        old_eq = eq_y;
        while (eq_y == old_eq) eq_y = quake_values[rand()%4];

        --GlobalGameState::earthquakecount;
        if (GlobalGameState::earthquakecount <= 0)
        {
          //player->isStunned = false;
          GlobalGameState::earthquake = false;
        }

        GlobalCamera::mm_camera->x += eq_x<<1;
        GlobalCamera::mm_camera->y += eq_y<<1;
      }

      //Most of time there are no FG tiles.
      if (stage->has_fg_tiles == false)
      {
        stage->draw(m_buffer, false);

        drawCharacters();

        mm_weapons::drawWeapons(m_buffer);
      }
      else if (stage->cameraSectorHasFgTiles(*camera, *player) == true)
      {
        clear_bitmap(m_buffer);

        stage->draw(m_buffer, true, false, true);

        drawCharacters();

        mm_weapons::drawWeapons(m_buffer);

        stage->draw(m_buffer, true, false, false);
      }
      else
      {
        clear_bitmap(m_buffer);
        stage->draw(m_buffer, true);

        drawCharacters();

        mm_weapons::drawWeapons(m_buffer);
      }

      // Restore camera x,y.
      GlobalCamera::mm_camera->x = old_camera_x;
      GlobalCamera::mm_camera->y = old_camera_y;

      if (weapon_menu == true)
      {
        weapon_menu = weaponMenu->inputAndDraw(m_buffer);
        if (weapon_menu == false)
        {
          game_pause = false;
        }
      }

      EnergyBar::drawEnerybar(m_buffer, 48, 34, mm_weapons::W_MEGA_BUSTER);
      if (player->curWeapon != mm_weapons::W_MEGA_BUSTER)
      {
        EnergyBar::drawEnerybar(m_buffer, 32, 34, player->curWeapon);
      }
      if (EnergyBar::m_boss != NULL)
      {
        EnergyBar::drawEnerybar(m_buffer, 78, 34, player->curWeapon, true);
      }

#ifdef DEBUG
      if (key[KEY_T]) tvmode(m_buffer);
      if (key[KEY_B]) bhmode(m_buffer);
      textprintf_right_ex(m_buffer, font, SCREEN_W, 10, makecol(255,255,255), 0, "X:[%f] Y:[%f] VX:[%f] VY:[%f]",
                    player->x, player->y, player->velx, player->vely);
      textprintf_right_ex(m_buffer, font, SCREEN_W, 20, makecol(255,255,255), 0, "camera.x:[%d] camera.y:[%d]",
                    camera->x, camera->y);

      int yd = ((int)player->y)/mm_graphs_defs::TILE_SIZE;
      int xd = ((int)player->x)/mm_graphs_defs::TILE_SIZE;
      int ydesl =  yd / mm_graphs_defs::TILES_Y;
      int xdesl =  xd / mm_graphs_defs::TILES_X;
      int sector = ydesl*(stage->max_x / mm_graphs_defs::TILES_X) + xdesl;

      textprintf_right_ex(m_buffer, font, SCREEN_W, 30, makecol(255,255,255), 0, "SECTOR = [%d]", sector);

      textprintf_right_ex(m_buffer, font, SCREEN_W, 40, makecol(255,255,255), 0, "SX:[%d] SY:[%d]",
                    player->sx, player->sy);
#endif

      textprintf_right_ex(m_buffer, font, SCREEN_W, 1, makecol(255,255,255), 0, "FPS:[%d] LPS[%d]", 
                          GlobalGameState::sm_fps, GlobalGameState::game_logic_lps);

      if (stageEndScreen != NULL)
        stageEndScreen->draw(m_buffer);

      Vsync::Sync();
      blit(m_buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);

      ++GlobalGameState::sm_fps_count;
    }

    if (game_over)
    {
      if (!stage->finished)
        game_over = GameoverMenu::choice(m_buffer);

      if (!game_over)
      {
        GlobalGameState::lives = 2;
      }
    }

    if (game_over == false)
    {
      // Recreate temp chars and doors.
      weaponMenu->selected = 7;
      player->curWeapon = mm_weapons::W_MEGA_BUSTER;
      player->changeWeapon();
      stage->horz_scroll = false;
      TemporaryCharacterList::mm_tempCharacterLst.remove_if(tempCharacterKill);
      special_chars_vec.clear();
      setupStage(false);
    }
  }

  // Just in case?!
  TemporaryCharacterList::mm_tempCharacterLst.remove_if(tempCharacterKill);

  ssm->stopAll();
  delete ssm;

  sboos->stopAll();
  delete sboos;
}

bool StageManager::findMagneticbeam()
{
  Character * curr_character = NULL;

  for (std::list<Character *>::iterator i = TemporaryCharacterList::mm_tempCharacterLst.begin();
    i != TemporaryCharacterList::mm_tempCharacterLst.end();
    ++i)
  {
    curr_character = *i;
    if (curr_character != NULL && curr_character->alive && curr_character->type == mm_spritefiles::WEAPONS_MAGNETIC)
    {
      return true;
    }
  }

  return false;
}

void StageManager::drawCharacters()
{
  Character * curr_character = NULL;
  std::vector<Character *>::iterator it;

  for (it = characters_vec.begin(); it != characters_vec.end(); ++it)
  {
    curr_character = *it;

    if (curr_character->alive == true)
    {
      curr_character->calcScreenCoords();
      curr_character->drawCharacter(m_buffer);
    }
  }
  for (std::list<Character *>::iterator i = TemporaryCharacterList::mm_tempCharacterLst.begin();
    i != TemporaryCharacterList::mm_tempCharacterLst.end();
    ++i)
  {
    curr_character = *i;
    if (curr_character != NULL && curr_character->alive)
    {
      curr_character->calcScreenCoords();
      curr_character->drawCharacter(m_buffer);
    }
  }
}

void StageManager::killThemAll()
{
  std::vector<Character *>::iterator it;
  Character * curr_character = NULL;
  for (it = characters_vec.begin(); it != characters_vec.end(); ++it)
  {
    curr_character = *it;
    if (curr_character != player)
    {
      curr_character->alive = false;
    }
  }
}

int StageManager::bossWarning()
{
  static unsigned long warningTimer = Clock::clockTicks;
  static int counter = 0;

  if (warningTimer == 0)
    warningTimer = Clock::clockTicks;

  if ((Clock::clockTicks - warningTimer) > 5)
  {
    warningTimer = Clock::clockTicks;

    for (unsigned int i = 0; i < bossWarningTiles.size(); ++i)
    {
      BITMAP * bmp = stage->getTile(bossWarningTiles[i]);
      if (bmp == NULL)
        continue;

      unsigned int color;
      for (int x = 0; x < bmp->w; ++x)
      {
        for (int y = 0; y < bmp->h; ++y)
        {
          color = _getpixel32(bmp, x, y);
          if (bossWarningColors.count(color) > 0)
            _putpixel32(bmp, x, y, bossWarningColors[color]);
        }
      }
    }

    ++counter;
    if (counter == 10)
    {
      counter = 0;
      warningTimer = 0;

      return -1; // It means: i'm done.
    }
  }

  return 0;
}

void StageManager::doStageSpecifics()
{
  static BossDoor * door = NULL;
  static int autoMovePlayerCount = 0;

  switch(cur_stage_state)
  {
    case SEARCHING_BOSS_DOOR:
    {
      if (stage->checkForBoss(camera->x, camera->y) == true && stage->horz_scroll==false)
      {
        cur_stage_state = StageManager::BOSS_WARNING;

        handlingDoor = true;

        initBossFight = true;
        killThemAll();

        Sounds::mm_sounds->stopAll();
        ssm->stopAll();

        if (sboos->isPlaying() == false) sboos->play(0, true);
        break;
      }

      std::vector<Character *>::iterator it;
      Character * curr_character = NULL;
      for (it = special_chars_vec.begin(); it != special_chars_vec.end(); ++it)
      {
        curr_character = *it;

        if (curr_character->type == mm_tile_actions::TILE_DOOR)
        {
          if (curr_character->collideWithPlayer == true)
          {
            door = dynamic_cast<BossDoor*> (curr_character);
            if (door->hasBeenUsed == false)
            {
              door->openDoor();

              killThemAll();

              cur_stage_state = StageManager::HANDLING_DOOR;
              handlingDoor = true;
              stopAnimations = true;
              doorTiming = Clock::clockTicks;
            }
          }
        }
      }
    }
    break;
    case HANDLING_DOOR:
    {
      if (door != NULL && door->curState == BossDoor::WAITING)
      {
        //stopAnimations = false;
        int xOffset = door->config.type == 0 ? (mm_graphs_defs::TILES_X*1.555) : (mm_graphs_defs::TILES_X*1.3);
        if (autoMovePlayerCount < (mm_graphs_defs::TILES_X * 2))
        {
          player->forceAnimation();

          if ((float)autoMovePlayerCount < (float)xOffset)
          {
            player->goRight();
          }

          if ((Clock::clockTicks - doorTiming) > 2)
          {
            doorTiming = Clock::clockTicks;
            autoMovePlayerCount++;
            camera->x += mm_graphs_defs::TILE_SIZE/2;
          }
        }
        else
        {
          cur_stage_state = StageManager::SEARCHING_BOSS_DOOR;
          if (door->config.type == 0)
          {
            stopAnimations = false;
            handlingDoor = false;
          }
          autoMovePlayerCount = 0;

          // Put on a method called "endireitarmegaman" hahaha
          player->vely = 1.0f;
          player->blinkCount = 28;
          player->bDraw = true;
          player->bHide = false;
          player->isHitAnimOn = false;
          player->isInvincible = false;
          player->isFacingRight = true;

          door->collideWithPlayer = false;
          door->blockPassage();
        }
      }
    }
    break;
    case BOSS_WARNING:
    {
      if (bossWarning() < 0 && player->onground)
      {
        cur_stage_state = StageManager::INITING_BOSS_FIGHT;
      }
      else
      {
        if (player->onground == false)
        {
          player->lockmoves = true;
          player->doGravitation();
        }
      }
    }
    break;
    case INITING_BOSS_FIGHT:
    {
      player->setAnimSeq(Player::STANDSTILL);

      int lastItem = special_chars_vec.size()-1;
      door = dynamic_cast<BossDoor*> (special_chars_vec[lastItem]);
      if (door->curState == BossDoor::OPENED)
      {
        door->closeDoor();
      }
      else if (door->curState == BossDoor::WAITING)
      {
        player->lockmoves = false;
        createBoss();
        EnergyBar::updateEnergyBar(mm_weapons::W_MEGA_BUSTER, 28, true);

        door = NULL;
        cur_stage_state = FIGHT; //Set FightingBoss to true
      }
      else
      {
        //initBossFight = false;
      }
    }
    break;
    case FIGHT:
    {
      if (EnergyBar::m_boss != NULL)
      {
        if (EnergyBar::m_boss->curState != 0 && handlingDoor)
        {
          BossDoor * tmpDoor = dynamic_cast<BossDoor*> (special_chars_vec[special_chars_vec.size() - 1]);
          if (tmpDoor != NULL && tmpDoor->config.type == 1)
          {
            stopAnimations = false;
            handlingDoor = false;
          }
          //handlingDoor = false;
        }
      }

      if (EnergyBar::m_boss == NULL)
      {
        Sounds::mm_sounds->play(BOMBOMB_HIT);

        sboos->stopAll();

        TemporaryCharacterList::mm_tempCharacterLst.push_back(stage->createCharacter(mm_tile_actions::STAGE_ORB, 
                                                                                     camera->x + (mm_graphs_defs::UTIL_W / 2) - 16,
                                                                                     camera->y+16, 0,0));
        cur_stage_state = BOSS_DEAD;
      }
    }
    break;
    case BOSS_DEAD:
    {
      if (stage->finished)
      {
        handlingDoor = true;
        stopAnimations = true;
        cur_stage_state = END_STAGE;
        stageEndScreen = new StageEndScreen();
      }
    }
    break;
    case END_STAGE:
    {
      if (stageEndScreen->play(stage_number))
      {
        playing = false;
        game_over = true;

        delete stageEndScreen;
      }
    }
    break;
  }

  return;
}

void StageManager::createBoss()
{
  ;
}

void StageManager::setupStage(bool hardStart)
{
  return;
}
