/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//Blader
class Blader : public Character
{
  public:
    Blader(const Stage & stage, int x, int y, void * pSpecialCase);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void respawn();

    virtual void checkOnCamera();

    // Only seeking corresponds to an animation, diving and calc_diving are just states.
    enum {SEEKING, DIVING, CALC_DIVING, STAND_STILL};

  private:
    int CTRL_POINTS[8];

    static const int CURVE_PNTS = 40;
    int curveX[Blader::CURVE_PNTS];
    int curveY[Blader::CURVE_PNTS];
    int curPoint;

    bool bSpecialCase;
    bool bDontRespawn;

    //int curState;
    //int colorOffset; // which sprite color to use.
    //Stage * cur_stage; // current stage.
};
