/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#include "tempcharacter.h"
#include "globals.h"

TempCharacter::TempCharacter(const Stage & stage, mm_spritefiles::SPRT_TYPE sprtType)
  : Character(stage, sprtType)
{
  dyingTimer = Clock::clockTicks;
  bMortal = false;
}

void TempCharacter::doLogic()
{
  if (bMortal == true)
  {
    if ((Clock::clockTicks - dyingTimer) > 130)
    {
      alive = false;
    }
  }
}
