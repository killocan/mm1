/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <string>
#include <vector>
#include <map>

#include "scenesoundmanager.h"
#include "bossdoor.h"

class Stage;
class Camera;
class Character;
class Player;
class WeaponMenu;
class StageEndScreen;

class StageManager
{
  public:
    StageManager();
    virtual ~StageManager();

    // Hardstart means load everything, when false just load create characters again.
    virtual void setupStage(bool hardStart);
    virtual void doStageSpecifics();
    virtual void createBoss();
    void play();
    void drawCharacters();
    bool findMagneticbeam();
    virtual const std::string & getStageFilePath() const;

  protected:
    virtual int bossWarning();
    virtual SceneSoundManager * CreateSoundManager();
    SceneSoundManager * ssm;
    SceneSoundManager * sboos;
    StageEndScreen * stageEndScreen;

    std::string STAGE_PATH;
    std::vector<Character *> characters_vec;
    std::vector<Character *> special_chars_vec; // just doors by now.
//    std::list<Character *>   temp_characters_lst;

    Stage  * stage;
    Camera * camera;

    // tiles to switch during boss warning. Used in bossWarning.
    //int bgTileWarning0, bgTileWarning1;
    std::vector<int> bossWarningTiles;
    std::map<unsigned, unsigned> bossWarningColors;

    enum STAGE_STATES {SEARCHING_BOSS_DOOR, HANDLING_DOOR, BOSS_WARNING, INITING_BOSS_FIGHT, FIGHT, BOSS_DEAD, END_STAGE};
    STAGE_STATES cur_stage_state;
    unsigned int stage_number;
    bool game_over;
    bool playing;

  private:
    void doMegamanSpawning(BITMAP * buffer, FONT * mm_font);
    //void handlePlayer();
    //void doDraw();
    void checkColissionAgainstEnemy();
    void checkColissionPlayerEnemyBullets();
    // Kill all alive chars from characters_vec. (don't mess with temp_list nor player)
    void killThemAll();

    bool game_pause;
    bool weapon_menu;

    BITMAP * m_buffer;

    Player * player;

    WeaponMenu * weaponMenu;

    bool handlingDoor;
    bool stopAnimations;
    bool initBossFight;
    unsigned long doorTiming;

    unsigned long dyingTime;
    bool dyingSequece;
};
