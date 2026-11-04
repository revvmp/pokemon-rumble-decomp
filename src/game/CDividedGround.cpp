#include <memory>
#include <string>
#include <game/BWorldObj.h>
#include <game/CDividedGround.h>
#include <game/ScnMdlSimpleDataInline.h>
#include <game/BWorld.h>
#include <game/CBSPTree.h>
#include <game/CFiber.h>
#include <game/CFileData.h>
#include <game/AnmObjChrRootScale.h>
#include <game/CModel.h>
#include <game/CPpuEngine.h>
#include <game/FileList.h>
#include <game/Memory.h>
#include <game/Random.h>
#include <game/StringFormat.h>
#include <game/VideoTiming.h>
#include <map>
#include <nw4r/g3d/g3d_scngroupex.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <stdio.h>
#include <string.h>

namespace {

template <typename T> inline const void* OffsetToPtr(const T* pBase, s32 offset) {
    return reinterpret_cast<const void*>(offset + reinterpret_cast<s32>(pBase));
}

inline u32 GetNodeCount(nw4r::g3d::ResAnmChr& rAnm) {
    return rAnm.ofs_to_obj<nw4r::g3d::ResDic>(rAnm.ref().toChrDataDic).GetNumData();
}

}

CDividedGround::CDividedGround(u32 type) : BModelObj(type | TYPE) {}

CDividedGround::~CDividedGround() {}

inline std::string FormatString(const char* pFormat, ...) {
    std::string result;
    va_list args;
    va_start(args, pFormat);
    VFormatString(result, pFormat, args);
    va_end(args);
    return result;
}

bool CDividedGround::LoadPkc(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv, const char* pPrefix) {
    mPpuEngine.reset(new CPpuEngine);

    pPrefix = pPrefix ? pPrefix : "";

    int stageNo = mStageNo;
    bool probe = false;
    if (probe)
        FormatString("");
    std::string name;
    name = FormatString("%s%s_%s%02d.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName,
                        gStageRanks[mStageID->mRank].mName, stageNo);
    std::tr1::shared_ptr<CFileData> file = CPpuEngine::LoadScriptFile(name.c_str(), mode);
    if (file) {
        return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
    }

    name = FormatString("%s%s.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName);
    file = CPpuEngine::LoadScriptFile(name.c_str(), mode);
    if (file) {
        return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
    }

    name = FormatString("%s%s_%02d.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName, stageNo);
    return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
}

bool CDividedGround::Load(const StageID* pStageID, int stageNo, bool background) {
    LoadFileLists(pStageID, stageNo, background);
    LoadAttr(pStageID, stageNo, nullptr, background);

    std::string rankName(gStageRanks[pStageID->mRank].mName);
    std::string number = FormatString("%02d", stageNo);
    std::tr1::shared_ptr<CModel> model(new CModel);
    std::string modelName = "ground_" + rankName + number;

    std::list<std::tr1::shared_ptr<CFileData> >::iterator it = mFiles.begin();
    for (; it != mFiles.end(); ++it) {
        nw4r::g3d::ResFile resFile((*it)->GetData());
        nw4r::g3d::ResMdl resMdl = resFile.GetResMdl(modelName.c_str());
        if (resMdl.IsValid()) {
            goto found;
        }
    }
    modelName = "ground_" + number;

found:

    CModel::LoadParams params = {nullptr, false, false, 0xD15, 1, nullptr};
    params.mpFileNames = &mFileNames[0];
    params.mpModelName = modelName.c_str();
    if (!model->Load(params, background)) {
        TerminateFiber(1);
        return false;
    }

    model->GetScnObj()->SetPriorityDrawXlu(1);
    BuildScene(pStageID, stageNo, background);
    ChangeModel(model, 1);
    mStageID = pStageID;
    mStageNo = stageNo;
    GetModel()->Play(0, 0.0f, 1.0f);

    AttachToWorld();

    gFrameTimingReset = true;
    return true;
}

void CDividedGround::LoadFileLists(const StageID* pStageID, int stageNo, bool background) {
    std::string worldName(gStageWorlds[pStageID->mWorld].mName);
    std::string number = FormatString("%02d", stageNo);
    std::tr1::shared_ptr<FileList> fileLists[2];
    void* entries[2];

    fileLists[0] = LoadResourceFileList(&entries[0], ("stage/" + worldName + "/" + number + ".arc/stage.brres").c_str(),
                                        0, background, nullptr);
    fileLists[1] =
        LoadResourceFileList(&entries[1], ("stage/" + worldName + "/common.arc/stage.brres").c_str(), 0, background, nullptr);

    int nameCount = 0;
    for (int i = 0; i < 2; ++i) {
        if (fileLists[i]) {
            const char** pNames = static_cast<const char**>(entries[i]);
            while (*pNames != nullptr) {
                ++nameCount;
                ++pNames;
            }
        }
    }

    mFileNames.reserve(nameCount + 1);
    for (int i = 0; i < 2; ++i) {
        if (fileLists[i]) {
            for (int j = 0; static_cast<const char**>(entries[i])[j] != nullptr; ++j) {
                mFileNames.push_back(static_cast<const char**>(entries[i])[j]);
            }
            mFiles.insert(mFiles.end(), fileLists[i]->begin(), fileLists[i]->end());
        }
    }
    mFileNames.push_back(nullptr);
}

bool CDividedGround::LoadAttr(const char* pName, bool background) {
    return LoadAttr(mStageID, mStageNo, pName, background);
}

bool CDividedGround::LoadAttr(const StageID* pStageID, int stageNo, const char* pName, bool background) {
    std::string worldName(gStageWorlds[pStageID->mWorld].mName);
    std::string rankName(gStageRanks[pStageID->mRank].mName);
    std::string number = FormatString("%02d", stageNo);
    std::string archivePath = "5:/stage/" + worldName + "/" + number + ".arc";

    for (int i = 0; i < BSP_COUNT; ++i) {
        std::tr1::shared_ptr<CBSPTree> tree(new CBSPTree);
        std::string prefix;
        if (i != 1) {
            prefix = "attr_";
        } else {
            prefix = "attrk_";
        }
        if (pName != nullptr && *pName) {
            prefix.append(pName);
            prefix.append("_");
        }

        std::string name = prefix + rankName + number;
        std::tr1::shared_ptr<CFileData> file =
            LoadResourceFile((archivePath + "/" + name + ".bsp").c_str(), 0, background);
        if (!(file && file->IsValid())) {
            name = prefix + number;
            file = LoadResourceFile((archivePath + "/" + name + ".bsp").c_str(), 0, background);
        }
        if (!(file && file->IsValid())) {
            if (i == 0) {
                return false;
            }
            continue;
        }

        tree->SetData(file->GetData());
        mBSPFiles[i] = file;
        mBSPTrees[i] = tree;
    }
    return true;
}

const char* CDividedGround::FindIndexSuffix(const char* pName) {
    size_t length = strlen(pName);
    if (length == 0) {
        return pName;
    }

    const char* pEnd = pName + length;
    const char* p = pEnd - 1;
    while ('0' <= *p && *p <= '9' && pName < p) {
        --p;
    }

    if (*p == '_') {
        return p;
    }

    return pEnd;
}

bool IsLayerFogName(const std::string& rName) {
    size_t length = rName.size();
    if (length < 8) {
        return false;
    }
    if (strcmp(rName.c_str() + length - 4, "_fog") != 0) {
        return false;
    }
    if (strncmp("layer_", rName.c_str(), 6) != 0 && strncmp("pale_", rName.c_str(), 5) != 0) {
        return false;
    }
    return true;
}

void CDividedGround::BuildScene(const StageID* pStageID, int stageNo, bool background) {
    std::map<std::string, std::list<nw4r::math::MTX34> > layers;
    std::string animName;
    if (pStageID->mKind == 2) {
        animName = FormatString("rep_EX%02d", stageNo);
    } else {
        animName = FormatString("rep_%s%02d", gStageRanks[pStageID->mRank].mName, stageNo);
    }

    nw4r::g3d::ResAnmChr anm(nullptr);
    std::list<std::tr1::shared_ptr<CFileData> >::iterator it = mFiles.begin();
    for (; it != mFiles.end(); ++it) {
        nw4r::g3d::ResFile resFile((*it)->GetData());
        anm = resFile.GetResAnmChr(animName.c_str());
        if (anm.IsValid()) {
            goto found;
        }
    }
    return;

found:

    u32 nodeCount = GetNodeCount(anm);
    for (u32 i = 0; i < nodeCount; ++i) {
        const nw4r::g3d::ResAnmChrNodeData* pNode = anm.GetNodeAnm(i);
        nw4r::g3d::ResName nodeName(OffsetToPtr(pNode, pNode->name - 4));
        if (strncmp(nodeName.GetName(), "REP_", 4) == 0 || strncmp(nodeName.GetName(), "rep_", 4) == 0) {
            const char* pBase = nodeName.GetName() + 4;
            std::string key(pBase, FindIndexSuffix(pBase));
            nw4r::g3d::ChrAnmResult result;
            anm.GetAnmResult(&result, i, 0.0f);
            nw4r::math::MTX34 mtx;
            result.GetMtx(&mtx);
            layers[key].push_back(mtx);
        }
    }

    mScnGroup.reset(nw4r::g3d::ScnGroup::Construct(GetDefaultAllocator(), nullptr, layers.size() + 1), DestroyG3dObj);

    std::map<std::string, std::list<nw4r::math::MTX34> >::iterator layer = layers.begin();
    for (; layer != layers.end(); ++layer) {
        bool fog = IsLayerFogName(layer->first);
        int priority = 0x65;
        if (fog) {
            priority = 0x66;
        }
        char name[100];
        snprintf(name, sizeof(name), "rep_%s", layer->first.c_str());

        std::tr1::shared_ptr<CModel> model(new CModel);
        CModel::LoadParams params = {nullptr, false, false, 0, 1, nullptr};
        params.mpFileNames = &mFileNames[0];
        params.mpModelName = name;
        if (!model->Load(params, background)) {
            continue;
        }

        int motionLength = model->GetMotionLength(name);
        if (motionLength > 0) {
            nw4r::g3d::ResFile resFile = model->GetResFile();
            if (resFile.GetResAnmVis(name).IsValid()) {
                params.mOptions |= 0x40;
            }
            if (resFile.GetResAnmClr(name).IsValid()) {
                params.mOptions |= 0x108;
            }
            if (resFile.GetResAnmTexPat(name).IsValid()) {
                params.mOptions |= 0x3;
            }
            if (resFile.GetResAnmTexSrt(name).IsValid()) {
                params.mOptions |= 0x204;
            }
            if (resFile.GetResAnmShp(name).IsValid()) {
                params.mOptions |= 0x7000;
            }
            params.mSimple = true;
        }

        std::list<nw4r::math::MTX34>& rMatrices = layer->second;
        std::tr1::shared_ptr<nw4r::g3d::ScnGroupEx> group(
            nw4r::g3d::ScnGroupEx::Construct(GetDefaultAllocator(), nullptr, rMatrices.size(), rMatrices.size(),
                                             model->GetResMdl()),
            DestroyG3dObj);
        mScnGroups.push_back(group);
        bool modelProbe = false;
        if (modelProbe) static_cast<ScnMdlSimpleData*>(nullptr)->GetModel();
        mScnGroup->PushBack(group.get());
        group->SetPriorityDrawXlu(priority);

        if (motionLength > 0) {
            std::list<nw4r::math::MTX34>::iterator matrix = rMatrices.begin();
            for (; matrix != rMatrices.end(); ++matrix) {
                std::tr1::shared_ptr<CModel> instance(new CModel);
                instance->Load(params, background);
                instance->GetScnObj()->SetPriorityDrawXlu(priority);
                instance->Play(name, static_cast<f32>(RandInt(0, motionLength)), 1.0f);
                mModels.push_back(instance);
                group->PushBack(instance->GetScnObj());
                instance->GetScnObj()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, *matrix);
            }
        } else {
            CModel::LoadParams simple = {nullptr, true, false, 0, 1, nullptr};
            simple.mpFileNames = &mFileNames[0];
            simple.mpModelName = name;
            std::list<nw4r::math::MTX34>::iterator matrix = rMatrices.begin();
            for (; matrix != rMatrices.end(); ++matrix) {
                std::tr1::shared_ptr<ScnMdlSimpleData> data(new ScnMdlSimpleData);
                LoadScnMdlSimpleData(data.get(), simple, 0, background);
                data->GetModel()->SetPriorityDrawXlu(priority);
                mSimpleModels.push_back(data);
                const ScnMdlSimpleData* pData = data.operator->();
                group->PushBack(pData->GetModel());
                data->GetModel()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, *matrix);
            }
        }
    }
}

bool CDividedGround::IsOutOfBounds(const nw4r::math::VEC3& rPos) {
    if (!mBSPTrees[0]) {
        return false;
    }

    CBSPTree::Trace trace;
    trace.mStart.x = rPos.x;
    trace.mStart.y = 1.0f;
    trace.mStart.z = rPos.z;
    trace.mDirection.x = 0.0f;
    trace.mDirection.y = -1.0f;
    trace.mDirection.z = 0.0f;
    return !mBSPTrees[0]->TraceSegment(trace, 0.0f, 2.0f);
}

void CDividedGround::OnEnterWorld() {
    BModelObj::OnEnterWorld();
    AttachToWorld();
}

void CDividedGround::OnLeaveWorld(BWorld* pWorld) {
    if (mScnGroup) {
        mScnGroup->DetachFromParent();
    }
    BModelObj::OnLeaveWorld(pWorld);
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}

void CDividedGround::Calc() {
    BModelObj::Calc();
    std::list<std::tr1::shared_ptr<CModel> >::iterator it = mModels.begin();
    for (; it != mModels.end(); ++it) {
        (*it)->UpdateFrame(1.0f);
    }
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
}

inline void CDividedGround::AttachToWorld() {
    BWorld* pWorld = GetWorld();
    if (pWorld != nullptr && mScnGroup) {
        GetModel()->GetScnObj()->DetachFromParent();
        mScnGroup->PushBack(GetModel()->GetScnObj());
        pWorld->GetModelGroup()->PushBack(mScnGroup.get());
    }
}
