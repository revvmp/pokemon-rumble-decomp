#ifndef GAME_CPIIBOX_H
#define GAME_CPIIBOX_H

#include <revolution/types.h>
#include <list>
#include <memory>
#include <game/SaveData.h>
#include <vector>

class CPiiPersonalData;
class COyaInfo;

class CPiiBox {
public:
    CPiiBox(int type, int capacity);
    ~CPiiBox();
    static std::tr1::shared_ptr<CPiiBox> Create(const std::vector<SD_PiiPersonalData::Data>& rData, int type, int capacity, const COyaInfo* pOyaInfo);
    void AddPersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd);
    void RemovePersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd);
    void ExportPersonalData(std::vector<SD_PiiPersonalData::Data>& rOut);
    bool Add(std::tr1::shared_ptr<CPiiPersonalData> personalData);
    void Clear();
    void RestoreAll();
    u32 GetCount() const { return mPersonalDataList.size(); }
    int GetCapacity() const { return mCapacity; }
    std::list<std::tr1::shared_ptr<CPiiPersonalData> >& GetPersonalDataList() { return mPersonalDataList; }

private:
    int mType;
    int mCapacity;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPersonalDataList;
};

#endif
