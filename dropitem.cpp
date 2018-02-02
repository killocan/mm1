/*
 * Killocan 2015
 * http://killocan.blogspot.com
*/

#include "dropitem.h"
#include "stage.h"
#include "tileactions.h"
#include "globals.h"

void DropItem::dropItem(Stage * stage, int x, int y)
{
  TemporaryCharacterList::mm_tempCharacterLst.push_back(stage->createCharacter(mm_tile_actions::BONUS_POINT_CHAR, 
                                                                               x, y));
}
