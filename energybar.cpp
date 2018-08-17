/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>

#include "energybar.h"
#include "globals.h"
#include "scenesoundmanager.h"
#include "sfx.h"

Player * EnergyBar::m_player = NULL;
int      EnergyBar::iUnits   = 0;
unsigned long EnergyBar::iTimer = 0UL;

void EnergyBar::drawEnerybar(BITMAP * buffer, int x, int y, mm_weapons::WEAPONS weapon)
{
  rectfill(buffer, x, y, x+15, y+111, 0);
  x+=2;

  int color1, color2;
  color1=color2=0;

  switch(weapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
      color1 = 0xFCE4A0;
      color2 = 0XFFFFFF;
    break;
    case mm_weapons::W_PLATFORM_CREATOR:
      color1 = 0x0080C0;
      color2 = 0x00FFFF;
    break;
    case mm_weapons::W_CUTMAN_GUN:
      color1 = 0x808080;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_GUTSMAN_GUN:
      color1 = 0xE06000;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_ICEMAN_GUN: 
      color1 = 0x0060C0;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_BOMBMAN_GUN: 
      color1 = 0x00C000;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_FIREMAN_GUN: 
      color1 = 0xE04000;
      color2 = 0xE0C000;
    break;
    case mm_weapons::W_ELECMAN_GUN:
      color1 = 0x808080;
      color2 = 0xE0E080;
    break;
    default:
    break;
  }

  int weaponEnergy = (weapon == mm_weapons::W_MEGA_BUSTER) ? m_player->life : m_player->weapons[weapon];

  int ybottom = y+112;
  for (int i = 0; i < weaponEnergy; ++i)
  {
    ybottom -= 4;
    rectfill(buffer, x,   ybottom, x+11, ybottom+1, color1);
    rectfill(buffer, x+4, ybottom, x+7,  ybottom+1, color2);
  }
}

void EnergyBar::updateEnergyBar(mm_weapons::WEAPONS weapon, int units)
{
  if (GameplayGlobals::bWeaponUpdateRunning == false)
  {
    if ( ((weapon == mm_weapons::W_MEGA_BUSTER) && (m_player->life == 28)) ||
         ((weapon != mm_weapons::W_MEGA_BUSTER) && (m_player->weapons[weapon] == 28)) )
    {
      EnergyBar::iUnits = 0;
      GameplayGlobals::bWeaponUpdateRunning = true;
      GameplayGlobals::bHoldLogic = true;
      EnergyBar::iTimer = Clock::clockTicks;
    }
    else
    {
      EnergyBar::iUnits = units;
      GameplayGlobals::bWeaponUpdateRunning = true;
      GameplayGlobals::bHoldLogic = true;
      EnergyBar::iTimer = Clock::clockTicks;
      if (units != 0)
      {
        Sounds::mm_sounds->play(ENERGY_FILL, true);
      }
    }
  }
  else
  {
    if ((Clock::clockTicks - EnergyBar::iTimer) > 3)
    {
      EnergyBar::iTimer = Clock::clockTicks;

      if (weapon == mm_weapons::W_MEGA_BUSTER)
      {
        if ((m_player->life < 28) && (EnergyBar::iUnits > 0))
        {
          ++m_player->life;
          --EnergyBar::iUnits;
        }
        else
        {
          GameplayGlobals::bWeaponUpdateRunning = false;
          GameplayGlobals::bHoldLogic = false;
          Sounds::mm_sounds->stopSample(ENERGY_FILL);
        }
      }
      else
      {
        if ((m_player->weapons[weapon] < 28) && (EnergyBar::iUnits > 0))
        {
          ++m_player->weapons[weapon];
          --EnergyBar::iUnits;
        }
        else
        {
          GameplayGlobals::bWeaponUpdateRunning = false;
          GameplayGlobals::bHoldLogic = false;
          Sounds::mm_sounds->stopSample(ENERGY_FILL);
        }
      }
    }
  }
}
