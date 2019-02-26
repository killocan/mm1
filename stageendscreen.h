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
    
    void draw(BITMAP * buffer);
    bool play(unsigned int stage_number);
    
    SceneSoundManager * normal_stage_victory_music;
    unsigned long delayTimer;

    enum state{PLAY_MUSIC, WAIT_MUSIC_TO_FINISH, DELAY1, HIDE_ENERY_BARS, DRAW_TEXT_1, UPDATE_SCORE, DELAY2, DRAW_TEXT_2, UPDATE_SCORE_BONUS, DELAY3, END};
    state cur_state;

    bool bDrawText1;
    bool bDrawText2;
    bool bDrawScore;
};
