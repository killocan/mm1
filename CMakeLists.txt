cmake_minimum_required(VERSION 3.12)
project(megaman)

set(CMAKE_CXX_STANDARD 14)

include_directories( . )

add_executable(megaman main.cpp mm_math.cpp soundpack.cpp spritefiles.cpp globalgamestate.cpp text_utils.cpp animsequence.cpp energybar.cpp stage.cpp  stagemanager.cpp collision.cpp character.cpp player.cpp weapons.cpp weaponmenu.cpp scenesoundmanager.cpp intro.cpp  cutmanstage.cpp gutsmanstage.cpp icemanstage.cpp bombmanstage.cpp firemanstage.cpp elecmanstage.cpp  dropitem.cpp movingflame.cpp disappearingblock.cpp  menu.cpp blader.cpp footholder.cpp beak.cpp supercutter.cpp flyingshell.cpp killerbullet.cpp spine.cpp met.cpp picketman.cpp sniperjoe.cpp  peng.cpp bigeye.cpp flea.cpp octopusbattery.cpp screwbomber.cpp bombomb.cpp crazyrazy.cpp moveplatform.cpp  tempcharacter.cpp lifechargerlittle.cpp lifechargerbig.cpp weaponchargerlittle.cpp weaponchargerbig.cpp newlifeitem.cpp  bonuspoint.cpp fireman.cpp gutsman.cpp gutsmanrock.cpp  hitexplosion.cpp explosionlittle.cpp sparkle.cpp lightningwall.cpp firepillar.cpp firebolt.cpp megamanexplosion.cpp  sparkleenm.cpp watcher.cpp tacklefire.cpp bossdoor.cpp stageorb.cpp)