/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <allegro.h>

#include "energybar.h"
#include "globals.h"
#include "scenesoundmanager.h"
#include "sfx.h"
#include "spritefiles.h"

Player * EnergyBar::m_player = NULL;
Character * EnergyBar::m_boss = NULL;
int      EnergyBar::iUnits   = 0;
unsigned long EnergyBar::iTimer = 0UL;

void EnergyBar::drawEnerybar(BITMAP * buffer, int x, int y, mm_weapons::WEAPONS weapon, bool boss)
{
  rectfill(buffer, x, y, x+15, y+111, 0);
  x+=2;

  int color1, color2;
  color1=color2=0;
  static int mm_weapon_color1[] = {0xFCE4A0, 0x0080C0, 0x808080, 0xE06000, 0x0060C0, 0x00C000, 0xE04000, 0x808080};
  static int mm_weapon_color2[] = {0XFFFFFF, 0x00FFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xE0C000, 0xE0E080};
  static int boss_weapon_color1[] = { 0XE40058, 0XE40058, 0X3CBCFC, 0XE40058, 0XE40058, 0XE40058, 0XE40058, 0XE40058, 0XE40058, 0XE40058 };
  static int boss_weapon_color2[] = { 0XFC9838, 0XFCFCFC, 0XFCFCFC, 0XFC9838, 0XFCFCFC, 0XFC9838, 0XFC9838, 0XFC9838, 0XFC9838, 0XFC9838 };

  int index = m_boss != NULL ? m_boss->type - mm_spritefiles::GUTSMAN_SPRITES : -1;
  color1 = (boss == false) ? mm_weapon_color1[weapon] : boss_weapon_color1[index];
  color2 = (boss == false) ? mm_weapon_color2[weapon] : boss_weapon_color2[index];
  
  int weaponEnergy = 
    (boss == false) 
    ? ((weapon == mm_weapons::W_MEGA_BUSTER) ? m_player->life : m_player->weapons[weapon])
    : m_boss->life;

  int ybottom = y+112;
  for (int i = 0; i < weaponEnergy; ++i)
  {
    ybottom -= 4;
    rectfill(buffer, x,   ybottom, x+11, ybottom+1, color1);
    rectfill(buffer, x+4, ybottom, x+7,  ybottom+1, color2);
  }
}

void EnergyBar::updateEnergyBar(mm_weapons::WEAPONS weapon, int units, bool boss)
{
  if (GameplayGlobals::bWeaponUpdateRunning == false)
  {
    if ( (!boss && (((weapon == mm_weapons::W_MEGA_BUSTER) && (m_player->life == 28)) ||
         ((weapon != mm_weapons::W_MEGA_BUSTER) && (m_player->weapons[weapon] == 28)))) ||
         (boss && m_boss->life == 28))
    {
      EnergyBar::iUnits = 0;
    }
    else
    {
      EnergyBar::iUnits = units;
      if (units != 0)
      {
        Sounds::mm_sounds->play(ENERGY_FILL, true);
      }
    }

    GameplayGlobals::bWeaponUpdateRunning = true;
    if (!boss) GameplayGlobals::bHoldLogic = true;
    EnergyBar::iTimer = Clock::clockTicks;
  }
  else
  {
    if ((Clock::clockTicks - EnergyBar::iTimer) > 2)
    {
      EnergyBar::iTimer = Clock::clockTicks;

      if (boss == false)
      {
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
      else
      {
        if ((m_boss->life < 28) && (EnergyBar::iUnits > 0))
        {
          ++m_boss->life;
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
