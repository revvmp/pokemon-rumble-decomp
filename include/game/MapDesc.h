#ifndef GAME_MAPDESC_H
#define GAME_MAPDESC_H

#include <game/CPpuRegister.h>
#include <game/MapIndex.h>
#include <revolution/types.h>
#include <game/NPpuVM.h>
#include <set>
#include <string>
#include <vector>

namespace NPpu {
class CValue;
}

class MapPiiTable {
public:
    MapPiiTable();
    ~MapPiiTable();
    int GetRowCount() const { return mRowCount; }
    NPpu::CValue* GetCell(int row, int column);

private:
    std::vector<NPpu::CValue> mValues;
    std::set<std::string> mStrings;
    std::set<std::wstring> mWideStrings;
    int mRowCount;
    int mColumnCount;
};

struct MapDesc {
    MapIndex mIndex;
    CPpuRegister mParams;
    MapPiiTable mPiiTable;
};

#endif
