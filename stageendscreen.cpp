/*
 * Killocan 2019
 * http://killocan.blogspot.com
*/

#include "stageendscreen.h"

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
;8. Halt Megaman
;9. Calculate score, 1000 points per frame
;10. Delay 64 frames
;11. Draw second patch of text
;12. Calculate bonus pearls, one per frame (3000 points each)
;13. Delay 383 frames
;14. Reboot game without clearing game status
*/
void StageEndScreen::play(unsigned int stage_number, BITMAP * buffer)
{
}
