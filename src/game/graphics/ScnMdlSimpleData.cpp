#include <game/graphics/ScnMdlSimpleData.h>
#include <game/graphics/CModel.h>
#include <game/io/CFileData.h>
#include <game/animation/AnmObjChrRootScale.h>
#include <game/core/Memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <algorithm>

void DestroyG3dObj(nw4r::g3d::G3dObj* pObj) {
    pObj->Destroy();
}

namespace {
int sMinViewCount = 1;
}

bool ScnMdlSimpleData::Load(const char* pName, const char* pSubName, u32* pOptions, bool background) {
    u32 options = 0;
    const char* fileNames[3] = {pName, pSubName, nullptr};
    CModel::LoadParams params = {nullptr, false, false, 0, 1, nullptr};
    params.mpFileNames = fileNames;
    params.mSimple = pOptions == nullptr;
    if (pOptions) {
        options = *pOptions;
    }
    params.mOptions = options;
    return LoadScnMdlSimpleData(this, params, pOptions, background);
}

bool LoadScnMdlSimpleData(ScnMdlSimpleData* pData, const CModel::LoadParams& rParams, u32* pOutOptions, bool background) {
    u32 count = 0;
    while (rParams.mpFileNames[count]) {
        count++;
    }
    std::vector<std::tr1::shared_ptr<CFileData> > files;
    files.resize(count);
    u32 modelIndex = count;
    nw4r::g3d::ResMdl resMdl(nullptr);
    for (u32 i = 0; i < count; i++) {
        std::tr1::shared_ptr<CFileData> file;
        file = LoadResourceFile(rParams.mpFileNames[i], 0, background);
        if (!file || !file->IsLoaded()) {
            return false;
        }
        files[i] = file;
        if (modelIndex >= count) {
            nw4r::g3d::ResFile resFile(file->GetData());
            if (rParams.mpModelName) {
                resMdl = resFile.GetResMdl(rParams.mpModelName);
                if (resMdl.IsValid()) {
                    modelIndex = i;
                }
            } else if (resFile.HasResMdl()) {
                modelIndex = i;
            }
        }
    }
    if (modelIndex >= count) {
        return false;
    }
    nw4r::g3d::ResFile modelFile(files[modelIndex]->GetData());
    if (!modelFile.Bind()) {
        for (u32 i = 0; i < count; i++) {
            if (i != modelIndex) {
                nw4r::g3d::ResFile other(files[i]->GetData());
                if (modelFile.Bind(other)) {
                    goto modelBound;
                }
            }
        }
        return false;
    }
modelBound:
    if (!resMdl.IsValid()) {
        resMdl = modelFile.GetResMdl(0);
    }
    u32 options = 0;
    if (!rParams.mSimple) {
        options = rParams.mOptions;
        if (!rParams.mExplicitOptions) {
            for (u32 i = 0; i < count; i++) {
                nw4r::g3d::ResFile resFile(files[i]->GetData());
                if (resFile.HasResAnmVis()) {
                    options |= 0x40;
                }
                if (resFile.HasResAnmClr()) {
                    options |= 0x108;
                }
                if (resFile.HasResAnmTexPat()) {
                    options |= 0x3;
                }
                if (resFile.HasResAnmTexSrt()) {
                    options |= 0x204;
                }
                if (resFile.HasResAnmShp()) {
                    options |= 0x7000;
                }
            }
        }
    }
    std::tr1::shared_ptr<nw4r::g3d::ScnMdlSimple> model;
    int viewCount = std::max(rParams.mMode, sMinViewCount);
    if (options == 0) {
        model.reset(nw4r::g3d::ScnMdlSimple::Construct(GetDefaultAllocator(), nullptr, resMdl, viewCount), DestroyG3dObj);
    } else {
        model.reset(nw4r::g3d::ScnMdl::Construct(GetDefaultAllocator(), nullptr, resMdl, options, viewCount), DestroyG3dObj);
    }
    pData->mModel = model;
    pData->mFiles.swap(files);
    if (pOutOptions) {
        *pOutOptions = options;
    }
    return true;
}

void SetSceneObjectEnabled(nw4r::g3d::ScnObj* pScnObj, bool enable) {
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_GATHER_SCNOBJ, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_CALC_WORLD, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_CALC_MAT, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_CALC_VTX, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_CALC_VIEW, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_DRAW_OPA, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_DRAW_XLU, !enable);
    pScnObj->SetScnObjOption(nw4r::g3d::ScnObj::OPTION_DISABLE_UPDATEFRAME, !enable);
}
