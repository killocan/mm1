/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include <allegro.h>

class Collision
{
  public:
    static bool pixelCollision(int object1X, int object1Y, BITMAP *sprite1, 
                               int object2X, int object2Y, BITMAP *sprite2,
                               int * colX = NULL, int * colY = NULL);
};
