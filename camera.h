/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

class Camera
{
  public:
    Camera(): x(0),y(0),old_x(0),w(0),h(0) {}
    int x, y;
    int old_x;
    int w, h;
};
