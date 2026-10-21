#include <game/CDividedGround.h>
#include <game/BWorld.h>
#include <game/CBSPTree.h>
#include <game/CFiber.h>
#include <game/CFileData.h>
#include <game/CModel.h>
#include <game/CPpuEngine.h>
#include <game/FileList.h>
#include <game/StringFormat.h>
#include <game/VideoTiming.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <string.h>

CDividedGround::CDividedGround(u32 type) : BModelObj(type | TYPE) {}

CDividedGround::~CDividedGround() {}

bool CDividedGround::LoadPkc(int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv, const char* pPrefix) {
    mPpuEngine.reset(new CPpuEngine);

    pPrefix = pPrefix ? pPrefix : "";

    int stageNo = mStageNo;
    std::string name;
    name.assign(FormatString("%s%s_%s%02d.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName,
                             gStageRanks[mStageID->mRank].mName, stageNo));
    std::tr1::shared_ptr<CFileData> file = CPpuEngine::LoadScriptFile(name.c_str(), mode);
    if (file) {
        return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
    }

    name.assign(FormatString("%s%s.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName));
    file = CPpuEngine::LoadScriptFile(name.c_str(), mode);
    if (file) {
        return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
    }

    name.assign(FormatString("%s%s_%02d.pkc", pPrefix, gStageWorlds[mStageID->mWorld].mName, stageNo));
    return mPpuEngine->Load(this, name.c_str(), mode, rEnv);
}

std::string FormatString(const char* pFormat, ...) {
    std::string result;
    va_list args;
    va_start(args, pFormat);
    VFormatString(result, pFormat, args);
    va_end(args);
    return result;
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
            break;
        }
    }
    if (it == mFiles.end()) {
        modelName.assign("ground_" + number);
    }

    CModel::LoadParams params = {nullptr, false, 0xD15, 1, nullptr};
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

    BWorld* pWorld = GetWorld();
    if (pWorld != nullptr && mScnGroup) {
        GetModel()->GetScnObj()->DetachFromParent();
        mScnGroup->PushBack(GetModel()->GetScnObj());
        pWorld->GetModelGroup()->PushBack(mScnGroup.get());
    }

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
        if (!file || !file->IsValid()) {
            name.assign(prefix + number);
            file = LoadResourceFile((archivePath + "/" + name + ".bsp").c_str(), 0, background);
            if (!file || !file->IsValid()) {
                if (i != 0) {
                    continue;
                }
                return false;
            }
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
