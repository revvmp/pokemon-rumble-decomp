#ifndef GAME_CPIIBOX_H
#define GAME_CPIIBOX_H

#include <revolution/types.h>
#include <list>
#include <memory>
#include <game/SaveData.h>
#include <vector>

class CPiiPersonalData;

class CPiiBox {
public:
    CPiiBox(int type, int capacity);
    ~CPiiBox();
    void ExportPersonalData(std::vector<SD_PiiPersonalData::Data>& rOut);
    std::list<std::tr1::shared_ptr<CPiiPersonalData> >& GetPersonalDataList() { return mPersonalDataList; }

private:
    int mType;
    int mCapacity;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPersonalDataList;
};

#endif
