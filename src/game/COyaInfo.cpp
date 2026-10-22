#include <game/COyaInfo.h>
#include <game/CPiiPersonalData.h>
#include <game/Random.h>

COyaInfo::COyaInfo() : mNextId(1), mCount(0) {
    mPersonalData.resize(511);
    SetCount(0);
}

COyaInfo::~COyaInfo() {}

void COyaInfo::SetCount(u32 count) {
    mCount = count;
    while (mCount < 0x100) {
        mCount = Rand32();
    }
}

std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const PiiSpeciesData* pSpeciesData, u32 sex) {
    std::tr1::shared_ptr<CPiiPersonalData> personalData(new CPiiPersonalData(pOyaInfo->GetNextId(), pOyaInfo->GetCount(), pSpeciesData, sex));
    return const_cast<COyaInfo*>(pOyaInfo)->RegisterPersonalData(personalData);
}

std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const SD_PiiPersonalData::Data& rData) {
    std::tr1::shared_ptr<CPiiPersonalData> personalData = CPiiPersonalData::Import(pOyaInfo->GetNextId(), rData);
    if (personalData->GetOriginalTrainerId() == 0) {
        personalData->SetOriginalTrainerId(pOyaInfo->GetCount());
    }
    return const_cast<COyaInfo*>(pOyaInfo)->RegisterPersonalData(personalData);
}

std::tr1::shared_ptr<CPiiPersonalData> COyaInfo::FindPersonalData(u32 key) const {
    int id = key;
    if (id < 1) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    const PersonalDataMap& rBucket = mPersonalData[key % mPersonalData.size()];
    if (rBucket.empty()) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    PersonalDataMap::iterator it = rBucket.find(id);
    if (it == rBucket.end()) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    return it->second.lock();
}

void COyaInfo::ApplyEnvironment(bool enable, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) {
    for (std::vector<PersonalDataMap>::iterator bucket = mPersonalData.begin(); bucket != mPersonalData.end(); ++bucket) {
        for (PersonalDataMap::iterator it = bucket->begin(); it != bucket->end(); ++it) {
            std::tr1::shared_ptr<CPiiPersonalData> personalData = it->second.lock();
            if (personalData) {
                personalData->ExecuteInitScript(enable, rEnvironment);
            }
        }
    }
}

std::tr1::shared_ptr<CPiiPersonalData> COyaInfo::RegisterPersonalData(std::tr1::shared_ptr<CPiiPersonalData> pData) {
    if (!pData) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    PersonalDataMap entries;
    PersonalDataMap& rBucket = mPersonalData[pData->GetId() % mPersonalData.size()];
    for (PersonalDataMap::iterator it = rBucket.begin(); it != rBucket.end(); ++it) {
        std::tr1::shared_ptr<CPiiPersonalData> personalData = it->second.lock();
        if (personalData) {
            std::tr1::weak_ptr<CPiiPersonalData> entry(personalData);
            entries.insert(PersonalDataMap::value_type(personalData->GetId(), entry));
        }
    }
    std::tr1::weak_ptr<CPiiPersonalData> entry(pData);
    entries.insert(PersonalDataMap::value_type(pData->GetId(), entry));
    rBucket.swap(entries);
    ++mNextId;
    return pData;
}
