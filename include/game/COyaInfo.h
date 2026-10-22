#ifndef GAME_COYAINFO_H
#define GAME_COYAINFO_H

#include <memory>
#include <vector>
#include <map>
#include <revolution/types.h>

namespace NPpu {
class CEnvironment;
}

class CPiiPersonalData;

class COyaInfo {
public:
    typedef std::map<int, std::tr1::weak_ptr<CPiiPersonalData> > PersonalDataMap;

    COyaInfo();
    ~COyaInfo();

    void SetCount(u32 count);
    void ApplyEnvironment(bool enable, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
    u32 GetCount() const { return mCount; }
    int GetNextId() const { return mNextId; }
    std::tr1::shared_ptr<CPiiPersonalData> FindPersonalData(u32 key) const;
    std::tr1::shared_ptr<CPiiPersonalData> RegisterPersonalData(std::tr1::shared_ptr<CPiiPersonalData> pData);

private:
    std::vector<PersonalDataMap> mPersonalData;
    int mNextId;
    u32 mCount;
};

#endif
