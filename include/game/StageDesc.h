#ifndef GAME_STAGEDESC_H
#define GAME_STAGEDESC_H

#include <memory>
#include <vector>

struct MapDesc;

struct StageDesc {
    int mParams[3];
    std::vector<std::tr1::shared_ptr<MapDesc> > mMapDescs;
};

#endif
