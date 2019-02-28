/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageendscreen.h"

#include "tileactions.h"
#include "scenesoundmanager.h"
#include "sfx.h"
#include "soundpack.h"
#include "globals.h"
#include "globalgamestate.h"
#include "text_utils.h"
#include "stage.h"

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
StageEndScreen::StageEndScreen(Stage * stage) : normal_stage_victory_music(NULL),
                                                delayTimer(0),
                                                cur_state(StageEndScreen::PLAY_MUSIC),
                                                bDrawText1(false),
                                                bDrawText2(false),
                                                bDrawScore(false)
{
  this->stage = stage;
  normal_stage_victory_music = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::VICTORY1]);
}

StageEndScreen::~StageEndScreen()
{
  if (normal_stage_victory_music != NULL)
    delete normal_stage_victory_music;
}

void StageEndScreen::draw(BITMAP * buffer)
{
  const int WHITE = makecol(255, 255, 255);
  if (bDrawText1)
  {
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2 -20,  100, WHITE, "CLEAR");
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2 + 20, 120, WHITE, "POINTS");
  }
  if (bDrawScore)
  {
    draw_number_center(buffer, Font::mm_font, SCREEN_W / 2, 150, WHITE, 0, 6);
  }
  if (bDrawText2)
  {
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2, 180, WHITE, "-1000 x ");
    draw_number_center(buffer, Font::mm_font, SCREEN_W / 2 + 50, 180, WHITE, GlobalGameState::bonus_points, 2);
    draw_text_center_shadow(buffer, Font::mm_font, SCREEN_W / 2, 200, WHITE, "BONUS");
    draw_number_center(buffer, Font::mm_font, SCREEN_W / 2, 230, WHITE, GlobalGameState::bonus_points * 1000, 6);
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

      if (GlobalGameState::cur_boss_points > 0)
      {
        GlobalGameState::points          += 1000;
        GlobalGameState::cur_boss_points -= 1000;
      }
      else
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::DELAY2;
      }
    }
    break;
    case StageEndScreen::DELAY2:
    {
      if (Clock::clockTicks - delayTimer > 60)
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::DRAW_TEXT_2;
      }
    }
    break;
    case StageEndScreen::DRAW_TEXT_2:
    {
      bDrawText2 = true;
      TemporaryCharacterList::mm_tempCharacterLst.push_back(stage->createCharacter(mm_tile_actions::BONUS_POINT_CHAR,
                                                                                   SCREEN_W/2 - 100, 172));
      cur_state = StageEndScreen::UPDATE_SCORE_BONUS;
    }
    break;
    case StageEndScreen::UPDATE_SCORE_BONUS:
    {
      if (GlobalGameState::bonus_points > 0)
      {
        GlobalGameState::points += GlobalGameState::bonus_points * 1000;
        GlobalGameState::bonus_points = 0;
      }
      else
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::DELAY3;
      }
    }
    break;
    case StageEndScreen::DELAY3:
    {
      if (Clock::clockTicks - delayTimer > 100)
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
