/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <string>
#include <vector>
//#include <list>

#include "scenesoundmanager.h"
#include "bossdoor.h"

class Stage;
class Camera;
class Character;
class Player;
class WeaponMenu;

class StageManager
{
  public:
    StageManager();
    virtual ~StageManager();

    virtual void setupStage();
    virtual void doStageSpecifics();
    virtual void createBoss();
    void play();

    virtual const std::string & getStageFilePath() const;

  protected:
    virtual int bossWarning();
    virtual SceneSoundManager * CreateSoundManager();
    SceneSoundManager * ssm;
    SceneSoundManager * sboos;

    std::string STAGE_PATH;
    std::vector<Character *> characters_vec;
    std::vector<Character *> special_chars_vec; // just doors by now.
//    std::list<Character *>   temp_characters_lst;

    Stage  * stage;
    Camera * camera;

    // tiles to switch during boss warning. Used in bossWarning.
    int bgTileWarning0, bgTileWarning1;

    enum STAGE_STATES {SEARCHING_BOSS_DOOR, HANDLING_DOOR, BOSS_WARNING, INITING_BOSS_FIGHT, FIGHT};
    STAGE_STATES cur_stage_state;

  private:
    void doMegamanSpawning(BITMAP * buffer, FONT * mm_font);
    void handlePlayer();
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
};
