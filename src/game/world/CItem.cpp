#include <game/world/CItem.h>
#include <game/core/CFiber.h>
#include <game/graphics/CModel.h>
#include <game/script/CPpuEngine.h>
#include <game/io/FileList.h>
#include <game/graphics/VideoTiming.h>
#include <string>

CItem::CItem(u32 type) : BModelObj(type | TYPE), mFlags(0) {}

CItem::~CItem() {}

void CItem::Initialize(bool background) {}

bool CItem::LoadScript(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment) {
    mPpuEngine.reset(new CPpuEngine);
    return mPpuEngine->Load(this, pName, mode, rEnvironment);
}

void CItem::LoadModel(const char* pName, bool background) {
    std::tr1::shared_ptr<CModel> model(new CModel);
    std::string path("item.arc/");
    path.replace(path.size(), 0, pName);
    void* pEntry;
    std::tr1::shared_ptr<FileList> files = LoadFileList(&pEntry, path.c_str(), 0, background, nullptr);
    CModel::LoadParams params = {static_cast<const char**>(pEntry), false, false, 128, 2, nullptr};
    if (!model->Load(params, background)) {
        TerminateFiber(1);
        return;
    }
    ChangeModel(model, 2);
    gFrameTimingReset = true;
}

void CItem::Calc() {
    BModelObj::Calc();
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
}

void CItem::OnLeaveWorld(BWorld* pWorld) {
    BModelObj::OnLeaveWorld(pWorld);
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}
