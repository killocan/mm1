/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>
#include <allegro.h>

#include "weapons.h"

class Player;

class AnimSequence
{
  friend class Player;

  public:
    struct FrameInfoSt
    {
      int frameNumber;
      int frameDuration;
    };

    AnimSequence(const char * seqFile, bool keepCopy = false);
    ~AnimSequence();

    BITMAP * getFrame(unsigned int frame_idx);
    int * getUtilXInfo();
    int getFrameHeight(int frameNumber);

    std::vector<std::vector<FrameInfoSt> > getAnimSeq();

  private:
    void changeSprite(mm_weapons::WEAPONS weapon);

    void loadSprites(const char * seqFile);
    void loadSequences(const char * seqFile);
    void unload();

    BITMAP * seqBmp;
    BITMAP * seqBmpCopy;
    std::vector<BITMAP *> seqFrames;
    std::vector<int>      frameHeight;
    int utilXSize[2];//X,Len

    bool m_bKeepCopy;

    std::vector<std::vector<FrameInfoSt> > anim_seqs;
};
