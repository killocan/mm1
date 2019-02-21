/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

class MM_Math
{
  public:
    struct pt { int x; int y; };

    // picketman hammer vector calculation
    static void CalculateVector2D(int p1x, int p1y, int p2x, int p2y, double & velx, double & vely);
    // Radian to Degress and back
    static double REG_TO_RAD(double n);
    static double RAD_TO_REG(double n);
    // Returns how many steps per tick to take to smoothly travel distance in seconds taking the clock tick
    static float DistanceToSteps(float distance, float seconds);
    // Return time to reach floor when dropped from some height
    static float dropTime(int height);
};
