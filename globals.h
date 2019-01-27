/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

#include <list>
#include <vector>

class SceneSoundManager;
class Camera;
class Character;

class Font
{
  public:
    static FONT * mm_font;
};

class Buffer
{
  public:
    static BITMAP * buffer;
};

class Sounds
{
  public:
    static SceneSoundManager * mm_sounds;
};

class Vsync
{
  public:
    static void (*Sync)();
};

class Clock
{
  public:
    inline static unsigned long seconds_to_ticks(float seconds)
    {
      return seconds * ticks;
    }
    inline static float ticks_to_seconds(unsigned long tick)
    {
      return tick / ticks;
    }

    // Gets incremented Ticks times a sec.
    static unsigned long clockTicks;
    static const unsigned long ticks = 60L;
};

class GlobalCamera
{
  public:
    static Camera * mm_camera;
};

class TemporaryCharacterList
{
  public:
    static std::list<Character *> mm_tempCharacterLst;
};

class CurrentCharacterList
{
  public:
    static std::vector<Character *> * mm_characterLst;
};

class GameplayGlobals
{
  public:
    static bool bHoldLogic;
    static bool bWeaponUpdateRunning;
};
