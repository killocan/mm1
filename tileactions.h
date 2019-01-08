/*
 * Killocan 2013
 * http://killocan.blogspot.com
*/

#pragma once

namespace mm_tile_actions
{
  enum TILE_ACTIONS
  {
    TILE_VOID,    //Empty tile
    TILE_SOLID,   //Solid tile
    TILE_DEATH,   //Cause megaman death
    TILE_ENEMY_BLADER,         //Blader
    TILE_ENEMY_BEAK,           //Beak
    TILE_ENEMY_SUPERCUTTER,    //Super Cutter
    TILE_ENEMY_FLYINGSHELL,    //Flying Shell
    TILE_ENEMY_KILLERBULLET,   //Killer Bullet
    TILE_ENEMY_SPINE,          //Spine
    TILE_ENEMY_MET,            //Met
    TILE_ENEMY_PICKETMAN,      //Picket Man
    TILE_ENEMY_SNIPERJOE,      //Sniper Joe
    TILE_ENEMY_WATCHER,        //Watcher
    TILE_ENEMY_TACKLEFIRE,     //Tackle Fire
    TILE_ENEMY_PENG,           //Peng
    TILE_ENEMY_FOOTHOLDER,     //Foot Holder
    TILE_ENEMY_CRAZYRAZY,      //Crazy Razy
    TILE_ENEMY_BIGEYE,         //Big Eye
    TILE_ENEMY_FLEA,           //Flea
    TILE_ENEMY_BOMBOMB,        //Bombomb
    TILE_ENEMY_OCTOPUSBATTERY_H, //Octopus Battery H
    TILE_ENEMY_OCTOPUSBATTERY_V, //Octopus Battery V
    TILE_ENEMY_SCREWBOMBER,    //Screw Bomber
    TILE_TIMER_PLATFORM, // Timer platform.
    TILE_MOVING_PLATFORM, // Moving platform.
    TILE_SPARKLE,         // Electric spark (background only).
    TILE_LIGHTNING_WALL,  //Device on wall that cause a lightning.
    TILE_FIRE_H_WALL,     // Device on wall that fires fire :) horizontal.
    TILE_FIRE_V_WALL,     // Device on floor that fires fire :P vertical.
    TILE_PLATFORM_WEAPON,        //Megaman platform weapon.
    TILE_RECHARGE_ALL,           //Recharge all weapons.
    TILE_BIG_LIFE_RECHARGE,      //Life recharger BIG.
    TILE_LITTLE_LIFE_RECHARGE,   //Life recharger LITTLE.
    TILE_BIG_WEAPON_RECHARGE,    //Weapon recharger BIG.
    TILE_LITTLE_WEAPON_RECHARGE, //Weapon recharger LITTLE.
    TILE_GUTSMAN_ROCK, //Rock that can be picked up by gutsman or megaman(when using gustman weapon).
    TILE_NEW_LIFE,     //Increment life count in one.
    TILE_SCROLL_LIMIT, //Define an screen that cannot be shown (camera avoid scrool to this areas).
    TILE_BOSS,   //Mark where is the boss room.
    TILE_TIMER,  //Mark where some timer starts or ends (engine control).
    TILE_STAIR_BEGIN,  //Stair Begin (marks a tile where a stait begin(up to down)) :D
    TILE_STAIR,        //Stair.
    TILE_MEGAMAN_WAYPOINT, //Mark where to place megaman.
    TILE_MAP_BEGIN,    //Mark where to start the camera.
    TILE_DOOR,   //Boss doors.

    // NOT FROM TILES
    HIT_EXPLOSION_CHAR,
    EXPLOSION_LITTLE_CHAR,
    MEGAMAN_EXPLOSION_CHAR,
    BOMBOMB_FRAGMENT_CHAR,
    PICKETMAN_HAMMER_CHAR,
    CRAZYRAZY_LOWER_CHAR,
    CRAZYRAZY_UPPER_CHAR,
    TACKLEFIRE_FRAGMENT,
    SPARKLE_ENM_CHAR,
    WATCHER_FRAGMENT,
    BONUS_POINT_CHAR,

    GUTSMAN,
    CUTMAN,
    ICEMAN,
    BOMBMAN,
    FIREMAN,
    ELECMAN,
    YELLOWDEVIL,
    COPYROBOT,
    CWUO1P,
    WILLY,

    MAGNET_BEAM,
    MOVING_FLAME,
    MOVING_FLAME_FRAGMENT,

    TILE_TIMER_PLATFORM_FRAGMENT,
    GUTSMAN_GUN,
    GUTSMAN_GUN_FRAGMENT,
  };
}
