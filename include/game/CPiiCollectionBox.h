#ifndef GAME_CPIICOLLECTIONBOX_H
#define GAME_CPIICOLLECTIONBOX_H

#include <game/SaveData.h>
#include <vector>

class CPiiCollectionBox {
public:
    int GetCollectionType(u16 monsno);
    void ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut);
};

#endif
