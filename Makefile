CC=g++

ifneq (,$(findstring Windows,$(PATH)))
	LDFLAGS=./libs/win32/libalmp3.a -lalleg
	CFLAGS=-c -Wall -I./libs_inc
	RM=del
else
	ARCHITECTURE := $(shell uname -p)
    	ifeq ($(ARCHITECTURE),x86_64)
			LDFLAGS=./libs/linux/libalmp3_64.a `allegro-config --libs`
		else
			LDFLAGS=./libs/linux/libalmp3.a `allegro-config --libs`
    	endif

	CFLAGS=-c -Wall -I./libs_inc
	RM=rm -f
endif

ifdef PROFILING
	CFLAGS += -pg
	LDFLAGS += -pg
endif

ifdef FINAL
	LDFLAGS += -mtune=native -O3
	CFLAGS += -O3
else
	LDFLAGS += -g
	CFLAGS += -g -DDEBUG
endif

SOURCES=main.cpp mm_math.cpp soundpack.cpp spritefiles.cpp globalgamestate.cpp text_utils.cpp animsequence.cpp energybar.cpp stage.cpp \
        stagemanager.cpp collision.cpp character.cpp player.cpp weapons.cpp weaponmenu.cpp scenesoundmanager.cpp intro.cpp \
        cutmanstage.cpp gutsmanstage.cpp icemanstage.cpp bombmanstage.cpp firemanstage.cpp elecmanstage.cpp \
        dropitem.cpp \
        menu.cpp blader.cpp beak.cpp supercutter.cpp flyingshell.cpp killerbullet.cpp spine.cpp met.cpp picketman.cpp sniperjoe.cpp \
        peng.cpp bigeye.cpp flea.cpp octopusbattery.cpp screwbomber.cpp bombomb.cpp crazyrazy.cpp moveplatform.cpp \
        tempcharacter.cpp lifechargerlittle.cpp lifechargerbig.cpp weaponchargerlittle.cpp weaponchargerbig.cpp newlifeitem.cpp \
        bonuspoint.cpp gutsman.cpp gutsmanrock.cpp \
        hitexplosion.cpp explosionlittle.cpp sparkle.cpp lightningwall.cpp firepillar.cpp firebolt.cpp megamanexplosion.cpp \
        sparkleenm.cpp watcher.cpp tacklefire.cpp bossdoor.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=megaman

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	$(RM) *.o *.out megaman megaman.exe
