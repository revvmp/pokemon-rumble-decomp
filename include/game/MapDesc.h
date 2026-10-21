#ifndef GAME_MAPDESC_H
#define GAME_MAPDESC_H

#include <game/CPpuRegister.h>
#include <game/MapIndex.h>

struct MapDesc {
    MapIndex mIndex;
    CPpuRegister mParams;
};

#endif
