/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

class SceneSoundManager;

class StageEndScreen
{
  public:
    StageEndScreen();
    ~StageEndScreen();
    
    bool play(unsigned int stage_number, BITMAP * buffer);
    
    SceneSoundManager * normal_stage_victory_music;
    unsigned long delayTimer;

    enum state{PLAY_MUSIC, WAIT_MUSIC_TO_FINISH, DELAY, HIDE_ENERY_BARS};
    state cur_state;
};
