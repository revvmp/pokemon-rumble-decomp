#ifndef GAME_STAGEDESC_H
#define GAME_STAGEDESC_H

#include <game/MapIndex.h>
#include <memory>
#include <vector>

struct MapDesc;
struct DescParams;

class DescParamTable {
public:
    DescParamTable();
    ~DescParamTable();
    void Reset();

private:
    std::tr1::shared_ptr<DescParams> mParams;
};

struct StageDesc {
    StageDesc() {}
    void Init(MapIndex index);

    MapIndex mIndex;
    DescParamTable mParamTable;
    std::vector<std::tr1::shared_ptr<MapDesc> > mMapDescs;
};

#endif
