/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//CrazyRazy
class CrazyRazy : public Character
{
  public:
    class CrazyRazyFragmentUpperHalf : public Character
    {
      public:
        CrazyRazyFragmentUpperHalf(const Stage & stage, int x, int y);

        virtual void hit(mm_weapons::weapon_st * pWeapon);
        virtual void doGravitation();
        virtual void doLogic();

        enum {MOVING=3, CALC_ATTACK, ATTACK};

        int CTRL_POINTS[8];
        static const int CURVE_PNTS = 50;
        int curveX[CrazyRazyFragmentUpperHalf::CURVE_PNTS];
        int curveY[CrazyRazyFragmentUpperHalf::CURVE_PNTS];
        int curPoint;
        bool m_bAnimation;
    };

    class CrazyRazyFragmentLowerHalf : public Character
    {
      public:
        CrazyRazyFragmentLowerHalf(const Stage & stage, int x, int y);

        virtual void doLogic();
        virtual void hit(mm_weapons::weapon_st * pWeapon);

        enum {MOVING=6};
    };

    CrazyRazy(const Stage & stage, int x, int y);

    virtual void fire();
    virtual void hit(mm_weapons::weapon_st * pWeapon);
    virtual void die();
    virtual void doLogic();
    virtual void respawn();

    bool bDieUpper;

    enum {MOVING};
};
