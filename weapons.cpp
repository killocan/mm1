/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <cmath>

#include "weapons.h"
#include "tileactions.h"
#include "globalgamestate.h"
#include "globals.h"
#include "defines.h"
#include "spritefiles.h"
#include "stage.h"
#include "character.h"
#include "player.h"
#include "camera.h"
#include "animsequence.h"
#include "collision.h"
#include "scenesoundmanager.h"
#include "sfx.h"

static void handleBombFragment(mm_weapons::weapon_st * pWeapon)
{
  static int bomb_fragment_frames[][14] = {{0,0,2,3,3,3,1,2,3,3,0,1,2,3},{0,0,2,1,2,3,4,3,0,1,4,2,4,3}};

  if ((Clock::clockTicks - pWeapon->ticks) > 1)
  {
    pWeapon->ticks = Clock::clockTicks;

    if (pWeapon->life > 0)
    {
      pWeapon->x += pWeapon->vx;
      pWeapon->y += pWeapon->vy;

      pWeapon->frameOffset = bomb_fragment_frames[pWeapon->subtype_num][14 - pWeapon->life];

      pWeapon->life--;
    }
    else
      pWeapon->alive = false;
  }
}

void mm_weapons::createExplosionParts(Stage * stage, float x, float y, bool player_created)
{
  static float vx_0[] = {1.0f, -1.0f, 0.0f,  0.0f, 3.0f, 3.0f,  -3.0f, -3.0f};
  static float vy_0[] = {0.0f, 0.0f,  -1.0f, 1.0f, 3.0f, -3.0f, 3.0f,  -3.0f};
  static float vx_1[] = {4.0f, -4.0f, 0.0f,  0.0f};
  static float vy_1[] = {0.0f, 0.0f,  -4.0f, 4.0f};

  for (int i = 0; i < 8; ++i)
  {
    mm_weapons::weapon_st explosion_part;

    explosion_part.x = x;
    explosion_part.y = y;

    explosion_part.vx = vx_0[i];
    explosion_part.vy = vy_0[i];

    explosion_part.can_hurt = true;
    explosion_part.subtype = true;
    explosion_part.subtype_num = 0;
    explosion_part.alive = true;
    explosion_part.life = 14;
    explosion_part.ticks = Clock::clockTicks;

    explosion_part.frameOffset = 0;
    explosion_part.bulletSpriteShet = stage->getAnimSeq(mm_spritefiles::EXPLOSIONLITTLE_SPRITES);
    explosion_part.weapon = mm_weapons::W_BOMBMAN_GUN;

    explosion_part.w = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->w;
    explosion_part.h = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->h;

    if (player_created)
      GlobalGameState::playerShots.push_back(explosion_part);
    else
      GlobalGameState::enemyShots.push_back(explosion_part);
  }
  for (int i = 0; i < 4; ++i)
  {
    mm_weapons::weapon_st explosion_part;

    explosion_part.x = x;
    explosion_part.y = y;

    explosion_part.vx = vx_1[i];
    explosion_part.vy = vy_1[i];

    explosion_part.can_hurt = true;
    explosion_part.subtype = true;
    explosion_part.subtype_num = 1;
    explosion_part.alive = true;
    explosion_part.life = 14;
    explosion_part.ticks = Clock::clockTicks;

    explosion_part.frameOffset = 0;
    explosion_part.bulletSpriteShet = stage->getAnimSeq(mm_spritefiles::EXPLOSIONLITTLE_SPRITES);
    explosion_part.weapon = mm_weapons::W_BOMBMAN_GUN;

    explosion_part.w = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->w;
    explosion_part.h = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->h;

    if (player_created)
      GlobalGameState::playerShots.push_back(explosion_part);
    else
      GlobalGameState::enemyShots.push_back(explosion_part);
  }
}

static void handleBomb(mm_weapons::weapon_st * pWeapon, Stage * stage)
{
  int tilecoordx, tilecoordy;

  pWeapon->vy += mm_player_defs::GRAVITATION;

  if (!stage->genericColHor(pWeapon->x, pWeapon->w, pWeapon->y + pWeapon->vy + (pWeapon->vy > 0 ? pWeapon->h : 0), tilecoordx, tilecoordy, pWeapon->vy > 0))
  {
    pWeapon->y += pWeapon->vy;
  }
  else
  {
    if (pWeapon->hyper_bomb_should_bounce && pWeapon->hyper_bomb_bounce_count < 4)
    {
      if (pWeapon->vy > 0.0f)
        pWeapon->y = tilecoordy * mm_graphs_defs::TILE_SIZE - pWeapon->h-1;

      pWeapon->hyper_bomb_bounce_count++;
      pWeapon->vy *= 0.70f;
      pWeapon->vx *= 0.6f;
      pWeapon->vy *= -1;
    }
    else
    {
      pWeapon->vy = 0.0f;
    }
  }

  if (!stage->genericColVer(pWeapon->x + pWeapon->vx + (pWeapon->vx > 0 ? pWeapon->w : 0), pWeapon->y, pWeapon->h, tilecoordx, tilecoordy))
  {
    if (pWeapon->hyper_bomb_bounce_count < 4)
      pWeapon->x += pWeapon->vx;
  }
  else
  {
    pWeapon->vx = 0.0f;
  }

  pWeapon->life--;
  if (pWeapon->life <= 0)
  {
    pWeapon->alive = false;
    Sounds::mm_sounds->play(BOMBOMB_HIT);
    mm_weapons::createExplosionParts(stage, pWeapon->x, pWeapon->y, true);
  }
}

static void handleThunderBeam(mm_weapons::weapon_st * pWeapon)
{
  static int thunder_beam_frames[][7] = {{0,1,2},{5,6,7,8,9,3,4}};

  if ((Clock::clockTicks - pWeapon->ticks) > 5)
  {
    pWeapon->ticks = Clock::clockTicks;

    pWeapon->frameOffset = thunder_beam_frames[pWeapon->vy == 0.0f][pWeapon->current_frame];
    ++pWeapon->current_frame;

    if (pWeapon->current_frame >= pWeapon->frames)
      pWeapon->current_frame = 0;
  }

  pWeapon->x += pWeapon->vx;
  pWeapon->y += pWeapon->vy;
}

static void handleRollingCutter(mm_weapons::weapon_st * pWeapon, Stage * stage)
{
  if (pWeapon->cutter_recalc)
  {
    if (pWeapon->cutter_foward)
    {
      pWeapon->cutter_recalc = false;

      pWeapon->cutter_target_x = pWeapon->vx > 0.0f ? pWeapon->x + 160 : pWeapon->x - 160; // 160 - Distance to target
      pWeapon->cutter_target_y = pWeapon->y;

      int mid_x1 = pWeapon->vx > 0.0f ? pWeapon->x + (160 / 2) : pWeapon->x - (160 / 2);
      int mid_y1 = pWeapon->y - 100; // Radius

      pWeapon->cutter_ctrl_points[0] = pWeapon->x;
      pWeapon->cutter_ctrl_points[1] = pWeapon->y;
      pWeapon->cutter_ctrl_points[2] = pWeapon->x;
      pWeapon->cutter_ctrl_points[3] = pWeapon->y;
      pWeapon->cutter_ctrl_points[4] = mid_x1;
      pWeapon->cutter_ctrl_points[5] = mid_y1;
      pWeapon->cutter_ctrl_points[6] = pWeapon->cutter_target_x;
      pWeapon->cutter_ctrl_points[7] = pWeapon->cutter_target_y;

      calc_spline(pWeapon->cutter_ctrl_points, CUTTER_CURVE_PNTS, pWeapon->cutter_curve_X, pWeapon->cutter_curve_Y);
    }
    else
    {
      //int mid_x1 = pWeapon->vx > 0.0f ? pWeapon->cutter_origin_x + (160 / 2) : pWeapon->cutter_origin_x - (160 / 2);
      //int mid_x1 = pWeapon->vx > 0.0f ? stage->m_player->x + (160 / 2) : stage->m_player->x - (160 / 2);
      int mid_x1 = (stage->m_player->x + stage->m_player->utilX) + ((pWeapon->cutter_target_x - stage->m_player->x) / 2);
      int mid_y1 = pWeapon->cutter_origin_y + 100;

      pWeapon->cutter_ctrl_points[0] = pWeapon->cutter_target_x;
      pWeapon->cutter_ctrl_points[1] = pWeapon->cutter_target_y;
      pWeapon->cutter_ctrl_points[2] = mid_x1;
      pWeapon->cutter_ctrl_points[3] = mid_y1;
      pWeapon->cutter_ctrl_points[4] = stage->m_player->x + (pWeapon->vx<0.0f?20:10);
      pWeapon->cutter_ctrl_points[5] = stage->m_player->y + 20;
      pWeapon->cutter_ctrl_points[6] = stage->m_player->x + (pWeapon->vx<0.0f?20:10);
      pWeapon->cutter_ctrl_points[7] = stage->m_player->y + 20;

      calc_spline(pWeapon->cutter_ctrl_points, CUTTER_CURVE_PNTS, pWeapon->cutter_curve_X, pWeapon->cutter_curve_Y);
    }
  }

  if (pWeapon->cutter_foward == false)
  {
    /*
    float x1 = pWeapon->x + (pWeapon->w/2);
    float y1 = pWeapon->y + (pWeapon->h/2);
    float x2 = stage->m_player->x + (stage->m_player->w/2);
    float y2 = stage->m_player->y + (stage->m_player->h/2);
     */
    //int distance = sqrt( pow(x2 - x1, 2) + pow(y2 - y1, 2) );

    if (//(distance > 5 && distance < 15) &&
        Collision::pixelCollision((int) pWeapon->x, (int) pWeapon->y,
                                  pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset),
                                  stage->m_player->x, stage->m_player->y, stage->m_player->getFrame()))
    {
      pWeapon->alive = false;
      return;
    }
  }

  pWeapon->x = pWeapon->cutter_curve_X[pWeapon->current_point];
  pWeapon->y = pWeapon->cutter_curve_Y[pWeapon->current_point];

  ++pWeapon->current_point;
  if (pWeapon->current_point >= CUTTER_CURVE_PNTS)
  {
    pWeapon->current_point = 0;
    pWeapon->cutter_recalc = true;
    pWeapon->cutter_foward = !pWeapon->cutter_foward;
    pWeapon->life--;
  }

  if (pWeapon->life == 0)
  {
    pWeapon->alive = false;
    return;
  }

  static int rolling_cutter_frames[4] = {3,2,1,0};

  if ((Clock::clockTicks - pWeapon->ticks) > 4)
  {
    pWeapon->ticks = Clock::clockTicks;

    pWeapon->frameOffset = rolling_cutter_frames[pWeapon->current_frame];
    ++pWeapon->current_frame;

    if (pWeapon->current_frame >= pWeapon->frames)
      pWeapon->current_frame = 0;
  }
}

static void handleFireStormProjectile(mm_weapons::weapon_st * pWeapon)
{
  pWeapon->x += pWeapon->vx;

  pWeapon->x_dist += pWeapon->vx;
  if (abs((int)pWeapon->x_dist) > 53)
  {
    pWeapon->x_dist = 0;

    ++pWeapon->current_frame;
    if (pWeapon->current_frame >= pWeapon->frames)
      pWeapon->current_frame = 0;

    pWeapon->frameOffset = pWeapon->frames + pWeapon->current_frame;
  }
}

static void handleFireStormShield(mm_weapons::weapon_st * pWeapon, Stage * stage)
{
  int frameNumber[] = {0, 1, 2, 1};
  int x_0[] = {-20+5,-30+5,-20+5,
               mm_player_defs::HALFPLAYERWIDTH+5,
               mm_player_defs::PLAYERWIDTH+5, mm_player_defs::PLAYERWIDTH+10+5, mm_player_defs::PLAYERWIDTH+5,
               mm_player_defs::HALFPLAYERWIDTH+5};

  int y_0[] = {mm_player_defs::PLAYERHEIGHT, mm_player_defs::PLAYERHEIGHT/2, 0,
               -20,
               0, mm_player_defs::PLAYERHEIGHT/2, mm_player_defs::PLAYERHEIGHT,
               mm_player_defs::PLAYERHEIGHT+10};

  int num_frames = sizeof(frameNumber) / sizeof(int);
  int num_elements = sizeof(x_0) / sizeof(float);

  if ((Clock::clockTicks - pWeapon->ticks) > 1)
  {
    pWeapon->ticks = Clock::clockTicks;

    pWeapon->x = (stage->m_player->x) + x_0[pWeapon->current_point];
    pWeapon->y = (stage->m_player->y) + y_0[pWeapon->current_point];

    pWeapon->life--;
    if (pWeapon->life == 0)
      pWeapon->alive = false;

    ++pWeapon->current_point;
    if (pWeapon->current_point == num_elements)
    {
      pWeapon->current_point = 0;
    }

    ++pWeapon->current_frame;
    if (pWeapon->current_frame >= num_frames)
    pWeapon->current_frame = 0;

    pWeapon->frameOffset = frameNumber[pWeapon->current_frame];
  }
}

static void handleIceSlasher(mm_weapons::weapon_st * pWeapon)
{
  pWeapon->x += pWeapon->vx;

  pWeapon->x_dist += pWeapon->vx;
  if (abs((int)pWeapon->x_dist) > 128)
  {
    pWeapon->x_dist = 0;
    pWeapon->frameOffset = 1 - pWeapon->frameOffset;
  }
}

static void doWeaponSpecifics(mm_weapons::weapon_st * pWeapon, Stage * stage)
{
  switch(pWeapon->weapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
    {
      pWeapon->x += pWeapon->vx;
      pWeapon->y += pWeapon->vy;
    }
    break;
    case mm_weapons::W_PLATFORM_CREATOR:
    {
      ;
    }
    break;
    case mm_weapons::W_CUTMAN_GUN:
    {
      handleRollingCutter(pWeapon, stage);
    }
    break;
    case mm_weapons::W_GUTSMAN_GUN:
    {
      ;
    }
    break;
    case mm_weapons::W_ICEMAN_GUN:
    {
      handleIceSlasher(pWeapon);
    }
    break;
    case mm_weapons::W_BOMBMAN_GUN:
    {
      if (pWeapon->subtype == false)
        handleBomb(pWeapon, stage);
      else
        handleBombFragment(pWeapon);
    }
    break;
    case mm_weapons::W_FIREMAN_GUN:
    {
      if (pWeapon->subtype)
        handleFireStormShield(pWeapon, stage);
      else
        handleFireStormProjectile(pWeapon);

    }
    break;
    case mm_weapons::W_ELECMAN_GUN:
    {
      handleThunderBeam(pWeapon);
    }
    break;
    default:
    break;
  }
}

static bool isDead (const mm_weapons::weapon_st& value) { return (value.alive == false); }
void mm_weapons::updateWeapons(Stage * stage)
{
  for (std::list<mm_weapons::weapon_st>::iterator it = GlobalGameState::playerShots.begin();
       it != GlobalGameState::playerShots.end();
       ++it)
  {
    weapon_st * pWeapon = &(*it);

    // Each weapon has particular behaviour.
    doWeaponSpecifics(pWeapon, stage);

    // Translate to screen coords
    pWeapon->sx = ((int) pWeapon->x) - GlobalCamera::mm_camera->x;
    pWeapon->sy = ((int) pWeapon->y) - GlobalCamera::mm_camera->y;

    // Test if it's inside camera view
    if (pWeapon->sx < -pWeapon->w || pWeapon->sx > mm_graphs_defs::UTIL_W ||
        pWeapon->sy < -pWeapon->h || pWeapon->sy > mm_graphs_defs::UTIL_H)
    {
      pWeapon->alive = false;
    }
  }

  GlobalGameState::playerShots.remove_if(isDead);

  for (std::list<mm_weapons::weapon_st>::iterator it = GlobalGameState::enemyShots.begin();
       it != GlobalGameState::enemyShots.end();
       ++it)
  {
    weapon_st * pWeapon = &(*it);

    // Each weapon has particular behaviour.
    doWeaponSpecifics(pWeapon, stage);

    //pWeapon->x += pWeapon->vx;
    //pWeapon->y += pWeapon->vy;

    pWeapon->sx = ((int) pWeapon->x) - GlobalCamera::mm_camera->x;
    pWeapon->sy = ((int) pWeapon->y) - GlobalCamera::mm_camera->y;

    if (pWeapon->sx < -pWeapon->w || pWeapon->sx > mm_graphs_defs::UTIL_W ||
        pWeapon->sy < -pWeapon->h || pWeapon->sy > mm_graphs_defs::UTIL_H)
    {
      pWeapon->alive = false;
    }
  }

  GlobalGameState::enemyShots.remove_if(isDead);
}

void mm_weapons::drawWeapons(BITMAP * buffer)
{
  for (std::list<mm_weapons::weapon_st>::iterator it = GlobalGameState::playerShots.begin();
       it != GlobalGameState::playerShots.end();
       ++it)
  {
    weapon_st * pWeapon = &(*it);

    if (pWeapon->vx > 0)
    {
      draw_sprite(buffer, pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset), pWeapon->sx, pWeapon->sy);
    }
    else
    {
      draw_sprite_h_flip(buffer, pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset), pWeapon->sx, pWeapon->sy);
    }
  }

  for (std::list<mm_weapons::weapon_st>::iterator it = GlobalGameState::enemyShots.begin();
       it != GlobalGameState::enemyShots.end();
       ++it)
  {
    weapon_st * pWeapon = &(*it);

    if (pWeapon->vx > 0)
    {
      draw_sprite(buffer, pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset), pWeapon->sx, pWeapon->sy);
    }
    else
    {
      draw_sprite_h_flip(buffer, pWeapon->bulletSpriteShet->getFrame(pWeapon->frameOffset), pWeapon->sx, pWeapon->sy);
    }
  }
}

void mm_weapons::createMegaBuster(Player * player)
{
  mm_weapons::weapon_st mega_buster;
  mega_buster.can_hurt = true;
  if (player->grabstair == false)
  {
    mega_buster.x = (float)(player->x + 58);
    if (player->isFacingRight == false)
    {
      mega_buster.x -= 64.0f;
    }

    if (player->onground == true)
    {
      mega_buster.y = (float)(player->y + 32.0f);
    }
    else
    {
      if (player->isFacingRight == false)
        mega_buster.x += 10.0f;
      else
        mega_buster.x -= 10.0f;

      mega_buster.y = (float)(player->y + 22.0f);
    }
  }
  else
  {
    mega_buster.x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      mega_buster.x -= 48.0f;
    }

    mega_buster.y = (float)(player->y + 22.0f);
  }

  mega_buster.vx = 7.5f;
  if (player->isFacingRight == false)
  {
    mega_buster.vx *= -1.0f;
  }
  mega_buster.vy = 0.0f;

  mega_buster.alive  = true;

  mega_buster.frameOffset = 0;
  mega_buster.bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_SPRITES);
  mega_buster.weapon = mm_weapons::W_MEGA_BUSTER;

  mega_buster.w = mega_buster.bulletSpriteShet->getFrame(mega_buster.frameOffset)->w;
  mega_buster.h = mega_buster.bulletSpriteShet->getFrame(mega_buster.frameOffset)->h;

  GlobalGameState::playerShots.push_back(mega_buster);
}

void mm_weapons::createMegaBuster(Character * character, int x, int y, float vx, float vy, int offset)
{
  mm_weapons::weapon_st mega_buster;
  mega_buster.can_hurt = true;
  mega_buster.x  = x;
  mega_buster.y  = y;
  mega_buster.vx = vx;
  mega_buster.vy = vy;

  mega_buster.alive  = true;

  mega_buster.frameOffset = offset;
  mega_buster.bulletSpriteShet = character->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_SPRITES);
  mega_buster.weapon = mm_weapons::W_MEGA_BUSTER;

  mega_buster.w = mega_buster.bulletSpriteShet->getFrame(mega_buster.frameOffset)->w;
  mega_buster.h = mega_buster.bulletSpriteShet->getFrame(mega_buster.frameOffset)->h;

  GlobalGameState::enemyShots.push_back(mega_buster);
}

void mm_weapons::createIceSlasher(Player * player)
{
  mm_weapons::weapon_st ice_slasher;
  ice_slasher.can_hurt = true;
  if (player->grabstair == false)
  {
    ice_slasher.x = (float)(player->x + 58);
    if (player->isFacingRight == false)
    {
      ice_slasher.x -= 64.0f;
    }

    if (player->onground == true)
    {
      ice_slasher.y = (float)(player->y + 22.0f);
    }
    else
    {
      ice_slasher.y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    ice_slasher.x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      ice_slasher.x -= 48.0f;
    }

    ice_slasher.y = (float)(player->y + 12.0f);
  }

  ice_slasher.vx = 6.5f;
  if (player->isFacingRight == false)
  {
    ice_slasher.vx *= -1.0f;
  }
  ice_slasher.vy = 0.0f;

  ice_slasher.alive  = true;

  ice_slasher.frameOffset = 0;
  ice_slasher.bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_ICEMAN);
  ice_slasher.weapon = mm_weapons::W_ICEMAN_GUN;

  ice_slasher.w = ice_slasher.bulletSpriteShet->getFrame(ice_slasher.frameOffset)->w;
  ice_slasher.h = ice_slasher.bulletSpriteShet->getFrame(ice_slasher.frameOffset)->h;

  GlobalGameState::playerShots.push_back(ice_slasher);
}

void mm_weapons::createIceSlasher(Character * character, int x, int y, float vx, float vy, int offset)
{
  return;
}

void mm_weapons::createHyperBomb(Player * player)
{
  mm_weapons::weapon_st bomb;
  bomb.can_hurt = false;

  bomb.hyper_bomb_should_bounce = true;
  bomb.life = 200;

  if (player->grabstair == false)
  {
    bomb.x = (float)(player->x + 38);
    if (player->isFacingRight == false)
    {
      bomb.x -= 44.0f;
    }

    if (player->onground == true)
    {
      bomb.y = (float)(player->y + 22.0f);
    }
    else
    {
      bomb.y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    bomb.x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      bomb.x -= 48.0f;
    }

    bomb.y = (float)(player->y + 12.0f);
  }

  bomb.vx = 4.5f;
  if (player->isFacingRight == false)
  {
    bomb.vx *= -1.0f;
  }
  bomb.vy = -9.2f;

  bomb.alive  = true;

  bomb.frameOffset = 0;
  bomb.bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_BOMBMAN);
  bomb.weapon = mm_weapons::W_BOMBMAN_GUN;

  bomb.w = bomb.bulletSpriteShet->getFrame(bomb.frameOffset)->w;
  bomb.h = bomb.bulletSpriteShet->getFrame(bomb.frameOffset)->h;

  int tilecoordx, tilecoordy;
  if (player->cur_stage->genericColVer(bomb.x + bomb.vx + (bomb.vx > 0 ? bomb.w : 0), bomb.y, bomb.h, tilecoordx, tilecoordy))
  {
    bomb.vx = 0.0f;
    bomb.vy = 0.0f;
    bomb.x = (float)(player->x + player->utilX);
  }

  GlobalGameState::playerShots.push_back(bomb);
}

void mm_weapons::createHyperBomb(Character * character, int x, int y, float vx, float vy, int offset)
{
  return;
}

void mm_weapons::createThunderBeam(Player * player)
{
  mm_weapons::weapon_st thunder_beam[3];
  thunder_beam[0].can_hurt = thunder_beam[1].can_hurt = thunder_beam[2].can_hurt = true;

  float x, y;

  if (player->grabstair == false)
  {
    x = (float)(player->x + 54);
    if (player->isFacingRight == false)
    {
      x -= 120.0f;
    }

    if (player->onground == true)
    {
      y = (float)(player->y + 12.0f);
    }
    else
    {
      y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      x -= 58.0f;
    }

    y = (float)(player->y + 12.0f);
  }

  thunder_beam[0].frames = 7;
  thunder_beam[1].frames = thunder_beam[2].frames = 3;

  thunder_beam[0].ticks = thunder_beam[1].ticks = thunder_beam[2].ticks = Clock::clockTicks;

  thunder_beam[0].x = x;
  thunder_beam[1].x = thunder_beam[2].x = x + ((player->isFacingRight == false) ? 18 : 0);

  thunder_beam[0].y = thunder_beam[1].y = y;
  thunder_beam[2].y = y - 55;

  thunder_beam[0].vx = 6.5f;
  if (player->isFacingRight == false)
  {
    thunder_beam[0].vx *= -1.0f;
  }
  thunder_beam[1].vy = 7.5f;
  thunder_beam[2].vy = -7.5f;

  thunder_beam[0].alive = thunder_beam[1].alive = thunder_beam[2].alive = true;

  for (int i = 0; i < 3; ++i)
  {
    thunder_beam[i].frameOffset = (i == 0) ? 5 : 0;
    thunder_beam[i].bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_ELECMAN);
    thunder_beam[i].weapon = mm_weapons::W_ELECMAN_GUN;

    thunder_beam[i].w = thunder_beam[i].bulletSpriteShet->getFrame(thunder_beam[i].frameOffset)->w;
    thunder_beam[i].h = thunder_beam[i].bulletSpriteShet->getFrame(thunder_beam[i].frameOffset)->h;

    GlobalGameState::playerShots.push_back(thunder_beam[i]);
  }
}

void mm_weapons::createThunderBeam(Character * character, int x, int y, float vx, float vy, int offset)
{
  return;
}

void mm_weapons::createRollingCutter(Player * player)
{
  mm_weapons::weapon_st cutter;

  if (player->grabstair == false)
  {
    cutter.x = (float)(player->x + 38);
    if (player->isFacingRight == false)
    {
      cutter.x -= 44.0f;
    }

    if (player->onground == true)
    {
      cutter.y = (float)(player->y + 22.0f);
    }
    else
    {
      cutter.y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    cutter.x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      cutter.x -= 48.0f;
    }

    cutter.y = (float)(player->y + 12.0f);
  }

  cutter.vx = 1.0f;
  if (player->isFacingRight == false)
  {
    cutter.vx = -1.0f;
  }

  cutter.life = 2;
  cutter.alive  = true;

  cutter.frameOffset = 3;
  cutter.bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_CUTMAN);
  cutter.weapon = mm_weapons::W_CUTMAN_GUN;

  cutter.h = cutter.bulletSpriteShet->getFrame(cutter.frameOffset)->h;
  cutter.w = cutter.bulletSpriteShet->getFrame(cutter.frameOffset)->w;

  cutter.cutter_origin_x = cutter.x;
  cutter.cutter_origin_y = cutter.y;

  cutter.ticks = Clock::clockTicks;
  cutter.frames = 4;

  cutter.cutter_target_x = cutter.vx > 0.0f ? cutter.x + 160 : cutter.x - 160; // 160 - Distance to target
  cutter.cutter_target_y = cutter.y;

  int mid_x1 = cutter.vx > 0.0f ? cutter.x + (160 / 2) : cutter.x - (160 / 2);
  int mid_y1 = cutter.y - 100; // Radius

  cutter.cutter_ctrl_points[0] = cutter.x;
  cutter.cutter_ctrl_points[1] = cutter.y;
  cutter.cutter_ctrl_points[2] = cutter.x;
  cutter.cutter_ctrl_points[3] = cutter.y;
  cutter.cutter_ctrl_points[4] = mid_x1;
  cutter.cutter_ctrl_points[5] = mid_y1;
  cutter.cutter_ctrl_points[6] = cutter.cutter_target_x;
  cutter.cutter_ctrl_points[7] = cutter.cutter_target_y;

  calc_spline(cutter.cutter_ctrl_points, CUTTER_CURVE_PNTS, cutter.cutter_curve_X, cutter.cutter_curve_Y);

  cutter.cutter_recalc = true;
  cutter.cutter_foward = true;
  cutter.current_point = 5;
  GlobalGameState::playerShots.push_back(cutter);
}

void mm_weapons::createRollingCutter(Character * character, int x, int y, float vx, float vy, int offset)
{
  return;
}

void mm_weapons::createFireStorm(Player *player)
{
  mm_weapons::weapon_st fire_storm[2];
  fire_storm[0].can_hurt = fire_storm[1].can_hurt = true;

  float x, y;

  if (player->grabstair == false)
  {
    x = (float)(player->x + 54);
    if (player->isFacingRight == false)
    {
      x -= 75.0f;
    }

    if (player->onground == true)
    {
      y = (float)(player->y + 22.0f);
    }
    else
    {
      y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      x -= 58.0f;
    }

    y = (float)(player->y + 12.0f);
  }

  fire_storm[1].subtype = true;
  fire_storm[1].life = 31;
  fire_storm[1].current_point = 1;

  fire_storm[0].frames = fire_storm[1].frames = 3;
  fire_storm[0].ticks = fire_storm[1].ticks = Clock::clockTicks;

  fire_storm[0].x = x;

  fire_storm[1].x = player->x - 20.0f;
  fire_storm[1].y = player->y + mm_player_defs::PLAYERHEIGHT;

  fire_storm[0].y = y;
  fire_storm[1].y = player->y;

  fire_storm[0].vx = 4.5f;
  if (player->isFacingRight == false)
  {
    fire_storm[0].vx *= -1.0f;
  }

  fire_storm[0].alive = fire_storm[1].alive = true;

  for (int i = 0; i < 2; ++i)
  {
    fire_storm[i].frameOffset = (i == 0) ? 3 : 0;
    fire_storm[i].frames = 3;
    fire_storm[i].bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_FIREMAN);
    fire_storm[i].weapon = mm_weapons::W_FIREMAN_GUN;

    fire_storm[i].w = fire_storm[i].bulletSpriteShet->getFrame(fire_storm[i].frameOffset)->w;
    fire_storm[i].h = fire_storm[i].bulletSpriteShet->getFrame(fire_storm[i].frameOffset)->h;

    GlobalGameState::playerShots.push_back(fire_storm[i]);
  }
}

void mm_weapons::createFireStorm(Character * character, int x, int y, float vx, float vy, int offset)
{
}

void mm_weapons::createMagnetBeam(Player *player)
{
  float x, y;

  if (player->grabstair == false)
  {
    x = (float)(player->x + 54);
    if (player->isFacingRight == false)
    {
      x -= 75.0f;
    }

    if (player->onground == true)
    {
      y = (float)(player->y + 22.0f);
    }
    else
    {
      y = (float)(player->y + 12.0f);
    }
  }
  else
  {
    x = (float)(player->x + 52.0f);
    if (player->isFacingRight == false)
    {
      x -= 58.0f;
    }

    y = (float)(player->y + 12.0f);
  }

  Character * magnet_beam = player->cur_stage->createCharacter(mm_tile_actions::MAGNET_BEAM, x, y, 0, 0, NULL);

  TemporaryCharacterList::mm_tempCharacterLst.push_back(magnet_beam);
}

void createMagnetBeam(Character * character, int x, int y, float vx, float vy, int offset)
{
}