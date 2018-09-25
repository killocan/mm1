/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include "collision.h"

bool Collision::pixelCollision(int object1X, int object1Y, BITMAP *sprite1, int object2X, int object2Y, BITMAP *sprite2,
                               int * colX, int * colY)
{
  int xmax1 = object1X + sprite1->w, ymax1 = object1Y + sprite1->h;
  int xmax2 = object2X + sprite2->w, ymax2 = object2Y + sprite2->h;

  // find the intersection
  int xmin = MAX(object1X, object2X);
  int ymin = MAX(object1Y, object2Y);
  int xmax = MIN(xmax1, xmax2);
  int ymax = MIN(ymax1, ymax2);

  // if they dont intersect, no collision.
  if (xmax <= xmin || ymax <= ymin) 
  {
    return false;
  }

  int mask = MASK_COLOR_32;

  int x1,x2,y1,y2;
  int color1, color2;
  for (int y = ymin; y < ymax; ++y) 
  {
    // normalize the coordinates.
    y1 = y - object1Y;
    y2 = y - object2Y;

    for (int x = xmin; x < xmax; ++x) 
    {
      // normalize the coordinates.
      x1 = x - object1X;
      x2 = x - object2X;

      color1 = _getpixel32(sprite1, x1, y1);
      color2 = _getpixel32(sprite2, x2, y2);

      //if (!((color1 & color2) ^ mask))
      //TODO: testar (color1&color2)-mask != 0
      if (color1 != mask && color2 != mask)
      {
        if (colX != NULL)
        {
          *colX = x;
          *colY = y;
        }
        return true;
      }
    }
  }

  return false;
}
