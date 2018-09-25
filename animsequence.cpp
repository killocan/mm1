/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <stdio.h>
#include <string>

#include "animsequence.h"

AnimSequence::AnimSequence(const char * seqFile, bool keepCopy) 
  : m_bKeepCopy(keepCopy)
{
  loadSequences(seqFile);
}

void AnimSequence::loadSequences(const char * def_file)
{
  char temp_buf[256];

  FILE * fp = fopen(def_file, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "ERRO LOADING Char Def File: [%s]\n", def_file);
    exit(-1);
  }
  if (fgets(temp_buf, sizeof(temp_buf)*sizeof(char), fp) == NULL)
  {
    fprintf(stderr, "ERRO Char Def File: [%s] format error\n", def_file);
    exit(-1); 
  }

  std::string sprite_shet_file = std::string(temp_buf);
  std::size_t pos = sprite_shet_file.find_first_of("\r\n");
  sprite_shet_file.insert(pos, 1, '\0');
  // Load sprite file.
  //spriteSheet = new AnimSequence(sprite_shet_file.c_str());
  //utilX    = spriteSheet->getUtilXInfo()[0];
  //utilXLen = spriteSheet->getUtilXInfo()[1];
  loadSprites(sprite_shet_file.c_str());

  //Now that i've done it, i guess 'twould be better to use a single loop
  //and use std::find to load it all, but i'm fucking lazy and refuse to redo 
  //the code, maybe one day.
  std::vector<std::string> tmp_pairs;
  while(fgets(temp_buf, sizeof(temp_buf)*sizeof(char), fp) != NULL)
  {
    tmp_pairs.clear();

    char * pair = strtok(temp_buf, "|");
    if (pair != NULL)
    {
      tmp_pairs.push_back(pair);
      while(pair != NULL)
      {
        pair = strtok(NULL, "|");
        if (pair != NULL)
          tmp_pairs.push_back(pair);
      }

      std::vector<AnimSequence::FrameInfoSt> tmp_frameInfo;
      for (std::vector<std::string>::iterator i = tmp_pairs.begin(); i != tmp_pairs.end(); i++)
      {
        std::string frame    = i->substr(0, i->find_first_of(','));
        std::string duration = i->substr(i->find_first_of(',')+1);

        AnimSequence::FrameInfoSt frameInfo;
        frameInfo.frameNumber   = atoi(frame.c_str());
        frameInfo.frameDuration = atoi(duration.c_str());

        tmp_frameInfo.push_back(frameInfo);
      }

      if (tmp_frameInfo.size() > 0)
      {
        anim_seqs.push_back(tmp_frameInfo);
      }
    }
    else
      break;
  }
  fclose(fp);

  if (anim_seqs.size() == 0)
  {
    fprintf(stderr, "ERRO Char Def File: [%s] format error (no animation desc)\n", def_file);
    exit(-1); 
  }

  //curAnimFrameDuration = getCurrFrameDuration();
}

void AnimSequence::loadSprites(const char * seqFile)
{
  static int RED   = makecol32(255,0,0);
  static int GREEN = makecol32(0,255,0);

  std::vector<int> framesLen;

  seqBmp = load_bitmap(seqFile, NULL);
  if (seqBmp == NULL)
  {
    fprintf(stderr, "[%s][%d] Could not load spriteseq[%s] \n", __FILE__, __LINE__, seqFile);
    exit(-1);
  }

  if (m_bKeepCopy == true)
  {
    seqBmpCopy = create_bitmap(seqBmp->w, seqBmp->h);
    blit(seqBmp, seqBmpCopy, 0,0,0,0, seqBmp->w, seqBmp->h);
  }

  int spriteUtilAreaPoints = 0;
  int frame_len = 0;
  int pixel     = 0;
  bool heightFound = false;
  for (int i = 0; i < seqBmp->w; i++)
  {
    pixel = _getpixel32(seqBmp, i, 0);
    if (pixel == 0)
    {
      heightFound = false;

      framesLen.push_back(frame_len);
      frame_len = 0;

      for (int y = 1; y < seqBmp->h; y++)
      {
        pixel = _getpixel32(seqBmp, i, y);
        if (pixel == GREEN)
        {
          heightFound = true;
          frameHeight.push_back((seqBmp->h - y));
          break;
        }
      }

      if (heightFound == false)
      {
        frameHeight.push_back(seqBmp->h);
      }
    }
    else
    {
      if ((pixel == RED) && (spriteUtilAreaPoints < 2))
      {
        utilXSize[spriteUtilAreaPoints] = i+1;
        spriteUtilAreaPoints++;
      }
      frame_len++;
    }
  }

  if (spriteUtilAreaPoints != 2)
  {
    fprintf(stderr,"Error loading anim sequence[%s]. missing control points\n", seqFile);
    exit(-1);
  }

  utilXSize[1]=utilXSize[1]-utilXSize[0];
  //fprintf(stderr,"anim sequence[%s]\nInitX[%d] LenX[%d]\n", seqFile, utilXSize[0], utilXSize[1]);

  //int x = 0;
  for (int i = 0, x = 0; i < framesLen.size(); i++)
  {
    seqFrames.push_back(create_sub_bitmap(seqBmp, x, 1, framesLen[i], seqBmp->h));
    x += framesLen[i] + 1;
  }
}

int AnimSequence::getFrameHeight(int frameNumber)
{
  return frameHeight[frameNumber];
}

AnimSequence::~AnimSequence()
{
  unload();
}

int * AnimSequence::getUtilXInfo()
{ 
  return utilXSize;
}

void AnimSequence::unload()
{
  for (int i = 0; i < seqFrames.size(); i++)
  {
    destroy_bitmap(seqFrames[i]);
  }

  destroy_bitmap(seqBmp);
  if (m_bKeepCopy == true)
  {
    destroy_bitmap(seqBmpCopy);
  }

  seqFrames.clear();
}

BITMAP * AnimSequence::getFrame(int frame_idx)
{
  if (frame_idx >= seqFrames.size())
    return NULL;

  return seqFrames[frame_idx];
}

std::vector<std::vector<AnimSequence::FrameInfoSt> > AnimSequence::getAnimSeq()
{
  return anim_seqs;
}

void AnimSequence::changeSprite(mm_weapons::WEAPONS weapon)
{
  int x,y;
  int color1, color2;
  bool default_weapom = false;

  color1=color2=0;

  //BGR
  switch(weapon)
  {
    case mm_weapons::W_MEGA_BUSTER:
    case mm_weapons::W_PLATFORM_CREATOR:
      default_weapom = true;
    break;
    case mm_weapons::W_CUTMAN_GUN:
      color1 = 0x808080;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_GUTSMAN_GUN:
      color1 = 0xE06000;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_ICEMAN_GUN: 
      color1 = 0x0060C0;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_BOMBMAN_GUN: 
      color1 = 0x00C000;
      color2 = 0xFFFFFF;
    break;
    case mm_weapons::W_FIREMAN_GUN: 
      color1 = 0xE04000;
      color2 = 0xE0C000;
    break;
    case mm_weapons::W_ELECMAN_GUN:
      color1 = 0x808080;
      color2 = 0xE0E080;
    break;
    default:
    break;
  }

  if (default_weapom == false)
  {
    int color = 0;
    for (y = 0; y < seqBmpCopy->h; y++)
    {
      for (x = 0; x < seqBmpCopy->w; x++)
      {
        color = _getpixel32(seqBmpCopy, x,y);
        
        switch(color)
        {
          case 0x0080c0:
            color = color1;
          break;
          case 0x00ffff:
            color = color2;
          break;
        }

        _putpixel32(seqBmp, x, y, color);
      }
    }
  }
  else
  {
    blit(seqBmpCopy, seqBmp, 0,0,0,0, seqBmpCopy->w, seqBmpCopy->h);
  }
}

