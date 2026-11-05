#define CBATTLEROYALBOX_EMISSION_LAYOUT
#include <memory>
class CBattleRoyalData;
namespace std { namespace tr1 { template<> shared_ptr<CBattleRoyalData>::shared_ptr(); } }
#include <game/session/CBattleRoyalBox.h>

f32 GetUnsetBattleRoyalTime() { return -1.0f; }

bool CBattleRoyalData::UpdateBestTime(f32 time) {
    if (mBestTime < 0.0f) {
        mBestTime = time;
        return true;
    }
    if (time < mBestTime) {
        mBestTime = time;
        return true;
    }
    return false;
}

std::tr1::shared_ptr<CBattleRoyalData> CreateBattleRoyalData(const f32& rTime) {
    return std::tr1::shared_ptr<CBattleRoyalData>(new CBattleRoyalData(rTime));
}

namespace std { namespace tr1 { template<> shared_ptr<CBattleRoyalData>::shared_ptr() : ptr_(nullptr), s_(nullptr) {} } }
CBattleRoyalBox::CBattleRoyalBox(const std::vector<SD_BattleRoyalData::Data>& rData) {
    std::vector<SD_BattleRoyalData::Data>& rList = const_cast<std::vector<SD_BattleRoyalData::Data>&>(rData);
    std::vector<SD_BattleRoyalData::Data>::iterator it = rList.begin();
    for (int mode = 0; mode < 3; ++mode) {
        for (int area = 0; area < 5; ++area) {
            if (it == rList.end()) {
                mData[mode][area].reset(new CBattleRoyalData());
            } else {
                mData[mode][area] = CreateBattleRoyalData(it->mValue);
                ++it;
            }
        }
    }
}

std::tr1::shared_ptr<CBattleRoyalData> CBattleRoyalBox::GetData(int mode, int area) const {
    return mData[mode][area];
}

void CBattleRoyalBox::ExportBattleRoyalData(std::vector<SD_BattleRoyalData::Data>& rOut) {
    std::vector<SD_BattleRoyalData::Data> times;
    times.reserve(15);
    for (int mode = 0; mode < 3; ++mode) {
        for (int area = 0; area < 5; ++area) {
            SD_BattleRoyalData::Data value;
            value.mValue = mData[mode][area]->GetBestTime();
            times.push_back(value);
        }
    }
    rOut.swap(times);
}

std::tr1::shared_ptr<CBattleRoyalBox> CBattleRoyalBox::Create(const std::vector<SD_BattleRoyalData::Data>& rData) {
    std::tr1::shared_ptr<CBattleRoyalBox> box(new CBattleRoyalBox(rData));
    return box;
}
