/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageendscreen.h"

#include "scenesoundmanager.h"
#include "sfx.h"
#include "soundpack.h"
#include "globals.h"
#include "globalgamestate.h"
#include "text_utils.h"

/*
STAGE CLEAR
; What happens in regular stages.
;1. Play item sound                                                  
;2. Play stage clear music                                           
;3. Load levelend gfx
;4. Remove all objects except Megaman
;5. Delay 383 frames
;6. Hide energy bars
;7. Draw first patch of text
;8. Halt Megaman?
;9. Calculate score, (boss points values). Add 1000 points per frame
;10. Delay 64 frames
;11. Draw second patch of text
;12. Calculate bonus pearls, one per frame * 1000.
;13. Delay 383 frames
;14. Reboot game without clearing game status
*/
StageEndScreen::StageEndScreen() : normal_stage_victory_music(NULL), cur_state(StageEndScreen::PLAY_MUSIC), delayTimer(0),
bDrawText1(false), bDrawScore(false)
{
  normal_stage_victory_music = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::VICTORY1]);
}

StageEndScreen::~StageEndScreen()
{
  if (normal_stage_victory_music != NULL)
    delete normal_stage_victory_music;
}

void StageEndScreen::draw(BITMAP * buffer)
{
  if (bDrawText1)
  {
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2 -20, 100, makecol(255, 255, 255), "CLEAR");
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2 + 20, 120, makecol(255, 255, 255), "POINTS");
  }
  if (bDrawScore)
  {
    draw_number_center(buffer, Font::mm_font, SCREEN_W / 2, 150, makecol(255, 255, 255), 0, 6);
  }
}

bool StageEndScreen::play(unsigned int stage_number)
{
  switch (cur_state)
  {
    case StageEndScreen::PLAY_MUSIC:
    {
      normal_stage_victory_music->play(0, false);
      cur_state = StageEndScreen::WAIT_MUSIC_TO_FINISH;
    }
    break;
    case StageEndScreen::WAIT_MUSIC_TO_FINISH:
    {
      if (normal_stage_victory_music->isPlaying() == false)
      {
        cur_state = StageEndScreen::DELAY1;
        delayTimer = Clock::clockTicks;
      }
    }
    break;
    case StageEndScreen::DELAY1:
    {
      if (Clock::clockTicks - delayTimer > 100)
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::HIDE_ENERY_BARS;
      }
    }
    break;
    case StageEndScreen::HIDE_ENERY_BARS:
    {
      GameplayGlobals::bDontDrawBars = true;
      cur_state = StageEndScreen::DRAW_TEXT_1;
    }
    break;
    case StageEndScreen::DRAW_TEXT_1:
    {
      bDrawText1 = true;
      cur_state = StageEndScreen::UPDATE_SCORE;
    }
    case StageEndScreen::UPDATE_SCORE:
    {
      bDrawScore = true;
      delayTimer = Clock::clockTicks;
      cur_state = StageEndScreen::DELAY2;
    }
    break;
    case StageEndScreen::DELAY2:
    {
      if (Clock::clockTicks - delayTimer > 60)
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::END;
      }
    }
    break;
    case StageEndScreen::END:
    {
      delete normal_stage_victory_music;
      normal_stage_victory_music = NULL;

      if (stage_number < 6)
        GlobalGameState::enemyDefeated[stage_number] = true;

      return true;
    }
    break;
  }

  return false;
}
