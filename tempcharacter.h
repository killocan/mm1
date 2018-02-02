/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"
#include "spritefiles.h"

class Stage;

//TempCharacter
class TempCharacter : public Character
{
  public:
    TempCharacter(const Stage & stage, mm_spritefiles::SPRT_TYPE sprtType);
    virtual void doLogic();
    bool bMortal;
    unsigned long dyingTimer;
};
