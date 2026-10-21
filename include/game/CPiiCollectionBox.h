#ifndef GAME_CPIICOLLECTIONBOX_H
#define GAME_CPIICOLLECTIONBOX_H

#include <game/SaveData.h>
#include <vector>

class CPiiCollectionBox {
public:
    void ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut);
};

#endif
