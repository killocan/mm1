#include <vector>

#include <cstdio>
#include <string>

#include <allegro.h>

class AnimSequence
{
  public:
    struct FrameInfoSt
    {
      int frameNumber;
      int frameDuration;
    };

    AnimSequence(const char * seqFile, bool keepCopy = false);
    ~AnimSequence();

    BITMAP * getFrame(int frame_idx);
    int * getUtilXInfo();
    int getFrameHeight(int frameNumber);

    std::vector<std::vector<FrameInfoSt> > getAnimSeq();

  private:
    void loadSprites(const char * seqFile);
    void loadSequences(const char * seqFile);
    void unload();

    BITMAP * seqBmp;
    std::vector<BITMAP *> seqFrames;
    std::vector<int>      frameHeight;
    int utilXSize[2];

    std::vector<std::vector<FrameInfoSt> > anim_seqs;
};

AnimSequence::AnimSequence(const char * seqFile, bool keepCopy) 
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
  loadSprites(sprite_shet_file.c_str());

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

static volatile int game_logic_tick = 0;
void update_game_logic_tick()
{
  game_logic_tick = 1;
}

int rotations[] = {0, 64, 128, 192};
int cur_rotation = 0;
bool rotation_mode = false;
int main(int argc, char ** argv)
{
	BITMAP * buffer;
	char anim_file[80];
	bool invertDraw = false;

	if (argc < 2)
	{
		fprintf(stderr,"test_animation <animation_file>\n");
		exit(-1);
	}

	strcpy(anim_file, argv[1]);

	allegro_init();
	install_keyboard();
	install_timer();

	install_int_ex(update_game_logic_tick, BPS_TO_TIMER(60));	

	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);

	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	clear_bitmap(buffer);

	AnimSequence * seq = new AnimSequence(anim_file);
	int w,h;
	w = seq->getFrame(0)->w;
	h = seq->getFrame(0)->h;
	
	int num_seqs = seq->getAnimSeq().size();
	int cur_seq  = 0;
	int seq_frames    = seq->getAnimSeq()[cur_seq].size();
	int cur_frame_num = 0;
	int cur_frame          = seq->getAnimSeq()[cur_seq][cur_frame_num].frameNumber;
	int cur_frame_duration = seq->getAnimSeq()[cur_seq][cur_frame_num].frameDuration;

	while(!key[KEY_ESC])
	{
		if (game_logic_tick)
		{
			game_logic_tick = 0;

			if (cur_frame_duration-- <= 0)
			{
				++cur_frame_num;
				if (cur_frame_num >= seq_frames) cur_frame_num = 0;
				
				cur_frame_duration = seq->getAnimSeq()[cur_seq][cur_frame_num].frameDuration;
			}
		}

		if (key[KEY_SPACE])
		{
			while(key[KEY_SPACE]);

			++cur_seq;
			if (cur_seq >= num_seqs) cur_seq = 0;

			seq_frames = seq->getAnimSeq()[cur_seq].size();
			cur_frame_num = 0;
			cur_frame_duration = seq->getAnimSeq()[cur_seq][cur_frame_num].frameDuration;
		}

		if (key[KEY_R])
    {
      rotation_mode = !rotation_mode;
      while(key[KEY_R]);
    }
		if (key[KEY_A])
		{
			invertDraw = !invertDraw;
      while(key[KEY_A]);
		}

		cur_frame = seq->getAnimSeq()[cur_seq][cur_frame_num].frameNumber;
		if (!rotation_mode)
    {
      if (!invertDraw)
        draw_sprite(buffer, seq->getFrame(cur_frame), (SCREEN_W / 2) - (w / 2), (SCREEN_H / 2) - (h / 2));
      else
        draw_sprite_h_flip(buffer, seq->getFrame(cur_frame), (SCREEN_W / 2) - (w / 2), (SCREEN_H / 2) - (h / 2));
    }
    else
    {
      rotate_sprite(buffer,
                    seq->getFrame(cur_frame),
                    (SCREEN_W / 2) - (w / 2), (SCREEN_H / 2) - (h / 2),
                    itofix(rotations[cur_rotation]));
    }

    if (key[KEY_RIGHT])
    {
      cur_rotation++;
      if (cur_rotation == 4) cur_rotation = 0;

      while(key[KEY_RIGHT]);
    }
		textprintf_ex(buffer, font, 5, 5,  makecol(255,255,255), -1, "N de Sequencias: [%d]", num_seqs);
		textprintf_ex(buffer, font, 5, 15, makecol(255,255,255), -1, "Sequencia atual: [%d]", cur_seq);
		textprintf_ex(buffer, font, 5, 25, makecol(255,255,255), -1, "N de Frames da Sequencia atual: [%d]", seq_frames);
		textprintf_ex(buffer, font, 5, 35, makecol(255,255,255), -1, "Frame Atual na Sequencia: [%d]", cur_frame_num);
		textprintf_ex(buffer, font, 5, 45, makecol(255,255,255), -1, "Duração do Frame Atual: [%d]", cur_frame_duration);
		textprintf_ex(buffer, font, 5, 55, makecol(255,255,255), -1, "Frame na Sequencia: [%d]", cur_frame);
		textprintf_ex(buffer, font, 5, 65, makecol(255,255,255), -1, "Sentido Direita: [%d]", !invertDraw);
    textprintf_ex(buffer, font, 5, 75, makecol(255,255,255), -1, "Rotacao: [%d]", rotations[cur_rotation]);

		blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
		clear_bitmap(buffer);
	}

	delete seq;

	allegro_exit();

	return 0;
}

