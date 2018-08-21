/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "weapons.h"
#include "globalgamestate.h"
#include "globals.h"
#include "defines.h"
#include "spritefiles.h"
#include "stage.h"
#include "character.h"
#include "player.h"
#include "camera.h"
#include "animsequence.h"

static int bomb_fragment_frames[][14] = {{0,0,2,3,3,3,1,2,3,3,0,1,2,3},{0,0,2,1,2,3,4,3,0,1,4,2,4,3}};

static void handleBombFragment(mm_weapons::weapon_st * pWeapon)
{
  if ((Clock::clockTicks - pWeapon->ticks) > 1)
  {
    pWeapon->ticks = Clock::clockTicks;

    if (pWeapon->bomb_fragment_life > 0)
    {
      pWeapon->x += pWeapon->vx;
      pWeapon->y += pWeapon->vy;

      pWeapon->frameOffset = bomb_fragment_frames[pWeapon->subtype_num][14 - pWeapon->bomb_fragment_life];

      pWeapon->bomb_fragment_life--;
    }
    else
      pWeapon->alive = false;
  }
}

static float vx_0[] = {1.0f, -1.0f, 0.0f,  0.0f, 3.0f, 3.0f,  -3.0f, -3.0f};
static float vy_0[] = {0.0f, 0.0f,  -1.0f, 1.0f, 3.0f, -3.0f, 3.0f,  -3.0f};
static float vx_1[] = {4.0f, -4.0f, 0.0f,  0.0f};
static float vy_1[] = {0.0f, 0.0f,  -4.0f, 4.0f};
static void createExplosionParts(Stage * stage, float x, float y, bool player_created)
{
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
    explosion_part.bomb_fragment_life = 14;
    explosion_part.ticks = Clock::clockTicks;

    explosion_part.frameOffset = 0;
    explosion_part.bulletSpriteShet = stage->getAnimSeq(mm_spritefiles::EXPLOSIONLITTLE_SPRITES);
    explosion_part.weapon = mm_weapons::W_BOMBMAN_GUN;

    explosion_part.w = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->w;
    explosion_part.h = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->h;

    GlobalGameState::playerShots.push_back(explosion_part);
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
    explosion_part.bomb_fragment_life = 14;
    explosion_part.ticks = Clock::clockTicks;

    explosion_part.frameOffset = 0;
    explosion_part.bulletSpriteShet = stage->getAnimSeq(mm_spritefiles::EXPLOSIONLITTLE_SPRITES);
    explosion_part.weapon = mm_weapons::W_BOMBMAN_GUN;

    explosion_part.w = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->w;
    explosion_part.h = explosion_part.bulletSpriteShet->getFrame(explosion_part.frameOffset)->h;

    GlobalGameState::playerShots.push_back(explosion_part);
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
    if (pWeapon->hyper_bomb_should_bounce)
    {
      pWeapon->vy *= 0.6f;
      pWeapon->vx *= 0.6f;
      pWeapon->vy *= -1;
    }
    else
    {
      pWeapon->vx = pWeapon->vy = 0.0f;
    }
  }

  if (!stage->genericColVer(pWeapon->x + pWeapon->vx + (pWeapon->vx > 0 ? pWeapon->w : 0), pWeapon->y, pWeapon->h, tilecoordx, tilecoordy))
  {
    pWeapon->x += pWeapon->vx;
  }
  else
  {
    pWeapon->vx = 0.0f;
  }

  pWeapon->hyper_bomb_lifetime--;
  if (pWeapon->hyper_bomb_lifetime <= 0)
  {
    pWeapon->alive = false;
    createExplosionParts(stage, pWeapon->x, pWeapon->y, true);
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
      stage;
    }
    break;
    case mm_weapons::W_GUTSMAN_GUN:
    {
      ;
    }
    break;
    case mm_weapons::W_ICEMAN_GUN:
    {
      pWeapon->x += pWeapon->vx;
      pWeapon->y += pWeapon->vy;

      pWeapon->iceman_x_dist += pWeapon->vx;
      if (abs((int)pWeapon->iceman_x_dist) > 128)
      {
        pWeapon->iceman_x_dist = 0;
        pWeapon->frameOffset = 1 - pWeapon->frameOffset;
      }
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
      ;
    }
    break;
    case mm_weapons::W_ELECMAN_GUN:
    {
      ;
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

    pWeapon->x += pWeapon->vx;
    pWeapon->y += pWeapon->vy;

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

void mm_weapons::createBomb(Player * player)
{
  mm_weapons::weapon_st bomb;
  bomb.can_hurt = false;

  bomb.hyper_bomb_should_bounce = true;
  bomb.hyper_bomb_lifetime = 200;

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
  bomb.vy = -9.0f;

  bomb.alive  = true;

  bomb.frameOffset = 0;
  bomb.bulletSpriteShet = player->cur_stage->getAnimSeq(mm_spritefiles::WEAPONS_BOMBMAN);
  bomb.weapon = mm_weapons::W_BOMBMAN_GUN;

  bomb.w = bomb.bulletSpriteShet->getFrame(bomb.frameOffset)->w;
  bomb.h = bomb.bulletSpriteShet->getFrame(bomb.frameOffset)->h;

  GlobalGameState::playerShots.push_back(bomb);
}

void mm_weapons::createBomb(Character * character, int x, int y, float vx, float vy, int offset)
{
  return;
}
