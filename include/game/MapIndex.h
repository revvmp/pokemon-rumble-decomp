#ifndef GAME_MAPINDEX_H
#define GAME_MAPINDEX_H

struct MapIndex {
    int mMode : 8;
    int mArea : 8;
    int mStage : 8;
    int mMap : 8;

    bool operator==(const MapIndex& rOther) const {
        return *reinterpret_cast<const unsigned int*>(this) == *reinterpret_cast<const unsigned int*>(&rOther);
    }
};

MapIndex MakeMapIndex(int mode, int area, int stage, int map);

#endif
