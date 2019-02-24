/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageendscreen.h"

#include "scenesoundmanager.h"
#include "sfx.h"
#include "soundpack.h"
#include "globals.h"

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
;9. Calculate score, 1000 points per frame
;10. Delay 64 frames
;11. Draw second patch of text
;12. Calculate bonus pearls, one per frame (3000 points each)
;13. Delay 383 frames
;14. Reboot game without clearing game status
*/
StageEndScreen::StageEndScreen() : normal_stage_victory_music(NULL), cur_state(StageEndScreen::PLAY_MUSIC), delayTimer(0)
{
  normal_stage_victory_music = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::VICTORY1]);
}

StageEndScreen::~StageEndScreen()
{
  if (normal_stage_victory_music != NULL)
    delete normal_stage_victory_music;
}

// return true if done.
bool StageEndScreen::play(unsigned int stage_number, BITMAP * buffer)
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
        cur_state = StageEndScreen::DELAY;
        delayTimer = Clock::clockTicks;
      }
    }
    break;
    case StageEndScreen::DELAY:
    {
      if (Clock::clockTicks - delayTimer > 383)
      {
        delayTimer = Clock::clockTicks;
        cur_state = StageEndScreen::HIDE_ENERY_BARS;
      }
    }
    break;
    case StageEndScreen::HIDE_ENERY_BARS:
    {
      delete normal_stage_victory_music;
      normal_stage_victory_music = NULL;

      return true;
    }
    break;
  }

  return false;
}
