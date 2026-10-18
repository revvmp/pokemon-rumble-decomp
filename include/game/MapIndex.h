#ifndef GAME_MAPINDEX_H
#define GAME_MAPINDEX_H

struct MapIndex {
    int mMode : 8;
    int mArea : 8;
    int mStage : 8;
    int mMap : 8;
};

#endif
