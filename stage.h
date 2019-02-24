/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

#include <string>
#include <vector>
#include <map>

#include "camera.h"
#include "player.h"
#include "animsequence.h"
#include "spritefiles.h"

class Stage
{
  public:
    Stage(std::string stage_path, Camera & camera, Player ** player);//, std::vector<Character *> & characters_vec);
    ~Stage();

    void createEnemies(std::vector<Character *> & characters_vec);
    Character * createCharacter(int TYPE, int x, int y, int vx = 0, int vy = 0, void * param = NULL);

    unsigned char tileAction(int x, int y) const;
    unsigned char tileNumber(int x, int y) const;

    unsigned char tileActionUnnormalized(int x, int y) const;
    unsigned char tileNumberUnnormalized(int x, int y) const;

    void setTileAction(int x, int y, unsigned char action);
    void setTileNumber(int x, int y, unsigned char tile_number);

    void draw(BITMAP * bmp, bool hasFg, bool bg_only=true, bool bg=true);
    void scrollForbid(Camera & camera);
	bool cameraForbiden(int x, int y);
    bool doCamera(Camera & camera);
    bool cameraSectorHasFgTiles(Camera & camera, Player & player);

    AnimSequence * getAnimSeq(int TYPE);
    BITMAP * getTileSet();
    BITMAP * getTile(unsigned int index);

    // Return the height of the ray cast from y to the nearest floor tile.
    int calculateHeight(float x, float w, float y);

    // For any entity that wants to check collision agains the map.
    bool genericColHor(int x, int w, int y, int &tilecoordx, int &tilecoordy, bool going_down);
    bool genericColVer(int x, int y, int h, int &tilecoordx, int &tilecoordy);

    void setOffset(int, int);
    int getOffset(int);

    void setWaypoint(Player * player);

    void defineCameraSector(int x, int y, bool state);
    bool checkForBoss(int x, int y);

    bool has_fg_tiles;
    int max_x, max_y;
    int scroll_count;

    bool horz_scroll;

    Player * m_player;
    int stageNumber;

    bool hasMovingPlatform;
    std::vector<int> platformTiles;

    void resetReachMaxX();

    void runtimeLoadCharacter(mm_spritefiles::SPRT_TYPE);

    bool finished;
  private:
    struct MAP_INFO
    {
      unsigned char tile_number;
      unsigned char action;
      int xOffset;
      bool isForeground;
    };
    MAP_INFO ** map;

    struct sector_t
    {
      bool scroll_forbid;
      bool has_fg_tiles;
      bool has_boss;
    };
    sector_t * sectors;

    struct waypoint_t
    {
      int x,y;
    };
    std::vector<waypoint_t> waypoints;

    struct tiles_t
    {
      std::vector<BITMAP*> tile_img;
      std::vector<int> x_pos;
    };
    tiles_t tiles;

    BITMAP * tileset;

    int default_tile;

    int cur_waypoint;

    bool reach_max_x;

    std::map<int, AnimSequence *> preLoadedSprites;

    std::map<int, int> offsetMap;

    void loadTiles(const std::string & stage_file);
    int load(const std::string & stage_path, Camera & camera, Player ** player);
    int unload();
};
