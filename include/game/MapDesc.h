#ifndef GAME_MAPDESC_H
#define GAME_MAPDESC_H

#include <game/CPpuRegister.h>
#include <game/MapIndex.h>
#include <revolution/types.h>

namespace NPpu {
class CValue;
}

class MapPiiTable {
public:
    int GetRowCount() const { return mRowCount; }
    NPpu::CValue* GetCell(int row, int column);

private:
    NPpu::CValue* mCells;
    u32 mReserved[8];
    int mRowCount;
    int mColumnCount;
};

struct MapDesc {
    MapIndex mIndex;
    CPpuRegister mParams;
    MapPiiTable mPiiTable;
};

#endif
