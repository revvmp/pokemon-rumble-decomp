#include <game/CPpuEngine.h>
#include <game/BWorld.h>
#include <game/BWorldObj.h>

void YieldFiber();

std::tr1::weak_ptr<NPpu::CEnvironment> CPpuEngine::sDefaultEnv;
StageID CPpuEngine::sDefaultStageDescTarget;
std::tr1::shared_ptr<std::string> CPpuEngine::sScriptRoot;

static bool sTemporaryIDReady;

static void InitTemporaryID() {
    if (!sTemporaryIDReady) {
        sTemporaryIDReady = true;
        NPpu::CEngine::SetNextID(CPpuEngine::TEMPORARY_ID_BASE);
    }
}

CPpuEngine::CPpuEngine() : mFlags(0), mOwnerObj(nullptr), mStageDescTarget(sDefaultStageDescTarget), mCachedPPDValue(0) {}

CPpuEngine::~CPpuEngine() {}

NPpu::CEnvironment* CPpuEngine::GetEnv() const {
    return GetEnvironment();
}

void CPpuEngine::SetDefaultEnvironment(const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv) {
    sDefaultEnv = rEnv;
}

void CPpuEngine::SetStageDescTarget(const StageID& rTarget) {
    mStageDescTarget = rTarget;
}

void CPpuEngine::SetDefaultStageDescTarget(const StageID& rTarget) {
    sDefaultStageDescTarget = rTarget;
}

void CPpuEngine::SetPPD(std::tr1::shared_ptr<CPPD> ppd) {
    mPPD = ppd;
}

void CPpuEngine::LeaveInitPart() {
    mFlags |= FLAG_LEFT_INIT_PART;
}

bool CPpuEngine::Load(BWorldObj* pOwner, const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv) {
    SetID(pOwner->GetId());
    if (!Load(pName, mode, rEnv)) {
        return false;
    }
    mOwnerObj = pOwner;
    NPpu::CValue owner;
    owner.mInt = pOwner->GetId();
    owner.mType = NPpu::CValue::TYPE_INT;
    SetThis(owner);
    Calc();
    return true;
}

bool CPpuEngine::Execute(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv) {
    InitTemporaryID();
    if (!Load(pName, mode, rEnv)) {
        return false;
    }
    while (true) {
        Calc();
        switch (GetState()) {
        case STATE_NONE:
        case STATE_IDLE:
        case STATE_END:
            return false;
        case STATE_WAIT:
            if (mode == 1) {
                YieldFiber();
            }
            break;
        case STATE_READY:
            return true;
        }
    }
}

void CPpuEngine::Calc() {
    if (GetState() == STATE_NONE) {
        return;
    }
    while (true) {
        switch (Run()) {
        case STATE_WAIT:
            if (mFlags & FLAG_LEFT_INIT_PART) {
                return;
            }
            break;
        case STATE_NONE:
        case STATE_IDLE:
        case STATE_READY:
        case STATE_END:
            if (mOwnerObj != nullptr) {
                std::tr1::shared_ptr<BWorldObj> owner(mOwnerObj->shared_from_this());
                BWorld* pWorld = mOwnerObj->GetWorld();
                if (pWorld != nullptr) {
                    pWorld->RemoveObject(owner);
                }
                mOwnerObj = nullptr;
            }
            return;
        }
    }
}
