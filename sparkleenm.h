/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

#include "character.h"

class Stage;

//SparkleEnm
class SparkleEnm : public Character
{
  public:
    SparkleEnm(const Stage & stage, int x, int y);

    virtual void doLogic();

    // Do nothing
    virtual void doGravitation();

    enum {ACTIVE};

  private:
    //Stage * cur_stage; // current stage.
};
