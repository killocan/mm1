/*
 * Killocan 2014
 * http://killocan.blogspot.com
*/

#pragma once

namespace mm_spritefiles
{
  enum SPRT_TYPE {
    MEGAMAN_SPRITES, //Megaman
    WEAPONS_SPRITES, //Various weapons
    DUMMY,
    BLADER_SPRITES,         //Blader
    BEAK_SPRITES,           //Beak
    SUPERCUTTER_SPRITES,    //Super Cutter
    FLYINGSHELL_SPRITES,    //Flying Shell
    KILLERBULLET_SPRITES,   //Killer Bullet
    SPINE_SPRITES,          //Spine
    MET_SPRITES,            //Met
    PICKETMAN_SPRITES,      //Picket Man
    SNIPERJOE_SPRITES,      //Sniper Joe
    WATCHER_SPRITES,        //Watcher
    TACKLEFIRE_SPRITES,     //Tackle Fire
    PENG_SPRITES,           //Peng
    FOOTHOLDER_SPRITES,     //Foot Holder
    CRAZYRAZY_SPRITES,      //Crazy Razy
    BIGEYE_SPRITES,         //Big Eye
    FLEA_SPRITES,           //Flea
    BOMBOMB_SPRITES,        //Bombomb
    OCTOPUSBATTERY_SPRITES,   //Octopus Battery Horizontal
    OCTOPUSBATTERY_SPRITES_V, //Octopus Battery Vertical (Same of OCTOPUSBATTERY_SPRITES)
    SCREWBOMBER_SPRITES,    //Screw Bomber
    TIMER_PLATFORM_SPRITES, // timer platform.
    MOVING_PLATFORM_SPRITES, // Moving platform.
    SPARKLE_SPRITES,         // Electric spark (background only).
    LIGHTNING_WALL_SPRITES,  //Device on wall that cause a lightning.
    FIRE_H_WALL_SPRITES,     // Device on wall that fires fire :) horizontal.
    FIRE_V_WALL_SPRITES,     // Device on floor that fires fire :P vertical.
    PLATFORM_WEAPON_SPRITES,        //Megaman platform weapon.
    RECHARGE_ALL_SPRITES,           //Recharge all weapons.
    BIG_LIFE_RECHARGE_SPRITES,      //Life recharger BIG.
    LITTLE_LIFE_RECHARGE_SPRITES,   //Life recharger LITTLE.
    BIG_WEAPON_RECHARGE_SPRITES,    //Weapon recharger BIG.
    LITTLE_WEAPON_RECHARGE_SPRITES, //Weapon recharger LITTLE.
    GUTSMANROCK_SPRITES, //Rock that can be picked up by gutsman or megaman(when using gustman weapon).
    NEW_LIFE_SPRITES,     // New Life
    WEAPONS_MAGNETIC,//(platform) magnetic beam
    WEAPONS_CUTMAN,  //cutman weapon
    WEAPONS_GUTSMAN, //gustman weapon (debris from rock)
    WEAPONS_ICEMAN,  //iceman weapon
    WEAPONS_BOMBMAN, //bombman weapon
    WEAPONS_FIREMAN, //fireman weapon
    WEAPONS_ELECMAN, //elecman weapon
    BOSSDOOR_SPRITES,

    HITEXPLOSION_SPRITES,
    EXPLOSIONLITTLE_SPRITES,
    MEGAMANEXPLOSION_SPRITES,
    PICKETMANHAMMER_SPRITES,
    CRAZYRAZY_LOWER_SPRITES,
    CRAZYRAZY_UPPER_SPRITES,
    SPARKLE_ENM_SPRITES,
    BONUS_POINT_SPRITES,

    GUTSMAN_SPRITES,
    CUTMAN_SPRITES,
    ICEMAN_SPRITES,
    BOMBMAN_SPRITES,
    FIREMAN_SPRITES,
    ELECMAN_SPRITES,
    YELLOW_DEVIL_SPRITES,
    COPY_ROBOT_SPRITES,
    CWU01P_SPRITES,
    WILLY_SPRITES,

    MAGNET_BEAM_SPRITES,
  };

  extern const char * sprite_files[];
}
