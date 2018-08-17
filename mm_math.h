/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

class MM_Math
{
  public:
    static void CalculateVector2D(int p1x, int p1y, int p2x, int p2y, double & velx, double & vely);
    static double REG_TO_RAD(double n);
    static double RAD_TO_REG(double n);
};
