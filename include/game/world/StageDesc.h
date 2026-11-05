#ifndef GAME_STAGEDESC_H
#define GAME_STAGEDESC_H

#include <game/world/MapIndex.h>
#include <game/script/CPpuRegister.h>
#include <memory>
#include <vector>

struct MapDesc;
struct DescParams;

typedef CPpuRegister DescParamTable;

struct StageDesc {
#if defined(CGAMESESSION_EMISSION_LAYOUT)
    StageDesc();
    ~StageDesc();
#else
    StageDesc() {}
#endif
    void Init(MapIndex index);

    MapIndex mIndex;
    DescParamTable mParamTable;
    std::vector<std::tr1::shared_ptr<MapDesc> > mMapDescs;
};

#endif
