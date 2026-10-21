#ifndef GAME_STAGEDESC_H
#define GAME_STAGEDESC_H

#include <game/MapIndex.h>
#include <game/CPpuRegister.h>
#include <memory>
#include <vector>

struct MapDesc;
struct DescParams;

typedef CPpuRegister DescParamTable;

struct StageDesc {
    StageDesc() {}
    void Init(MapIndex index);

    MapIndex mIndex;
    DescParamTable mParamTable;
    std::vector<std::tr1::shared_ptr<MapDesc> > mMapDescs;
};

#endif
