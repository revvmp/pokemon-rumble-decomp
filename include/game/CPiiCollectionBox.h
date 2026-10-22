#ifndef GAME_CPIICOLLECTIONBOX_H
#define GAME_CPIICOLLECTIONBOX_H

#include <game/SaveData.h>
#include <memory>
#include <vector>

class CPiiCollectionBox {
public:
    static std::tr1::shared_ptr<CPiiCollectionBox> Create(const std::vector<SD_PiiCollectionData::Data>& rData);
    int GetCollectionType(u16 monsno);
    void ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut);
};

#endif
