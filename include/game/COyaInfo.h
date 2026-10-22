#ifndef GAME_COYAINFO_H
#define GAME_COYAINFO_H

#include <memory>
#include <revolution/types.h>

namespace NPpu {
class CEnvironment;
}

class CPiiPersonalData;

class COyaInfo {
public:
    COyaInfo();
    ~COyaInfo();

    void SetCount(u32 count);
    void ApplyEnvironment(bool enable, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
    u32 GetCount() const { return mCount; }
    std::tr1::shared_ptr<CPiiPersonalData> FindPersonalData(u32 key) const;

private:
    u32 mStorage[4];
    u32 mCount;
};

#endif
