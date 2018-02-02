/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

#include <list>

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
    // Gets incremented 60times a sec.
    static unsigned long clockTicks;
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

class GameplayGlobals
{
  public:
    static bool bHoldLogic;
    static bool bWeaponUpdateRunning;
};
