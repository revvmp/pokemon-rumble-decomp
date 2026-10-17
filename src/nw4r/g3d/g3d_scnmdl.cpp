#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmshp.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_calcvtx.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/g3d_draw.h>
namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(ScnMdl);

ResTexObj ScnMdl::CopiedMatAccess::GetResTexObj(bool markDirty) {
    if (mpScnMdl != nullptr && mTexObj.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_TEXOBJ);
        }

        return mTexObj;
    }

    return ResTexObj(nullptr);
}

ResTlutObj ScnMdl::CopiedMatAccess::GetResTlutObj(bool markDirty) {
    if (mpScnMdl != nullptr && mTlutObj.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_TLUTOBJ);
        }
        return mTlutObj;
    }
    return ResTlutObj(nullptr);
}
ResTexSrt ScnMdl::CopiedMatAccess::GetResTexSrt(bool markDirty) {
    if (mpScnMdl != nullptr && mTexSrt.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_TEXSRT);
        }

        return mTexSrt;
    }

    return ResTexSrt(nullptr);
}



ResGenMode ScnMdl::CopiedMatAccess::GetResGenMode(bool markDirty) {
    if (mpScnMdl != nullptr && mGenMode.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_GENMODE);
        }

        return mGenMode;
    }

    return ResGenMode(nullptr);
}



ResMatPix ScnMdl::CopiedMatAccess::GetResMatPix(bool markDirty) {
    if (mpScnMdl != nullptr && mPix.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_MATPIX);
        }

        return mPix;
    }

    return ResMatPix(nullptr);
}

ResMatTevColor ScnMdl::CopiedMatAccess::GetResMatTevColor(bool markDirty) {
    if (mpScnMdl != nullptr && mTevColor.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_MATTEVCOLOR);
        }

        return mTevColor;
    }

    return ResMatTevColor(nullptr);
}

ResMatTexCoordGen ScnMdl::CopiedMatAccess::GetResMatTexCoordGen(bool markDirty) {
    if (mpScnMdl != nullptr && mTexCoordGen.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_MATTEXCOORDGEN);
        }
        return mTexCoordGen;
    }
    return ResMatTexCoordGen(nullptr);
}
ResTev ScnMdl::CopiedMatAccess::GetResTev(bool markDirty) {
    if (mpScnMdl != nullptr && mTev.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFOPTION_TEV);
        }

        return mTev;
    }

    return ResTev(nullptr);
}







ScnMdl::CopiedMatAccess::CopiedMatAccess(ScnMdl *pScnMdl, u32 id)
    : mTexObj(nullptr),
      mTlutObj(nullptr),
      mTexSrt(nullptr),
      mChan(nullptr),
      mGenMode(nullptr),
      mMatMisc(nullptr),
      mPix(nullptr),
      mTevColor(nullptr),
      mIndMtxAndScale(nullptr),
      mTexCoordGen(nullptr),
      mTev(nullptr) {
    if (pScnMdl != nullptr && pScnMdl->GetResMdl().GetResMat(id).IsValid()) {
        mpScnMdl = pScnMdl;
        mMatID = id;

        DrawResMdlReplacement &rReplacement = pScnMdl->mReplacement;

        if (rReplacement.texObjDataArray != nullptr) {
            mTexObj = ResTexObj(&rReplacement.texObjDataArray[id]);
        } else {
            mTexObj = ResTexObj(nullptr);
        }

        if (rReplacement.tlutObjDataArray != nullptr) {
            mTlutObj = ResTlutObj(&rReplacement.tlutObjDataArray[id]);
        } else {
            mTlutObj = ResTlutObj(nullptr);
        }

        if (rReplacement.texSrtDataArray != nullptr) {
            mTexSrt = ResTexSrt(&rReplacement.texSrtDataArray[id]);
        } else {
            mTexSrt = ResTexSrt(nullptr);
        }

        if (rReplacement.chanDataArray != nullptr) {
            mChan = ResMatChan(&rReplacement.chanDataArray[id]);
        } else {
            mChan = ResMatChan(nullptr);
        }

        if (rReplacement.genModeDataArray != nullptr) {
            mGenMode = ResGenMode(&rReplacement.genModeDataArray[id]);
        } else {
            mGenMode = ResGenMode(nullptr);
        }

        if (rReplacement.matMiscDataArray != nullptr) {
            mMatMisc = ResMatMisc(&rReplacement.matMiscDataArray[id]);
        } else {
            mMatMisc = ResMatMisc(nullptr);
        }

        if (rReplacement.pixDLArray != nullptr) {
            mPix = ResMatPix(&rReplacement.pixDLArray[id]);
        } else {
            mPix = ResMatPix(nullptr);
        }

        if (rReplacement.tevColorDLArray != nullptr) {
            mTevColor = ResMatTevColor(&rReplacement.tevColorDLArray[id]);
        } else {
            mTevColor = ResMatTevColor(nullptr);
        }

        if (rReplacement.indMtxAndScaleDLArray != nullptr) {
            mIndMtxAndScale = ResMatIndMtxAndScale(&rReplacement.indMtxAndScaleDLArray[id]);
        } else {
            mIndMtxAndScale = ResMatIndMtxAndScale(nullptr);
        }

        if (rReplacement.texCoordGenDLArray != nullptr) {
            mTexCoordGen = ResMatTexCoordGen(&rReplacement.texCoordGenDLArray[id]);
        } else {
            mTexCoordGen = ResMatTexCoordGen(nullptr);
        }

        if (rReplacement.tevDataArray != nullptr) {
            mTev = ResTev(&rReplacement.tevDataArray[id]);
        } else {
            mTev = ResTev(nullptr);
        }

    } else {
        mpScnMdl = nullptr;
        mMatID = id;

        mTexObj = ResTexObj(nullptr);
        mTlutObj = ResTlutObj(nullptr);
        mTexSrt = ResTexSrt(nullptr);
        mChan = ResMatChan(nullptr);
        mGenMode = ResGenMode(nullptr);
        mMatMisc = ResMatMisc(nullptr);
        mPix = ResMatPix(nullptr);
        mTevColor = ResMatTevColor(nullptr);
        mIndMtxAndScale = ResMatIndMtxAndScale(nullptr);
        mTexCoordGen = ResMatTexCoordGen(nullptr);
        mTev = ResTev(nullptr);
    }
}

bool ScnMdl::CopiedVisAccess::IsVisible() const {
    if (mpScnMdl != nullptr) {
        if (mpVis != nullptr) {
            return *mpVis != 0;
        }

        return mpScnMdl->GetResMdl().GetResNode(mNodeID).IsVisible();
    }

    return false;
}





ScnMdl::CopiedVisAccess::CopiedVisAccess(ScnMdl *pScnMdl, u32 id) {
    if (pScnMdl != nullptr && pScnMdl->GetResMdl().GetResNode(id).IsValid()) {
        mpScnMdl = pScnMdl;
        mNodeID = id;

        if (pScnMdl->mReplacement.visArray != nullptr) {
            mpVis = &pScnMdl->mReplacement.visArray[id];
        } else {
            mpVis = nullptr;
        }
    } else {
        mpScnMdl = nullptr;
        mNodeID = id;
        mpVis = nullptr;
    }
}





ScnMdl *ScnMdl::Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, u32 bufferOption, int numView) {
    if (!mdl.IsValid()) {
        return nullptr;
    }

    if (numView == 0) {
        numView = 1;
    } else if (numView > VIEW_MAX) {
        numView = VIEW_MAX;
    }

    ScnMdl *pScnMdl = nullptr;

    u32 worldMtxNum = mdl.GetResMdlInfo().GetNumPosNrmMtx();
    u32 viewMtxNum = mdl.GetResMdlInfo().GetNumViewMtx();
    u32 matNum = mdl.GetResMatNumEntries();
    u32 nodeNum = mdl.GetResNodeNumEntries();

    u32 scnMdlSize = sizeof(ScnMdl);
    u32 worldMtxArraySize = worldMtxNum * sizeof(math::MTX34);
    u32 worldMtxAttribArraySize = worldMtxNum * sizeof(u32);

    u32 viewPosMtxArrayUnitSize = viewMtxNum * sizeof(math::MTX34);
    u32 viewPosMtxArraySize = numView * align32(viewPosMtxArrayUnitSize);

    u32 viewNrmMtxArrayUnitSize = viewMtxNum * sizeof(math::MTX33);
    u32 viewNrmMtxArraySize =
        mdl.GetResMdlInfo().ref().need_nrm_mtx_array ? numView * align32(viewNrmMtxArrayUnitSize) : 0;

    u32 viewTexMtxArrayUnitSize = viewPosMtxArrayUnitSize;

    u32 viewTexMtxArraySize = mdl.ref().info.need_tex_mtx_array ? numView * align32(viewTexMtxArrayUnitSize) : 0;

    u32 matBufferDirtyFlagSize = matNum * sizeof(u32);

    u32 resTexObjSize         = (bufferOption & BUFOPTION_TEXOBJ)         ? matNum  * sizeof(ResTexObjData)        : 0;
    u32 resTlutObjSize        = (bufferOption & BUFOPTION_TLUTOBJ)        ? matNum  * sizeof(ResTlutObjData)       : 0;
    u32 resTexSrtSize         = (bufferOption & BUFOPTION_TEXSRT)         ? matNum  * sizeof(ResTexSrtData)        : 0;
    u32 resChanSize           = (bufferOption & BUFOPTION_MATCHAN)        ? matNum  * sizeof(ResChanData)          : 0;
    u32 resGenModeSize        = (bufferOption & BUFOPTION_GENMODE)        ? matNum  * sizeof(ResGenModeData)       : 0;
    u32 resMatMiscSize        = (bufferOption & BUFOPTION_MATMISC)        ? matNum  * sizeof(ResMatMiscData)       : 0;
    u32 visSize               = (bufferOption & BUFOPTION_VIS)            ? nodeNum * sizeof(u8)                   : 0;
    u32 resPixSize            = (bufferOption & BUFOPTION_MATPIX)         ? matNum  * sizeof(ResPixDL)             : 0;
    u32 resTevColorSize       = (bufferOption & BUFOPTION_MATTEVCOLOR)    ? matNum  * sizeof(ResTevColorDL)        : 0;
    u32 resIndMtxAndScaleSize = (bufferOption & BUFOPTION_MATINDMTXSCALE) ? matNum  * sizeof(ResIndMtxAndScaleDL)  : 0;
    u32 resTexCoordGenSize    = (bufferOption & BUFOPTION_MATTEXCOORDGEN) ? matNum  * sizeof(ResTexCoordGenDL)     : 0;
    u32 resTevSize            = (bufferOption & BUFOPTION_TEV)            ? matNum  * sizeof(ResTevData)           : 0;

    u32 vtxPosTableSize = 0;
    u32 vtxPosBufferSize = 0;

    if (bufferOption & BUFOPTION_VTXPOS) {
        u32 vtxPosNum = mdl.GetResVtxPosNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxPosTableSize = vtxPosNum * sizeof(ResVtxPosData *);

        for (u32 i = 0; i < vtxPosNum; i++) {
            u32 j;
            ResVtxPos pos = mdl.GetResVtxPos(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (pos.ptr() == shp.GetResVtxPos().ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                vtxPosBufferSize += align32(pos.GetSize());
            }
        }
    }

    u32 vtxNrmTableSize = 0;
    u32 vtxNrmBufferSize = 0;

    if (bufferOption & BUFOPTION_VTXNRM) {
        u32 vtxNrmNum = mdl.GetResVtxNrmNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxNrmTableSize = vtxNrmNum * sizeof(ResVtxNrmData *);

        for (u32 i = 0; i < vtxNrmNum; i++) {
            u32 j;
            ResVtxNrm nrm = mdl.GetResVtxNrm(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (nrm.ptr() == shp.GetResVtxNrm().ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                vtxNrmBufferSize += align32(nrm.GetSize());
            }
        }
    }

    u32 vtxClrTableSize = 0;
    u32 vtxClrBufferSize = 0;

    if (bufferOption & BUFOPTION_VTXCLR) {
        u32 vtxClrNum = mdl.GetResVtxClrNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxClrTableSize = vtxClrNum * sizeof(ResVtxClrData *);

        for (u32 i = 0; i < vtxClrNum; i++) {
            u32 j;
            ResVtxClr clr = mdl.GetResVtxClr(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (clr.ptr() == shp.GetResVtxClr(0).ptr() || clr.ptr() == shp.GetResVtxClr(1).ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                vtxClrBufferSize += align32(clr.GetSize());
            }
        }
    }

    u32 worldMtxArrayOfs       = align32(scnMdlSize);
    u32 worldMtxAttribArrayOfs = align32(worldMtxArrayOfs + worldMtxArraySize);
    u32 viewPosMtxArrayOfs     = align32(worldMtxAttribArrayOfs + worldMtxAttribArraySize);
    u32 viewNrmMtxArrayOfs     = align32(viewPosMtxArrayOfs + viewPosMtxArraySize);
    u32 viewTexMtxArrayOfs     = align32(viewNrmMtxArrayOfs + viewNrmMtxArraySize);
    u32 matBufferDirtyFlagOfs  = align4(viewTexMtxArrayOfs + viewTexMtxArraySize);
    u32 resTexObjOfs           = align32(matBufferDirtyFlagOfs + matBufferDirtyFlagSize);
    u32 resTlutObjOfs          = align4(resTexObjOfs + resTexObjSize);
    u32 resTexSrtOfs           = align4(resTlutObjOfs + resTlutObjSize);
    u32 resChanOfs             = align4(resTexSrtOfs + resTexSrtSize);
    u32 resGenModeOfs          = align4(resChanOfs + resChanSize);
    u32 resMatMiscOfs          = align4(resGenModeOfs + resGenModeSize);
    u32 visOfs                 = align4(resMatMiscOfs + resMatMiscSize);
    u32 resPixOfs              = align32(visOfs + visSize);
    u32 resTevColorOfs         = align32(resPixOfs + resPixSize);
    u32 resIndMtxAndScaleOfs   = align32(resTevColorOfs + resTevColorSize);
    u32 resTexCoordGenOfs      = align32(resIndMtxAndScaleOfs + resIndMtxAndScaleSize);
    u32 resTevOfs              = align32(resTexCoordGenOfs + resTexCoordGenSize);

    u32 vtxPosTableOfs = align32(resTevOfs + resTevSize);
    u32 vtxNrmTableOfs = vtxPosTableOfs + vtxPosTableSize;
    u32 vtxClrTableOfs = vtxNrmTableOfs + vtxNrmTableSize;

    u32 vtxPosBufferOfs = align32(vtxClrTableOfs + vtxClrTableSize);
    u32 vtxNrmBufferOfs = align32(vtxPosBufferOfs + vtxPosBufferSize);
    u32 vtxClrBufferOfs = align32(vtxNrmBufferOfs + vtxNrmBufferSize);

    u32 size = align32(vtxClrBufferOfs + vtxClrBufferSize);
    if (pSize != nullptr) {
        *pSize = size;
    }

    if (pAllocator != nullptr) {
        u8 *pBuffer = reinterpret_cast<u8 *>(Alloc(pAllocator, size));
        if (pBuffer == nullptr) {
            return nullptr;
        }
        DrawResMdlReplacement replacement;
        replacement.flag = 0;
        if (!(bufferOption & BUFOPTION_RESTORE_VTX_ON_DETACH)) {
            replacement.flag |= 1;
        }

        replacement.visArray              = visSize               != 0 ? reinterpret_cast<u8*>(pBuffer + visOfs)                                : nullptr;
        replacement.texObjDataArray       = resTexObjSize         != 0 ? reinterpret_cast<ResTexObjData*>(pBuffer + resTexObjOfs)               : nullptr;
        replacement.tlutObjDataArray      = resTlutObjSize        != 0 ? reinterpret_cast<ResTlutObjData*>(pBuffer + resTlutObjOfs)             : nullptr;
        replacement.texSrtDataArray       = resTexSrtSize         != 0 ? reinterpret_cast<ResTexSrtData*>(pBuffer + resTexSrtOfs)               : nullptr;
        replacement.chanDataArray         = resChanSize           != 0 ? reinterpret_cast<ResChanData*>(pBuffer + resChanOfs)                   : nullptr;
        replacement.genModeDataArray      = resGenModeSize        != 0 ? reinterpret_cast<ResGenModeData*>(pBuffer + resGenModeOfs)             : nullptr;
        replacement.matMiscDataArray      = resMatMiscSize        != 0 ? reinterpret_cast<ResMatMiscData*>(pBuffer + resMatMiscOfs)             : nullptr;
        replacement.pixDLArray            = resPixSize            != 0 ? reinterpret_cast<ResPixDL*>(pBuffer + resPixOfs)                       : nullptr;
        replacement.tevColorDLArray       = resTevColorSize       != 0 ? reinterpret_cast<ResTevColorDL*>(pBuffer + resTevColorOfs)             : nullptr;
        replacement.indMtxAndScaleDLArray = resIndMtxAndScaleSize != 0 ? reinterpret_cast<ResIndMtxAndScaleDL*>(pBuffer + resIndMtxAndScaleOfs) : nullptr;
        replacement.texCoordGenDLArray    = resTexCoordGenSize    != 0 ? reinterpret_cast<ResTexCoordGenDL*>(pBuffer + resTexCoordGenOfs)       : nullptr;
        replacement.tevDataArray          = resTevSize            != 0 ? reinterpret_cast<ResTevData*>(pBuffer + resTevOfs)                     : nullptr;

        replacement.vtxPosTable = vtxPosTableSize != 0 ? reinterpret_cast<ResVtxPosData**>(pBuffer + vtxPosTableOfs) : nullptr;
        replacement.vtxNrmTable = vtxNrmTableSize != 0 ? reinterpret_cast<ResVtxNrmData**>(pBuffer + vtxNrmTableOfs) : nullptr;
        replacement.vtxClrTable = vtxClrTableSize != 0 ? reinterpret_cast<ResVtxClrData**>(pBuffer + vtxClrTableOfs) : nullptr;

        if (replacement.vtxPosTable != nullptr) {
            u32 vtxPosNum = mdl.GetResVtxPosNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = vtxPosBufferOfs;

            for (u32 i = 0; i < vtxPosNum; i++) {
                u32 j;
                ResVtxPos pos = mdl.GetResVtxPos(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (pos.ptr() == shp.GetResVtxPos().ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    replacement.vtxPosTable[i] = reinterpret_cast<ResVtxPosData *>(pBuffer + ofs);

                    ofs += align32(pos.GetSize());
                    pos.CopyTo(replacement.vtxPosTable[i]);
                } else {
                    replacement.vtxPosTable[i] = pos.ptr();
                }
            }
        }

        if (replacement.vtxNrmTable != nullptr) {
            u32 vtxNrmNum = mdl.GetResVtxNrmNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = vtxNrmBufferOfs;

            for (u32 i = 0; i < vtxNrmNum; i++) {
                u32 j;
                ResVtxNrm nrm = mdl.GetResVtxNrm(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (nrm.ptr() == shp.GetResVtxNrm().ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    replacement.vtxNrmTable[i] = reinterpret_cast<ResVtxNrmData *>(pBuffer + ofs);

                    ofs += align32(nrm.GetSize());
                    nrm.CopyTo(replacement.vtxNrmTable[i]);
                } else {
                    replacement.vtxNrmTable[i] = nrm.ptr();
                }
            }
        }

        if (replacement.vtxClrTable != nullptr) {
            u32 vtxClrNum = mdl.GetResVtxClrNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = vtxClrBufferOfs;

            for (u32 i = 0; i < vtxClrNum; i++) {
                u32 j;
                ResVtxClr clr = mdl.GetResVtxClr(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (clr.ptr() == shp.GetResVtxClr(0).ptr() || clr.ptr() == shp.GetResVtxClr(1).ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    replacement.vtxClrTable[i] = reinterpret_cast<ResVtxClrData *>(pBuffer + ofs);

                    ofs += align32(clr.GetSize());
                    clr.CopyTo(replacement.vtxClrTable[i]);
                } else {
                    replacement.vtxClrTable[i] = clr.ptr();
                }
            }
        }
        u32 replacementFlag = 0;
        if (bufferOption & BUFOPTION_RESTORE_VTX_ON_DETACH) {
            replacementFlag |= 1;
        }

        pScnMdl = new (pBuffer) ScnMdl(
            pAllocator,
            mdl,
            reinterpret_cast<math::MTX34*>(pBuffer + worldMtxArrayOfs),
            reinterpret_cast<u32*>(pBuffer + worldMtxAttribArrayOfs),
            reinterpret_cast<math::MTX34*>(pBuffer + viewPosMtxArrayOfs),
            viewNrmMtxArraySize != 0 ? reinterpret_cast<math::MTX33*>(pBuffer + viewNrmMtxArrayOfs) : nullptr,
            viewTexMtxArraySize != 0 ? reinterpret_cast<math::MTX34*>(pBuffer + viewTexMtxArrayOfs) : nullptr,
            numView,
            viewMtxNum,
            &replacement,
            reinterpret_cast<u32*>(pBuffer + matBufferDirtyFlagOfs),
            replacementFlag);

        pScnMdl->InitBuffer();
    }

    return pScnMdl;
}

void ScnMdl::ScnMdl_G3DPROC_CALC_WORLD(u32 param, const math::MTX34* pParent) {
    ScnMdlSmpl_CalcPosture(param, pParent);

    if (IsVisBufferRefreshNeeded() && IsVisBufferDirty()) {
        CleanVisBuffer();
    }

    if (GetAnmObjVis() != nullptr) {
        if (mReplacement.visArray != nullptr) {
            ApplyVisAnmResult(mReplacement.visArray, GetResMdl(),
                              GetAnmObjVis());
            VisBufferDirty();
        } else {
            ApplyVisAnmResult(GetResMdl(), GetAnmObjVis());
        }
    }

    CheckCallback_CALC_WORLD(CALLBACK_TIMING_C, param,
                             const_cast<math::MTX34*>(pParent));
}

void ScnMdl::ScnMdl_G3DPROC_CALC_MAT(u32 param, void* pInfo) {
    CheckCallback_CALC_MAT(CALLBACK_TIMING_A, param, pInfo);

    ResMdl mdl = GetResMdl();
    u32 matNum = mdl.GetResMatNumEntries();

    for (u32 i = 0; i < matNum; i++) {
        ResMat mat = mdl.GetResMat(i);

        if (IsMatBufferDirty(i, BUFOPTION_TEXOBJ)) {
            CleanMatBuffer(i, BUFOPTION_TEXOBJ);
        }

        if (IsMatBufferDirty(i, BUFOPTION_TLUTOBJ)) {
            CleanMatBuffer(i, BUFOPTION_TLUTOBJ);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATINDMTXSCALE)) {
            CleanMatBuffer(i, BUFOPTION_MATINDMTXSCALE);
        }

        if (IsMatBufferDirty(i, BUFOPTION_TEXSRT)) {
            CleanMatBuffer(i, BUFOPTION_TEXSRT);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATCHAN)) {
            CleanMatBuffer(i, BUFOPTION_MATCHAN);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATTEVCOLOR)) {
            CleanMatBuffer(i, BUFOPTION_MATTEVCOLOR);
        }

        if (IsMatBufferDirty(i, BUFOPTION_GENMODE)) {
            CleanMatBuffer(i, BUFOPTION_GENMODE);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATMISC)) {
            CleanMatBuffer(i, BUFOPTION_MATMISC);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATPIX)) {
            CleanMatBuffer(i, BUFOPTION_MATPIX);
        }

        if (IsMatBufferDirty(i, BUFOPTION_MATTEXCOORDGEN)) {
            CleanMatBuffer(i, BUFOPTION_MATTEXCOORDGEN);
        }

        if (IsMatBufferDirty(i, BUFOPTION_TEV)) {
            CleanMatBuffer(i, BUFOPTION_TEV);
        }

        if (GetAnmObjTexPat() != nullptr && GetAnmObjTexPat()->TestExistence(i)) {
            TexPatAnmResult resultBuf;

            const TexPatAnmResult* pResult =
                GetAnmObjTexPat()->GetResult(&resultBuf, i);

            ResTexObj texObj = mReplacement.texObjDataArray != nullptr
                                   ? ResTexObj(&mReplacement.texObjDataArray[i])
                                   : mat.GetResTexObj();

            ResTlutObj tlutObj =
                mReplacement.tlutObjDataArray != nullptr
                    ? ResTlutObj(&mReplacement.tlutObjDataArray[i])
                    : mat.GetResTlutObj();

            ApplyTexPatAnmResult(texObj, tlutObj, pResult);

            texObj.EndEdit();
            tlutObj.EndEdit();

            MatBufferDirty(i, BUFOPTION_TEXOBJ | BUFOPTION_TLUTOBJ);
        }

        if (GetAnmObjTexSrt() != nullptr && GetAnmObjTexSrt()->TestExistence(i)) {
            TexSrtAnmResult resultBuf;

            const TexSrtAnmResult* pResult =
                GetAnmObjTexSrt()->GetResult(&resultBuf, i);

            ResTexSrt srt = mReplacement.texSrtDataArray != nullptr
                                ? ResTexSrt(&mReplacement.texSrtDataArray[i])
                                : mat.GetResTexSrt();

            ResMatIndMtxAndScale ind =
                mReplacement.indMtxAndScaleDLArray != nullptr
                    ? ResMatIndMtxAndScale(
                          &mReplacement.indMtxAndScaleDLArray[i])
                    : mat.GetResMatIndMtxAndScale();

            ApplyTexSrtAnmResult(srt, ind, pResult);

            ind.EndEdit();
            srt.EndEdit();

            MatBufferDirty(i, BUFOPTION_TEXSRT | BUFOPTION_MATINDMTXSCALE);
        }

        if (GetAnmObjMatClr() != nullptr && GetAnmObjMatClr()->TestExistence(i)) {
            ClrAnmResult resultBuf;

            ResMatTevColor tevColor =
                mReplacement.tevColorDLArray != nullptr
                    ? ResMatTevColor(&mReplacement.tevColorDLArray[i])
                    : mat.GetResMatTevColor();

            ResMatChan chan = mReplacement.chanDataArray != nullptr
                                  ? ResMatChan(&mReplacement.chanDataArray[i])
                                  : mat.GetResMatChan();

            const ClrAnmResult* pResult =
                GetAnmObjMatClr()->GetResult(&resultBuf, i);

            ApplyClrAnmResult(chan, tevColor, pResult);

            chan.EndEdit();
            tevColor.EndEdit();

            MatBufferDirty(i, BUFOPTION_MATTEVCOLOR | BUFOPTION_MATCHAN);
        }
    }

    CheckCallback_CALC_MAT(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_DRAW_OPA(u32 param, void* pInfo) {
    CheckCallback_DRAW_OPA(CALLBACK_TIMING_A, param, pInfo);

    u32 drawMode = pInfo != nullptr ? *static_cast<u32*>(pInfo) : GetDrawMode();

    DrawResMdlDirectly(GetResMdl(), GetViewPosMtxArray(), GetViewNrmMtxArray(),
                       GetViewTexMtxArray(), GetByteCodeDrawOpa(), nullptr,
                       &mReplacement, drawMode);

    CheckCallback_DRAW_OPA(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_DRAW_XLU(u32 param, void* pInfo) {
    CheckCallback_DRAW_XLU(CALLBACK_TIMING_A, param, pInfo);

    u32 drawMode = pInfo != nullptr ? *static_cast<u32*>(pInfo) : GetDrawMode();

    DrawResMdlDirectly(GetResMdl(), GetViewPosMtxArray(), GetViewNrmMtxArray(),
                       GetViewTexMtxArray(), nullptr, GetByteCodeDrawXlu(),
                       &mReplacement, drawMode);

    CheckCallback_DRAW_XLU(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_CALC_VTX(u32 param, void* pInfo) {
#pragma unused(param)
#pragma unused(pInfo)

    if (GetAnmObjShp() == nullptr) {
        return;
    }

    CalcVtx(GetResMdl(), GetAnmObjShp(), mReplacement.vtxPosTable,
            mReplacement.vtxNrmTable, mReplacement.vtxClrTable);
}

void ScnMdl::G3dProc(u32 task, u32 param, void* pInfo) {
    if (IsG3dProcDisabled(task)) {
        return;
    }

    switch (task) {
    case G3DPROC_GATHER_SCNOBJ: {
        ScnMdlSmpl_G3DPROC_GATHER_SCNOBJ(param,
                                         static_cast<IScnObjGather*>(pInfo));
        break;
    }

    case G3DPROC_CALC_WORLD: {
        ScnMdl_G3DPROC_CALC_WORLD(param, static_cast<math::MTX34*>(pInfo));
        break;
    }

    case G3DPROC_CALC_MAT: {
        ScnMdl_G3DPROC_CALC_MAT(param, pInfo);
        break;
    }

    case G3DPROC_CALC_VIEW: {
        ScnMdlSmpl_G3DPROC_CALC_VIEW(param, static_cast<math::MTX34*>(pInfo));
        break;
    }

    case G3DPROC_DRAW_OPA: {
        ScnMdl_G3DPROC_DRAW_OPA(param, pInfo);
        break;
    }

    case G3DPROC_DRAW_XLU: {
        ScnMdl_G3DPROC_DRAW_XLU(param, pInfo);
        break;
    }

    case G3DPROC_UPDATEFRAME: {
        ScnMdlSmpl_G3DPROC_UPDATEFRAME(param, pInfo);

        if (mpAnmObjShp != nullptr) {
            mpAnmObjShp->UpdateFrame();
        }
        break;
    }

    case G3DPROC_CHILD_DETACHED: {
        RemoveAnmObj(static_cast<AnmObj*>(pInfo));
        break;
    }

    case G3DPROC_CALC_VTX: {
        ScnMdl_G3DPROC_CALC_VTX(param, pInfo);
        break;
    }

    default: {
        DefG3dProcScnLeaf(task, param, pInfo);
        break;
    }
    }
}

bool ScnMdl::SetScnObjOption(u32 option, u32 value) {
    switch (option) {
    case OPTION_VISBUFFER_REFRESH_NEEDED: {
        if (value) {
            mFlagVisBuffer &= ~VISBUFFER_NOT_REFRESH_NEEDED;
        } else {
            mFlagVisBuffer |= VISBUFFER_NOT_REFRESH_NEEDED;
        }
        break;
    }

    default: {
        return ScnMdlSimple::SetScnObjOption(option, value);
    }
    }

    return true;
}

bool ScnMdl::GetScnObjOption(u32 option, u32* pValue) const {
    if (pValue == nullptr) {
        return false;
    }

    switch (option) {
    case OPTION_VISBUFFER_REFRESH_NEEDED: {
        *pValue = !(mFlagVisBuffer & VISBUFFER_NOT_REFRESH_NEEDED);
        break;
    }

    default: {
        return ScnMdlSimple::GetScnObjOption(option, pValue);
    }
    }

    return true;
}

void ScnMdl::InitBuffer() {
    ResMdl mdl = GetResMdl();

    u32 matNum = mdl.GetResMatNumEntries();
    u32 nodeNum = mdl.GetResNodeNumEntries();

    u32 i;

    if (mReplacement.visArray != nullptr) {
        for (i = 0; i < nodeNum; i++) {
            if (mdl.GetResNode(i).IsVisible()) {
                mReplacement.visArray[i] = true;
            } else {
                mReplacement.visArray[i] = false;
            }
        }
    }

    for (i = 0; i < matNum; i++) {
        ResMat mat = mdl.GetResMat(i);
        mpMatBufferDirtyFlag[i] = 0;

        if (mReplacement.texObjDataArray != nullptr) {
            mat.GetResTexObj()
                .CopyTo(&mReplacement.texObjDataArray[i])
                .EndEdit();
        }

        if (mReplacement.tlutObjDataArray != nullptr) {
            mat.GetResTlutObj()
                .CopyTo(&mReplacement.tlutObjDataArray[i])
                .EndEdit();
        }

        if (mReplacement.texSrtDataArray != nullptr) {
            mat.GetResTexSrt()
                .CopyTo(&mReplacement.texSrtDataArray[i])
                .EndEdit();
        }

        if (mReplacement.chanDataArray != nullptr) {
            mat.GetResMatChan()
                .CopyTo(&mReplacement.chanDataArray[i])
                .EndEdit();
        }

        if (mReplacement.genModeDataArray != nullptr) {
            mat.GetResGenMode()
                .CopyTo(&mReplacement.genModeDataArray[i])
                .EndEdit();
        }

        if (mReplacement.matMiscDataArray != nullptr) {
            mat.GetResMatMisc()
                .CopyTo(&mReplacement.matMiscDataArray[i])
                .EndEdit();
        }

        if (mReplacement.pixDLArray != nullptr) {
            mat.GetResMatPix().CopyTo(&mReplacement.pixDLArray[i]).EndEdit();
        }

        if (mReplacement.tevColorDLArray != nullptr) {
            mat.GetResMatTevColor()
                .CopyTo(&mReplacement.tevColorDLArray[i])
                .EndEdit();
        }

        if (mReplacement.indMtxAndScaleDLArray != nullptr) {
            mat.GetResMatIndMtxAndScale()
                .CopyTo(&mReplacement.indMtxAndScaleDLArray[i])
                .EndEdit();
        }

        if (mReplacement.texCoordGenDLArray != nullptr) {
            mat.GetResMatTexCoordGen()
                .CopyTo(&mReplacement.texCoordGenDLArray[i])
                .EndEdit();
        }

        if (mReplacement.tevDataArray != nullptr) {
            mat.GetResTev().CopyTo(&mReplacement.tevDataArray[i]).EndEdit();
        }
    }
}

void ScnMdl::CleanMatBuffer(u32 idx, u32 option) {
    ResMat mat = GetResMdl().GetResMat(idx);

    if ((option & BUFOPTION_TEXOBJ) && mReplacement.texObjDataArray != nullptr) {
        mat.GetResTexObj().CopyTo(&mReplacement.texObjDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_TLUTOBJ) && mReplacement.tlutObjDataArray != nullptr) {
        mat.GetResTlutObj().CopyTo(&mReplacement.tlutObjDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_TEXSRT) && mReplacement.texSrtDataArray != nullptr) {
        mat.GetResTexSrt().CopyTo(&mReplacement.texSrtDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_MATCHAN) && mReplacement.chanDataArray != nullptr) {
        mat.GetResMatChan().CopyTo(&mReplacement.chanDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_GENMODE) && mReplacement.genModeDataArray != nullptr) {
        mat.GetResGenMode().CopyTo(&mReplacement.genModeDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_MATMISC) && mReplacement.matMiscDataArray != nullptr) {
        mat.GetResMatMisc().CopyTo(&mReplacement.matMiscDataArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_MATPIX) && mReplacement.pixDLArray != nullptr) {
        mat.GetResMatPix().CopyTo(&mReplacement.pixDLArray[idx]).EndEdit();
    }

    if ((option & BUFOPTION_MATTEVCOLOR) &&
        mReplacement.tevColorDLArray != nullptr) {

        mat.GetResMatTevColor()
            .CopyTo(&mReplacement.tevColorDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFOPTION_MATINDMTXSCALE) &&
        mReplacement.indMtxAndScaleDLArray != nullptr) {

        mat.GetResMatIndMtxAndScale()
            .CopyTo(&mReplacement.indMtxAndScaleDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFOPTION_MATTEXCOORDGEN) &&
        mReplacement.texCoordGenDLArray != nullptr) {

        mat.GetResMatTexCoordGen()
            .CopyTo(&mReplacement.texCoordGenDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFOPTION_TEV) && mReplacement.tevDataArray != nullptr) {
        mat.GetResTev().CopyTo(&mReplacement.tevDataArray[idx]).EndEdit();
    }

    mpMatBufferDirtyFlag[idx] = mpMatBufferDirtyFlag[idx] & ~option;
}

void ScnMdl::CleanVisBuffer() {
    ResMdl mdl = GetResMdl();
    u32 nodeNum = mdl.GetResNodeNumEntries();

    if (mReplacement.visArray != nullptr) {
        for (u32 i = 0; i < nodeNum; i++) {
            if (mdl.GetResNode(i).IsVisible()) {
                mReplacement.visArray[i] = true;
            } else {
                mReplacement.visArray[i] = false;
            }
        }
    }

    mFlagVisBuffer &= ~VISBUFFER_DIRTY;
}

bool ScnMdl::SetAnmObj(AnmObj* pObj, AnmObjType type) {
    if (pObj != nullptr && pObj->GetParent() == nullptr) {
        if (type == ANMOBJTYPE_SHP || type == ANMOBJTYPE_NOT_SPECIFIED) {
            AnmObjShp* pShp = DynamicCast<AnmObjShp>(pObj);

            if (pShp != nullptr) {
                if (!pShp->IsBound()) {
                    return false;
                }

                if (mpAnmObjShp != nullptr) {
                    RemoveAnmObj(mpAnmObjShp);
                }

                mpAnmObjShp = pShp;
                pShp->G3dProc(G3DPROC_ATTACH_PARENT, 0, this);

                mReplacement.flag &= ~1;

                return true;
            } else {
                if (type == ANMOBJTYPE_NOT_SPECIFIED) {
                    return ScnMdlSimple::SetAnmObj(pObj, type);
                }

                return false;
            }
        } else {
            return ScnMdlSimple::SetAnmObj(pObj, type);
        }
    }

    return false;
}

bool ScnMdl::RemoveAnmObj(AnmObj* pObj) {
    if (pObj == nullptr) {
        return false;
    }

    if (pObj == mpAnmObjShp) {
        mpAnmObjShp->G3dProc(G3DPROC_DETACH_PARENT, 0, this);
        mpAnmObjShp = nullptr;

        if (!(mReplacementFlag & 1)) {
            mReplacement.flag |= 1;
            return true;
        }

            if (mReplacement.vtxPosTable != nullptr) {
                u32 vtxPosNum = GetResMdl().GetResVtxPosNumEntries();

                for (u32 i = 0; i < vtxPosNum; i++) {
                    ResVtxPos pos = GetResMdl().GetResVtxPos(i);

                    if (pos.ptr() != mReplacement.vtxPosTable[i]) {
                        pos.CopyTo(mReplacement.vtxPosTable[i]);
                    }
                }
            }

            if (mReplacement.vtxNrmTable != nullptr) {
                u32 vtxNrmNum = GetResMdl().GetResVtxNrmNumEntries();

                for (u32 i = 0; i < vtxNrmNum; i++) {
                    ResVtxNrm nrm = GetResMdl().GetResVtxNrm(i);

                    if (nrm.ptr() != mReplacement.vtxNrmTable[i]) {
                        nrm.CopyTo(mReplacement.vtxNrmTable[i]);
                    }
                }
            }

            if (mReplacement.vtxClrTable != nullptr) {
                u32 vtxClrNum = GetResMdl().GetResVtxClrNumEntries();

                for (u32 i = 0; i < vtxClrNum; i++) {
                    ResVtxClr clr = GetResMdl().GetResVtxClr(i);

                    if (clr.ptr() != mReplacement.vtxClrTable[i]) {
                        clr.CopyTo(mReplacement.vtxClrTable[i]);
                    }
                }
            }

        return true;
    }

    return ScnMdlSimple::RemoveAnmObj(pObj);
}

AnmObj* ScnMdl::RemoveAnmObj(AnmObjType type) {
    if (type == ANMOBJTYPE_SHP) {
        AnmObj* pOld = mpAnmObjShp;
        RemoveAnmObj(mpAnmObjShp);
        return pOld;
    }

    return ScnMdlSimple::RemoveAnmObj(type);
}

AnmObj* ScnMdl::GetAnmObj(AnmObjType type) {
    if (type == ANMOBJTYPE_SHP) {
        return mpAnmObjShp;
    }

    return ScnMdlSimple::GetAnmObj(type);
}

const AnmObj* ScnMdl::GetAnmObj(AnmObjType type) const {
    if (type == ANMOBJTYPE_SHP) {
        return mpAnmObjShp;
    }

    return ScnMdlSimple::GetAnmObj(type);
}

ScnMdl::ScnMdl(MEMAllocator* pAllocator, ResMdl mdl,
               math::MTX34* pWorldMtxArray, u32* pWorldMtxAttribArray,
               math::MTX34* pViewPosMtxArray, math::MTX33* pViewNrmMtxArray,
               math::MTX34* pViewTexMtxArray, int numView, int numViewMtx,
               DrawResMdlReplacement* pReplacement, u32* pMatBufferDirtyFlag, u32 replacementFlag)
    : ScnMdlSimple(pAllocator, mdl, pWorldMtxArray, pWorldMtxAttribArray,
                   pViewPosMtxArray, pViewNrmMtxArray, pViewTexMtxArray,
                   numView, numViewMtx),
      mpAnmObjShp(nullptr),
      mFlagVisBuffer(0),
      mpMatBufferDirtyFlag(pMatBufferDirtyFlag),
      mReplacement(*pReplacement),
      mReplacementFlag(replacementFlag) {}

ScnMdl::~ScnMdl() {
    if (mpAnmObjShp != nullptr) {
        RemoveAnmObj(mpAnmObjShp);
    }
}

}
}
