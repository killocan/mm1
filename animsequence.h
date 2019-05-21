/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>
#include <allegro.h>

#include "weapons.h"

class Player;
class MegamanExplosion;

class AnimSequence
{
  friend class Player;
  friend class MegamanExplosion;

  public:
    struct FrameInfoSt
    {
      int frameNumber;
      int frameDuration;
	  bool frameLoop;
    };

    AnimSequence(const char * seqFile, bool keepCopy = false);
    ~AnimSequence();

    BITMAP * getFrame(int frame_idx);
    unsigned frameNumber();
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

