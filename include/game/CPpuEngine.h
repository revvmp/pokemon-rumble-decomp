#ifndef GAME_CPPUENGINE_H
#define GAME_CPPUENGINE_H

#include <game/ICalc.h>
#include <game/NPpuVM.h>
#include <game/StageID.h>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <nw4r/math/math_types.h>
#include <set>
#include <vector>

class BWorld;
class BWorldObj;
class BWorldObjOwner;
class CFileData;
class CPPD;

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
    static void SetDefaultStageDescTarget(const StageID& rTarget);
    static const char* GetScriptRoot();

    CPpuEngine();

    BWorld* GetWorld() const;
    NPpu::CEnvironment* GetEnv() const;
    std::tr1::shared_ptr<CFileData> LoadFile(const char* pName);
    bool LoadPlacement(const char* pFileName, const char* pAnmName, const char* pNodePrefix, u32 flags);
    void SetStageDescTarget(const StageID& rTarget);
    void SetPPD(std::tr1::shared_ptr<CPPD> ppd);
    void LeaveInitPart();
    static std::tr1::shared_ptr<CFileData> LoadScriptFile(const char* pName, u32 flags);
    bool Load(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    bool Load(BWorldObj* pOwner, const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    bool Execute(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv);
    BWorldObj* PopWorldObj();
    bool HasCallback(int messageID) const;
    int SendMessage(const Message& rMessage);

    const std::tr1::shared_ptr<BWorldObjOwner>& GetOwner() const { return mOwner; }

private:
    static std::tr1::weak_ptr<NPpu::CEnvironment> sDefaultEnv;
    static StageID sDefaultStageDescTarget;
    static std::tr1::shared_ptr<std::string> sScriptRoot;

    u32 mFlags;
    BWorldObj* mOwnerObj;
    std::map<int, std::list<int> > mCallbacks;
    std::tr1::shared_ptr<BWorldObjOwner> mOwner;
    StageID mStageDescTarget;
    std::set<std::tr1::shared_ptr<CFileData> > mFiles;
    std::vector<nw4r::math::MTX34> mPlacements;
    std::tr1::shared_ptr<CPPD> mPPD;
    int mCachedPPDValue;
    std::tr1::weak_ptr<CPPD> mCachedPPD;

public:
    virtual ~CPpuEngine();
    virtual void Calc();
};

bool RunScript(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);

#endif
