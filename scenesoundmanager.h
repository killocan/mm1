/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <limits.h>

#include <allegro.h>
#include <string>

#include "almp3.h"

class SceneSoundManager
{
  public:
    // loop_ini and loop_end are given in ms * 44(samples).
    SceneSoundManager(const std::string & fileName, int loop_ini = 0, int track_end = INT_MAX);
    ~SceneSoundManager();

    void play(int soundId, bool loop = false);
    bool isPlaying();
    void stopAll();
    void stopSample(int soundId);
    void tickBuffer();

    DATAFILE * data;
    SAMPLE   * music;

    bool curDataOk;
    bool bIsPlaying;
};
