#include <game/CSimpleGround.h>
#include <game/CFiber.h>
#include <game/CPpuEngine.h>
#include <game/FileList.h>
#include <game/VideoTiming.h>
#include <game/CModel.h>

CSimpleGround::CSimpleGround(u32 type) : BModelObj(type | TYPE), mReserved(0) {}

CSimpleGround::~CSimpleGround() {}

void CSimpleGround::Initialize(bool background) {}

void CSimpleGround::LoadScript(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment) {
    mPpuEngine.reset(new CPpuEngine);
    mPpuEngine->Load(this, pName, mode, rEnvironment);
}

void CSimpleGround::LoadModel(const char* pName, bool background) {
    std::tr1::shared_ptr<CModel> model(new CModel);

    void* pEntry;
    std::tr1::shared_ptr<FileList> fileList = LoadFileList(&pEntry, pName, 0, background, nullptr);

    CModel::LoadParams params = {nullptr, false, false, 0xD15, 1, nullptr};
    params.mpFileNames = static_cast<const char**>(pEntry);
    if (!model->Load(params, background)) {
        TerminateFiber(1);
        return;
    }

    model->GetScnObj()->SetPriorityDrawXlu(1);
    ChangeModel(model, 1);
    gFrameTimingReset = true;
}

void CSimpleGround::Calc() {
    BModelObj::Calc();
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
}

void CSimpleGround::OnLeaveWorld(BWorld* pWorld) {
    BModelObj::OnLeaveWorld(pWorld);
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}
