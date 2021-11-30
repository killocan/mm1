/*
 * Killocan 2021 
 * http://killocan.blogspot.com
*/

#include <allegro.h>

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <string>
#include <vector>


typedef struct 
{
  unsigned char tile_number;
  unsigned char action;
  int xOffset;
  bool isForeground;
} MAP_INFO;

MAP_INFO **map;

typedef struct 
{
  unsigned char tile_number;
  unsigned char action;
  int xOffset;
  bool isForeground;
  bool isIce;
} NEW_MAP_INFO;

int x_desloc = 0;
int cur_tile = 0;
int cur_act  = 0;

int max_x, max_y;
unsigned char default_tile;

int map_load(FILE * fp)
{
  fread(&max_x, sizeof(int), 1, fp);
  fread(&max_y, sizeof(int), 1, fp);
  fread(&default_tile, sizeof(unsigned char), 1, fp);

  map = (MAP_INFO**) malloc(sizeof(MAP_INFO*) * max_y);
  for(int i = 0; i < max_y; i++)
  {
    map[i] = (MAP_INFO*) malloc(sizeof(MAP_INFO) * max_x);
    fread(&map[i][0], sizeof(MAP_INFO), max_x, fp);
  }

  fclose(fp);

  return 0;
}

void convert_map(std::string & stage)
{
  FILE * fp = NULL;
  if ((fp = fopen(stage.c_str(), "rb")) != NULL)
  {
    map_load(fp);
  }
  
  NEW_MAP_INFO ** converted_map;
  converted_map = (NEW_MAP_INFO**) malloc(sizeof(NEW_MAP_INFO*) * max_y);

  for(int i = 0; i < max_y; i++)
  {
    converted_map[i] = (NEW_MAP_INFO*) malloc(sizeof(NEW_MAP_INFO) * max_x);
    for (int j = 0; j < max_x; j++)
    {
      converted_map[i][j].tile_number  = map[i][j].tile_number;
      converted_map[i][j].action       = map[i][j].action;
      converted_map[i][j].xOffset      = map[i][j].xOffset;
      converted_map[i][j].isForeground = map[i][j].isForeground;
      converted_map[i][j].isIce        = false;
    }
  }

  printf("OLD STRUCT SIZE:\n");
  printf("FIELD SUM = %lu\n", sizeof(map[0][0].tile_number)+
                             sizeof(map[0][0].action)+
                             sizeof(map[0][0].xOffset)+
                             sizeof(map[0][0].isForeground));
  printf("STRUCT SIZE = %lu\n", sizeof(MAP_INFO));
  printf("\nNEW STRUCT SIZE:\n");
  printf("FIELD SUM = %lu\n", sizeof(converted_map[0][0].tile_number)+
                             sizeof(converted_map[0][0].action)+
                             sizeof(converted_map[0][0].xOffset)+
                             sizeof(converted_map[0][0].isForeground)+
                             sizeof(converted_map[0][0].isIce));
  printf("STRUCT SIZE = %lu\n", sizeof(NEW_MAP_INFO));
  
  std::string stageOut = stage;
  stageOut += ".new";
  fp = fopen(stageOut.c_str(), "wb+");

  fwrite(&max_x, sizeof(int), 1, fp);
  fwrite(&max_y, sizeof(int), 1, fp);
  fwrite(&default_tile, sizeof(unsigned char), 1, fp);

  for (int i = 0; i < max_y; i++)
  {
    fwrite(&converted_map[i][0], sizeof(NEW_MAP_INFO), max_x, fp);
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  std::string filename = argv[1];
  filename += "/stage.dat";
  convert_map(filename);

  return 0;
}

