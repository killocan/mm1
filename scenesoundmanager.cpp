/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include "scenesoundmanager.h"
#include <stdio.h>

SceneSoundManager::SceneSoundManager(const std::string & fileName, int loop_ini, int track_end)
  : data(NULL), music(NULL), curDataOk(false), bIsPlaying(false)
{
  if (fileName.find(".dat") != std::string::npos)
  {
    data = load_datafile(fileName.c_str());
  }
  else
  {
    int data_len = 0;

    PACKFILE * mp3_file = pack_fopen(fileName.c_str(), F_READ);
    if (mp3_file != NULL)
    {
      ALMP3_MP3 * mp3_handle = NULL;
      unsigned char * mp3_data = NULL;

      data_len = file_size_ex(fileName.c_str());
      mp3_data = (unsigned char *) malloc(sizeof(unsigned char) * data_len);

      if ((data_len = pack_fread(mp3_data, data_len, mp3_file)) > 0)
      {
        mp3_handle = almp3_create_mp3(mp3_data, data_len);
      }

      music = almp3_create_sample_from_mp3(mp3_handle);
      if (music != NULL)
      {
        // +/- 44 per millisecond
        music->loop_start = loop_ini;
        if (track_end >= 0)
          music->loop_end   = track_end;
      }

      almp3_destroy_mp3(mp3_handle);
      free(mp3_data);

      pack_fclose(mp3_file);
    }
  }

  if (data != NULL || music != NULL)
  {
    curDataOk = true;
  }
  else
  {
    curDataOk = false;
  }
}

SceneSoundManager::~SceneSoundManager()
{
  stopAll();

  if (data != NULL)
  {
    unload_datafile(data);
  }
  else if (music != NULL)
  {
    destroy_sample(music);
  }
}

void SceneSoundManager::play(int soundId, bool loop)
{
  // VOLUME 0..245. Since sfx are played a little bit louder
  static const int VOLUME = 50;
  if (curDataOk == true)
  {
    if (data != NULL)
    {
      switch(data[soundId].type)
      {
        case DAT_SAMPLE:
        {
          SAMPLE * sample = static_cast<SAMPLE *>(data[soundId].dat);
          play_sample(sample, VOLUME+10, 128, 1000, (int)loop);
        }
        break;
        case DAT_MIDI:
        {
          MIDI * midi = static_cast<MIDI *>(data[soundId].dat);
          play_midi(midi, loop);
        }
        break;
      }
    }
    else if (music != NULL && soundId == 0)
    {
      play_sample(music, VOLUME, 128, 1000, (int)loop);
    }

    bIsPlaying = true;
  }
}

bool SceneSoundManager::isPlaying()
{
  //if (mp3_data)
  //{
  //  return (almp3_is_playing_mp3(mp3_handle) == TRUE);
  //}

  return bIsPlaying;
}

void SceneSoundManager::stopSample(int soundId)
{
  if (curDataOk == true)
  {
    bIsPlaying = false;
    if (data != NULL)
    {
      SAMPLE * sample = static_cast<SAMPLE *>(data[soundId].dat);
      stop_sample(sample);
    }
  }
}

void SceneSoundManager::tickBuffer()
{
  ;
}

void SceneSoundManager::stopAll()
{
  bIsPlaying = false;

  stop_midi();
  if (music != NULL)
  {
    stop_sample(music);
  }
}
