#ifndef GAME_CPPUENGINE_H
#define GAME_CPPUENGINE_H

#include <game/ICalc.h>
#include <game/CPpuRegister.h>
#include <game/NPpuVM.h>
#include <game/StageID.h>
#include <game/MapIndex.h>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <nw4r/math/math_types.h>
#include <set>
#include <vector>

namespace Metrowerks {
template <> struct is_POD<nw4r::math::MTX34> {
    enum { value = true };
};
}

namespace std {
template <> struct __vec_bulk_move<nw4r::math::MTX34> {
    enum { value = false };
};

template <>
inline void __vec_constructor<nw4r::math::MTX34, allocator<nw4r::math::MTX34>, true>::reallocate(size_type n) {
    base buffer(this->alloc());
    if (n > this->max_size()) {
        this->throw_length_error();
    }
    buffer.data_ = buffer.alloc().allocate(n, 0);
    buffer.cap() = n;
    const nw4r::math::MTX34* pFirst = begin();
    const nw4r::math::MTX34* pLast = end();
    for (nw4r::math::MTX34* p = buffer.data_ + buffer.size_; pFirst < pLast; ++pFirst, ++p) {
        buffer.alloc().construct(p, *pFirst);
        ++buffer.size_;
    }
    clear_moved();
    std::swap(this->cap(), buffer.cap());
    std::swap(this->data_, buffer.data_);
    std::swap(this->size_, buffer.size_);
}
}

class BWorld;
class BWorldObj;
class CEffect;
class CFileData;
struct PiiSpeciesData;
class CPiiPersonalData;

class CPpuEngine : public NPpu::CEngine, public ICalc, public std::tr1::enable_shared_from_this<CPpuEngine> {
public:
    enum Flag {
        FLAG_LEFT_INIT_PART = 1 << 0
    };

    enum {
        FRAME_CAPACITY = 64,
        BASE_COUNT = 32,
        HANDLER_CAPACITY = 8,
        TEMPORARY_ID_BASE = 1000000000
    };

    struct Message {
        int mSender;
        int mMessageID;
        NPpu::CValue mArgument;
    };

    static void SetDefaultEnvironment(const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    static void SetDefaultStageDescTarget(const MapIndex& rTarget);
    static const char* GetScriptRoot();

    CPpuEngine();

    BWorld* GetWorld() const;
    NPpu::CEnvironment* GetEnv() const;
    std::tr1::shared_ptr<CFileData> LoadFile(const char* pName, bool background = false);
    bool LoadPlacement(const char* pFileName, const char* pAnmName, const char* pNodePrefix, bool background);
    void SetStageDescTarget(const MapIndex& rTarget);
    MapIndex GetStageDescTarget() const { return mStageDescTarget; }
    int InsertCallback(int messageID, int functionID, int position);
    int EraseCallback(int messageID, int functionID);
    void SetPPD(std::tr1::shared_ptr<CPiiPersonalData> ppd);
    void LeaveInitPart();
    static std::tr1::shared_ptr<CFileData> LoadScriptFile(const char* pName, u32 flags);
    bool Load(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    bool Load(BWorldObj* pOwner, const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    bool Execute(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    BWorldObj* PopWorldObj();
    CEffect* PopEffect();
    const PiiSpeciesData* PopSpeciesData();
    std::tr1::shared_ptr<CPiiPersonalData> PopPersonalData();
    u32 PeekWorldObjType(int argIndex);
    bool HasCallback(int messageID) const;
    int SendMessage(const Message& rMessage);

    u32 GetPlacementCount() const { return mPlacements.size(); }
    const nw4r::math::MTX34& GetPlacement(u32 index) const { return mPlacements[index]; }

    CPpuRegister* GetRegister() { return &mRegister; }
    BWorldObj* GetOwner() const { return mOwnerObj; }

private:
    static std::tr1::weak_ptr<NPpu::CEnvironment> sDefaultEnv;
    static MapIndex sDefaultStageDescTarget;
    static std::tr1::shared_ptr<std::string> sScriptRoot;

    u32 mFlags;
    BWorldObj* mOwnerObj;
    std::map<int, std::list<int> > mCallbacks;
    CPpuRegister mRegister;
    MapIndex mStageDescTarget;
    std::set<std::tr1::shared_ptr<CFileData> > mFiles;
    std::vector<nw4r::math::MTX34> mPlacements;
    std::tr1::shared_ptr<CPiiPersonalData> mPPD;
    int mCachedPPDValue;
    std::tr1::weak_ptr<CPiiPersonalData> mCachedPPD;

public:
    virtual ~CPpuEngine();
    virtual void Calc();
};

std::tr1::shared_ptr<NPpu::CEnvironment> CreateScriptEnvironment();
bool RunScript(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);

#endif
