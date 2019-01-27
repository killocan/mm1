/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#include "mm_math.h"
#include "defines.h"
#include "globals.h"
#include <allegro.h>
#include <cstdio>

void MM_Math::CalculateVector2D(int p1x, int p1y, int p2x, int p2y, double & velx, double & vely)
{
  // Calculates angle between p1 and p2
  double co      = fabs(double(p2y) - double(p1y));
  double ca      = fabs(double(p2x) - double(p1x));
  double hip     = sqrt(pow(ca,2.0) + pow(co,2.0));
  double sinang  = co/hip;
  double angreal = asin(sinang);

  // Define launch angle (plus 45 degrees)
  double angle   = (angreal/2.0) + REG_TO_RAD(45.0);

  // Delta X (take in account which one has the highest X)
  double dist    = MAX(p2x,p1x) - MIN(p2x,p1x);

  // Given the angle and distance, calculates initial velocity so 
  // p2 will be on projectile's path.
  double onecos  = 1.0 / cos(angle);  
  double y0      = (p2y - p1y);
  double v0      = onecos * sqrt( (0.5 * 1 * pow(dist,2.0)) / (dist * tan(angle) + y0) );
  double mag     = v0;

  if (!isnan(mag))
  {
    vely = mag * sin(angle);
    velx = mag * cos(angle);
  }
  else
  {
    // If something nasty happens, choose some value, just in case.
    vely = 20;
    velx = 4;
  }
}

double MM_Math::REG_TO_RAD(double n)
{
  return (n * M_PI / 180);
}
double MM_Math::RAD_TO_REG(double n)
{
  return (n * 180 / M_PI);
}

float MM_Math::DistanceToSteps(float distance, float seconds)
{
  unsigned long ticks = Clock::seconds_to_ticks(seconds);
  float steps = distance / ticks;
  return steps;
}
