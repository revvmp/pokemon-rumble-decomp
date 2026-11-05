#ifndef GAME_CPIIBOX_H
#define GAME_CPIIBOX_H

#include <revolution/types.h>
#include <list>
#include <memory>
#include <game/SaveData.h>
#include <vector>

class CPpuEnv;

class CPiiPersonalData;
class COyaInfo;

namespace NPpu {
class CEnvironment;
}

class CPiiBox {
public:
    CPiiBox(int type, int capacity);
    ~CPiiBox();
    static std::tr1::shared_ptr<CPiiBox> Create(const std::vector<SD_PiiPersonalData::Data>& rData, int type, int capacity, const COyaInfo* pOyaInfo);
    bool AddPersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd);
    bool RemovePersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd);
    void ExportPersonalData(std::vector<SD_PiiPersonalData::Data>& rOut);
    void Clear();
    void RestoreAll();
    void ShowAllSpecs();
    void ApplyEnvironment(bool enable, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment);
    u32 GetCount() const { return mPersonalDataList.size(); }
    int GetCapacity() const { return mCapacity; }
    const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& GetPersonalDataList() const { return mPersonalDataList; }
#if defined(GAME_DLGPIISELECTOR_EMISSION_LAYOUT)
    __declspec(section ".text.selector_prefix")
#endif
    std::list<std::tr1::shared_ptr<CPiiPersonalData> >& GetPersonalDataList() { return mPersonalDataList; }

private:
    bool RemoveItem(std::tr1::shared_ptr<CPiiPersonalData> ppd);

    int mType;
    int mCapacity;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPersonalDataList;
};

#endif
