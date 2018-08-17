/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

namespace mm_graphs_defs
{
  const int  TILE_SIZE = 32;
  const int  UTIL_W    = 512;
  const int  UTIL_H    = 480;
  const int  TILES_X   = UTIL_W/TILE_SIZE;
  const int  TILES_Y   = UTIL_H/TILE_SIZE;
}

namespace mm_player_defs
{
  const int PLAYERHEIGHT = 64;
  const int PLAYERWIDTH  = 42;
  const int HALFPLAYERWIDTH = PLAYERWIDTH>>1;
  const int GRAVITATION  = 1;
  const int VELMOVING    = 3;
  const int VELJUMP      = 15;
}
