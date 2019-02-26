/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#include <allegro.h>
#include <cstdlib>
#include <cstdio>

#include "menu.h"
#include "globals.h"
#include "soundpack.h"
#include "scenesoundmanager.h"
#include "sfx.h"
#include "globalgamestate.h"
#include "character.h"
#include "text_utils.h"
#include "animsequence.h"

#include "cutmanstage.h"
#include "gutsmanstage.h"
#include "icemanstage.h"
#include "bombmanstage.h"
#include "firemanstage.h"
#include "elecmanstage.h"

volatile int update_mm_menu = 0;
void update_menu_tick()
  {
  update_mm_menu = 1;
  }
END_OF_FUNCTION(update_menu_tick)

static const int ENEMYS_SLOTS = 7;
static int curSelection = 0;

static const char * BOSS_NAMES[] = {"CUTMAN",
                                    "GUTSMAN",
                                    "ICEMAN",
                                    "BOMBMAN",
                                    "FIREMAN",
                                    "ELECMAN",
                                    "DrWILY"};
BITMAP * STRIP;

struct coord{int x,y;};
static coord FRAMES_POS[] = {{148,36},  // 0 CUTMAN
                             {276,36},  // 1 GUTSMAN
                             {340,164}, // 2 ICEMAN
                             {276,292}, // 3 BOMBMAN
                             {148,292}, // 4 FIREMAN
                             {84, 164}, // 5 ELECMAN
                             {230,220}};// 6 DrWILY

//                                     P1         P2        P3        P4
static int BOSS_CTRL_POINTS[][8] = {{148,36,   168,-60,   168,-60,  168,178}, // CUTMAN
                                    {276,36,   260,  0,   260,  0,  168,178}, // GUTSMAN
                                    {340,164,  260,  0,   260,  0,  168,178}, // ICEMAN
                                    {276,292,  260, 20,   260, 20,  168,178}, // BOMBMAN
                                    {148,292,  168,  0,   168,  0,  168,178}, // FIREMAN
                                    {84 ,164,  110,  0,   110,  0,  168,178}};// ELECMAN
static BITMAP * FRAMES_IMG[2];

AnimSequence *boss_show_seq[6];

struct frame_time
{
  int frame; // -1 = start moving | -2 stop animation.
  int time;
};
frame_time cutman_frame_time[]  = {{-1,0},{0, 0},       {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {1,8},{2,8}, {-2,0}};
frame_time gutsman_frame_time[] = {{2,10},{-1,0},{3,0}, {2,10}, {0,15},{1,15},{0,15},{1,15},{0,15},{1,15},{0,15},{1,15}, {0,0},{-2,0}};
frame_time iceman_frame_time[]  = {{-1,0},{1, 0},       {2,8},{0,8}, {2,8},{3,8}, {-2,0}};
frame_time bombman_frame_time[] = {{-1,0},{3, 0},       {1,60},{0,30},{1,10},{-2,0}};
frame_time fireman_frame_time[] = {{-1,0},{1, 0},       {2,5},{0,10},{2,10},{4,10},{3,10},{4,10},{3,10},{4,10},{3,10},{4,10},{-2,0}};
frame_time elecman_frame_time[] = {{-1,0},{1, 0},       {3,5},{0,10},{2,10},{3,10},{-2,0}};

struct boss_anim_st
{
  int still_frame;
  frame_time * sequence;
  bool should_mirror;
};
boss_anim_st boss_anim[6] = {
                            {2, cutman_frame_time,  false}, // CUTMAN
                            {0, gutsman_frame_time, true},  // GUTSMAN
                            {0, iceman_frame_time,  true},  // ICEMAN
                            {0, bombman_frame_time, true}, // BOMBMAN
                            {0, fireman_frame_time, false}, // FIREMAN
                            {0, elecman_frame_time, false}, // ELECMAN
                            };

static int bgcolor  = 0;
static int white    = 0;
static int cyan     = 0;
static int yellow   = 0;
static int dblue    = 0;

//static SceneSoundManager * ssm = NULL;

static bool bBlink = false;

static void draw_texts(BITMAP * buffer, FONT * mm_font)
{
  int count = (GlobalGameState::allEnemysDefeated) ? 7 : 6;
  int color = cyan;
  int x, y;
  for (int i = 0; i < count; i++)
  {
    color = cyan;
    if (i == curSelection)
    {
      color = bBlink ? white : cyan;
    }

    if (i == 2)
      x = FRAMES_POS[i].x + 4;
    else if (i == 5)
      x = FRAMES_POS[i].x - 10;
    else
      x = FRAMES_POS[i].x - 6;

    y = FRAMES_POS[i].y + 92;

    textout_ex(buffer, mm_font, BOSS_NAMES[i], x, y, color, -1);
  }

  if (GlobalGameState::allEnemysDefeated == false)
  {
    color = bBlink ? white : cyan;

    int y = 172;
    int h = text_height(mm_font);
    textout_ex(buffer, mm_font, "SELECT", 216, y, color, -1); 
    y+=h+1;
    textout_ex(buffer, mm_font, "STAGE",  216, y, color, -1); 
    y+=h<<1;
    textout_ex(buffer, mm_font, "PRESS",  216, y, color, -1); 
    y+=h+1;
    textout_ex(buffer, mm_font, "START",  216, y, color, -1); 
  }
}

static void draw_frames(BITMAP * buffer)
{
  int count = (GlobalGameState::allEnemysDefeated) ? 7 : 6;
  BITMAP * img = NULL;
  BITMAP * boss_frame = NULL;
  int color = 0;
  int x, y;
  for (int i = 0; i < count; i++)
  {
    img = FRAMES_IMG[0];
    if (i == curSelection)
    {
      img = FRAMES_IMG[bBlink];
    }
    
    if (GlobalGameState::enemyDefeated[i])
      color = 0;
    else
      color = yellow;

    rectfill(buffer, FRAMES_POS[i].x+12, FRAMES_POS[i].y+12, FRAMES_POS[i].x+76, FRAMES_POS[i].y+76, color);
    draw_sprite(buffer, img, FRAMES_POS[i].x, FRAMES_POS[i].y);
    
    boss_frame = boss_show_seq[i]->getFrame(boss_anim[i].still_frame);
    x = FRAMES_POS[i].x+12+(32-boss_frame->w/2);
    y = FRAMES_POS[i].y+(76-boss_frame->h);

    BOSS_CTRL_POINTS[i][0] = x;
    BOSS_CTRL_POINTS[i][1] = y;
    draw_sprite(buffer, boss_frame, x, y);
  }
}

#ifndef DEBUG
static void boss_show(BITMAP * buffer, FONT * mm_font)
{
  SceneSoundManager bossShowMusic(mm_soundpack::sounds[mm_soundpack::STAGE_SELECTED_MUSIC]);

  BITMAP * bomb = NULL;
  float bomb_y = 0.0f;
  float bomb_vy = -3.2f;
  float bomb_vy_step = -.15f;

  static const int CURVE_PNTS = 55;
  static int curveX[CURVE_PNTS] = {};
  static int curveY[CURVE_PNTS] = {};

  calc_spline(BOSS_CTRL_POINTS[curSelection], CURVE_PNTS, curveX, curveY);
  Character boss;
  boss.sx = curveX[0];
  boss.sy = curveY[0];
  boss.w  = 45;
  boss.h  = 64;

  Sounds::mm_sounds->play(BOSS_SELECTED);

  BITMAP * boss_frame = NULL;
  int blink  = 10;
  int anim_frame = 0;
  int curve_idx = 0;
  int curFrame, frameDuration;
  int anim_ticks = 0;
  bool bAnimOn   = true;
  bool bMenuEnd  = false;
  bool bBlinking = false;
  bool moving = false;
  bool boss_show_ended = false;

  enum tmpStates {WAITING, NAMES, POINTS};
  tmpStates infoStatus = WAITING;

  curFrame      = boss_anim[curSelection].sequence[anim_frame].frame;
  frameDuration = boss_anim[curSelection].sequence[anim_frame].time;

  // Bombman bomb
  if (curSelection == 3)
  {
    bomb = boss_show_seq[curSelection]->getFrame(4);
  }

  bossShowMusic.play(0);

  ++anim_frame;
  while(bMenuEnd == false)
  {
    if (update_mm_menu == 1 && blink)
    {
      update_mm_menu = 0;
      --blink;
      bBlinking = !bBlinking;
    }
    else if (blink == 0)
    {
      bBlinking = false;
    }

    if (bBlinking)
    {
      clear_to_color(buffer, white);
    }
    else
    {
      clear_to_color(buffer, bgcolor);
      blit(STRIP, buffer, 0,0, 0,145, STRIP->w, STRIP->h);
    }

    if (curFrame == -1)
    {
      frameDuration = boss_anim[curSelection].sequence[anim_frame].time;
      curFrame      = boss_anim[curSelection].sequence[anim_frame].frame;

      anim_frame++;
      moving = true;
    }

    if (GlobalGameState::game_logic_tick == 1)
    {
      if (moving == true)
      {
        boss.sx = curveX[curve_idx];
        boss.sy = curveY[curve_idx];

        if (curve_idx < CURVE_PNTS-1)
          ++curve_idx;
        else
          moving = false;
      }
      else if (moving == false && boss_show_ended == false)
      {
        if (frameDuration <= 0)
        {
          int curFrameTmp = curFrame;
          frameDuration = boss_anim[curSelection].sequence[anim_frame].time;
          curFrame      = boss_anim[curSelection].sequence[anim_frame].frame;

          switch(curFrame)
          {
            case -1:
            {
              anim_frame++;
              continue;
            }
            break;
            case -2:
            {
              boss_show_ended = true;
              curFrame = curFrameTmp;
            }
            break;
            default:
            {
              anim_frame++;
            }
            break;
          }
        }
        else
        {
          anim_ticks++;
          frameDuration--;
        }

        if (bomb != NULL && (anim_frame == 4 || anim_frame == 5))
        {
          bomb_y += bomb_vy;
          bomb_vy -= bomb_vy_step;
        }
      }

      GlobalGameState::game_logic_tick = 0;
    }

    boss_frame = boss_show_seq[curSelection]->getFrame(curFrame);
    if (curve_idx < CURVE_PNTS-1)
    {
      draw_sprite(buffer, boss_frame, boss.sx, boss.sy);
    }
    else
    {
      if (bomb != NULL)
      {
        draw_sprite(buffer, bomb, boss.sx+26, boss.sy-34+((int)bomb_y));
      }

      if (boss_anim[curSelection].should_mirror == false)
      {
        draw_sprite(buffer, boss_frame, boss.sx, boss.sy);
      }
      else
      {
        draw_sprite_h_flip(buffer, boss_frame, boss.sx, boss.sy);
      }
    }

    const char * infoStr[] = { BOSS_NAMES[curSelection], "CLEAR POINTS" };
    if (bAnimOn == false)
    {
      switch (infoStatus)
      {
        int y, x;

        case WAITING:
        {
          y = 178;
          x = 270;
          float time_left = 3000.0f - (anim_ticks*(1000.0f / 60.0f));
          if (time_left < 0.0f) time_left = 0;
          rest((int)time_left);
          infoStatus = NAMES;
        }
        break;
        case NAMES:
        {
          for (int count = 0; count < 2; count++)
          {
            for (unsigned int j = 0; j < strlen(infoStr[count])+1; ++j)
            {
              draw_text(screen, mm_font, x, y, white, dblue, infoStr[count], j);
              rest(100);
            }
            y += text_height(mm_font) + 10;
          }

          infoStatus = POINTS;
        }
        break;
        case POINTS:
        {
          #define oht 100*1000
          #define tt  10*1000
          int clearPoints = rand() % 10 * 10000; //GlobalGameState::enemyClearPoints[curSelection];
          int rndNumber = (rand() % 10) * ((clearPoints >= oht) ? oht : tt);

          textprintf_ex(buffer, mm_font, x, 178, white, -1, "%s", infoStr[0]);
          textprintf_ex(buffer, mm_font, x, 178 + text_height(mm_font) + 10, white, -1, "%s", infoStr[1]);

          static int timer = 0;
          if (timer <= 50)
          {
            draw_number(buffer, mm_font, x, y, white, dblue, rndNumber);
            rest(25);
            ++timer;
          }
          else
          {
            textprintf_ex(buffer, mm_font, x + 2, y + 2, 0, dblue, "%d  ", clearPoints);
            textprintf_ex(buffer, mm_font, x, y, white, -1, "%d  ", clearPoints);
            timer = 0;
            bMenuEnd = true;
          }
        }
        break;
      }
    }

    Vsync::Sync();
    blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);

    if ((blink == 0) && (curve_idx == CURVE_PNTS-1) && (boss_show_ended == true))
    {
      remove_int(update_menu_tick);
      update_mm_menu = 0;
      bAnimOn = false;
    }
  }

  rest(1000);
}
#endif

static void goToStage()
{
  StageManager * stage = NULL;

  switch(curSelection)
  {
    case 0:
      stage = new CutManStage;
    break;
    case 1:
      stage = new GutsManStage;
    break;
    case 2:
      stage = new IceManStage;
    break;
    case 3:
      stage = new BombManStage;
    break;
    case 4:
      stage = new FireManStage;
    break;
    case 5:
      stage = new ElecManStage;
    break;
    case 6:
      ;
    break;
  }

  if (stage != NULL) stage->play();
  delete stage;
}

void menu()
{
  BITMAP * buffer  = Buffer::buffer;
  FONT   * mm_font = Font::mm_font;
  int colorIndex = 0;

  bgcolor  = makecol(0,   112, 236);
  white    = makecol(255, 255, 255);
  cyan     = makecol(156, 252, 240);
  yellow   = makecol(240, 188, 60);
  dblue    = makecol(24,  60,  92);

  char buf[80];
  char lower_case_name[15];
  for (unsigned int i = 0, j = 0; i < 6; i++)
  {
    for (j = 0; j < strlen(BOSS_NAMES[i]); j++)
    {
      lower_case_name[j]=BOSS_NAMES[i][j]+32;
    }
    lower_case_name[j]=0;

    sprintf(buf, "./game_data/%s_menu_seq.dat", lower_case_name);
    boss_show_seq[i] = new AnimSequence(buf);
  }

  FRAMES_IMG[0] = load_bitmap("./game_data/frame_b.bmp", NULL);
  FRAMES_IMG[1] = load_bitmap("./game_data/frame_w.bmp", NULL);

  STRIP = create_bitmap(SCREEN_W, 128);
  clear_to_color(STRIP, dblue);

  LOCK_VARIABLE(update_mm_menu);
  LOCK_FUNCTION(update_menu_tick);

  clear_to_color(buffer, bgcolor);

  //ssm = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::MENU_SOUNDS]);
  //ssm = new SceneSoundManager(mm_soundpack::sounds[mm_soundpack::STAGE_SELECT_MUSIC]);
  //ssm->play(0, true);
  Sounds::mm_sounds->play(MENU_BG, true);

  install_int(update_menu_tick, 300);

  bool btnPressR = false, btnPressL = false;
  while(!key[KEY_ESC])
  {
    int disp = GlobalGameState::allEnemysDefeated ? 0 : 1;
    if (key[KEY_RIGHT] && btnPressR == false)
    {
      Sounds::mm_sounds->play(MENU_ARROW);
      curSelection++;
      if (curSelection == ENEMYS_SLOTS-disp) curSelection = 0;
      btnPressR = true;
      bBlink = true;
    }
    else if (!key[KEY_RIGHT] && btnPressR == true)
    {
      btnPressR = false;
    }

    if (key[KEY_LEFT] && btnPressL == false)
    {
      Sounds::mm_sounds->play(MENU_ARROW);
      curSelection--;
      if (curSelection < 0) curSelection = ENEMYS_SLOTS-(1+disp);
      btnPressL = true;
      bBlink = true;
    }
    else if (!key[KEY_LEFT] && btnPressL == true)
    {
      btnPressL = false;
    }

    if (key[KEY_ENTER])
    {
      clear_keybuf();

      Sounds::mm_sounds->stopSample(MENU_BG);

#ifndef DEBUG
      install_int(update_menu_tick, 120);
      boss_show(buffer, mm_font);
      remove_int(update_menu_tick);
#endif

      goToStage();

#ifndef DEBUG
      install_int(update_menu_tick, 300);
#endif
      
      Sounds::mm_sounds->play(MENU_BG, true);
    }

    if (update_mm_menu == 1)
    {
      update_mm_menu = 0;
      colorIndex = 1 - colorIndex;
      bBlink = !bBlink;
    }

    draw_frames(buffer);
    draw_texts(buffer, mm_font);

    blit(buffer, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
    clear_to_color(buffer, bgcolor);
  }

  Sounds::mm_sounds->stopSample(MENU_BG);
  //ssm->stopAll();
  //delete ssm;

  destroy_bitmap(STRIP);

  destroy_bitmap(FRAMES_IMG[0]);
  destroy_bitmap(FRAMES_IMG[1]);
}
