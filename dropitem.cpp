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
  int type = mm_tile_actions::BONUS_POINT_CHAR;

  int chance = rand()%1000;
  if (chance > 600)
  {
    if (chance > 970) type = mm_tile_actions::TILE_NEW_LIFE;
    else
    {
      const int possibilities[] = {mm_tile_actions::TILE_BIG_LIFE_RECHARGE,
                                   mm_tile_actions::TILE_BIG_WEAPON_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_WEAPON_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_WEAPON_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_WEAPON_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_LIFE_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_LIFE_RECHARGE,
                                   mm_tile_actions::TILE_LITTLE_LIFE_RECHARGE};

      type = possibilities[rand() % 8];
    }

    bool mortal = true;
    TemporaryCharacterList::mm_tempCharacterLst.push_back(stage->createCharacter(type, x, y, 0,0, &mortal));
  }
}
