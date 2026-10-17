#include <nw4r/ef/ef_effect.h>
#include <string.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_arithmetic.h>

namespace nw4r {
namespace ef {
Resource Resource::mResource;

Resource* Resource::GetInstance() {
    return &mResource;
}

Resource::Resource() {
    Initialize();
}

void Resource::Initialize() {
    ut::List_Init(&mBREFFList, offsetof(EffectProject, projectlink));
    ut::List_Init(&mBREFTList, offsetof(TextureProject, projectlink));
    mNumEmitter = 0;
    mNumTexture = 0;
}

struct FileHeader {
    char signature[4];
    u8 byteorder[2];
    u16 version;
    u32 filesize;
    u16 headersize;
    u16 blocknum;
};

struct BlockHeader {
    char type[4];
    u32 headersize;
};

struct NameTable {
    u32 tablesize;
    u16 numEntry;
    u16 PADDING_0x4;
};

 void breffRelocate(EffectProject* pEffProject) {
    u8* pPtr = reinterpret_cast<u8*>(pEffProject);
    pPtr += pEffProject->headersize;

    u8* pData = pPtr;
    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);

    pData += sizeof(NameTable);

    for (int i = 0; i < pNameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        u32 size =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        EmitterResource* pResource = reinterpret_cast<EmitterResource*>(
            reinterpret_cast<u8*>(pNameTable) + offset);

        *reinterpret_cast<char**>(pResource) = pName;

        pResource->LocateTracks();
    }
}

static u16 breffNumEmitter(EffectProject* pEffProject) {
    u8* pPtr = reinterpret_cast<u8*>(pEffProject);
    pPtr += pEffProject->headersize;

    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);
    return pNameTable->numEntry;
}

static u16 breftNumTexture(TextureProject* pTexProject) {
    u8* pPtr = reinterpret_cast<u8*>(pTexProject);
    pPtr += pTexProject->headersize;

    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);
    return pNameTable->numEntry;
}

static EmitterResource* breffSearchName(EffectProject* pEffProject,
                                        const char* pEffName) {
    u8* pPtr = reinterpret_cast<u8*>(pEffProject);
    pPtr += pEffProject->headersize;

    u8* pData = pPtr;
    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);

    pData += sizeof(NameTable);

    for (int i = 0; i < pNameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        pData += sizeof(u32);

        if (strcmp(pEffName, pName) == 0) {
            return reinterpret_cast<EmitterResource*>(
                reinterpret_cast<u8*>(pNameTable) + offset);
        }
    }

    return NULL;
}

static EmitterResource* breffIndexOf(EffectProject* pEffProject, u16 idx) {
    u8* pPtr = reinterpret_cast<u8*>(pEffProject);
    pPtr += pEffProject->headersize;

    u8* pData = pPtr;
    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);

    if (idx >= pNameTable->numEntry) {
        return NULL;
    }

    pData += sizeof(NameTable);

    for (int i = 0; i < pNameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pEffName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        pData += sizeof(u32);

        if (i == idx) {
            return reinterpret_cast<EmitterResource*>(
                reinterpret_cast<u8*>(pNameTable) + offset);
        }
    }

    return NULL;
}

static void breftRelocate(TextureProject* pTexProject) {
    u8* pPtr = reinterpret_cast<u8*>(pTexProject);
    pPtr += pTexProject->headersize;

    u8* pData = pPtr;
    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);

    pData += sizeof(NameTable);

    for (int i = 0; i < pNameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        u32 size =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        TextureData* pTexData = reinterpret_cast<TextureData*>(
            reinterpret_cast<u8*>(pNameTable) + offset);

        pTexData->name = pName;
        pTexData->texture =
            reinterpret_cast<u8*>(pTexData) + sizeof(TextureData);
        pTexData->tlut = reinterpret_cast<u8*>(pTexData) + sizeof(TextureData) +
                         pTexData->dataSize;
    }
}

static TextureData* breftSearchName(TextureProject* pTexProject,
                                    const char* pTexName) {
    u8* pPtr = reinterpret_cast<u8*>(pTexProject);
    pPtr += pTexProject->headersize;

    u8* pData = pPtr;
    NameTable* pNameTable = reinterpret_cast<NameTable*>(pPtr);

    pData += sizeof(NameTable);

    for (int i = 0; i < pNameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        pData += sizeof(u32);

        if (strcmp(pTexName, pName) == 0) {
            return reinterpret_cast<TextureData*>(
                reinterpret_cast<u8*>(pNameTable) + offset);
        }
    }

    return NULL;
}

EffectProject* Resource::Add(u8* pData) {
    u8* pPtr = pData;

    FileHeader* pFileHeader = reinterpret_cast<FileHeader*>(pPtr);
    pPtr += pFileHeader->headersize;

    BlockHeader* pBlockHeader = reinterpret_cast<BlockHeader*>(pPtr);
    pPtr += sizeof(BlockHeader);

    EffectProject* pEffProject = reinterpret_cast<EffectProject*>(pPtr);

    memset(&pEffProject->projectlink, 0, sizeof(ut::Link));
    ut::List_Append(&mBREFFList, pEffProject);

    mNumEmitter += breffNumEmitter(pEffProject);
    breffRelocate(pEffProject);

    return pEffProject;
}

ResEmitter Resource::FindEmitter(const char* pName,
                                 EffectProject* pEffProject) const {
    EmitterResource* const pResource = _FindEmitter(pName, pEffProject);
    return ResEmitter(pResource);
}

EmitterResource* Resource::_FindEmitter(const char* pName,
                                        EffectProject* pEffProject) const {
    EmitterResource* pResource;

    if (pEffProject != NULL) {
        pResource = breffSearchName(pEffProject, pName);
        if (pResource != NULL) {
            return pResource;
        }
    }

    pEffProject = static_cast<EffectProject*>(ut::List_GetLast(&mBREFFList));

    while (pEffProject != NULL) {
        pResource = breffSearchName(pEffProject, pName);
        if (pResource != NULL) {
            return pResource;
        }

        pEffProject = static_cast<EffectProject*>(
            ut::List_GetPrev(&mBREFFList, pEffProject));
    }

    return NULL;
}

ResTexture Resource::FindTexture(const char* pName,
                                 TextureProject* pTexProject) const {
    TextureData* pTexData = _FindTexture(pName, pTexProject);
    return ResTexture(pTexData);
}

TextureData* Resource::_FindTexture(const char* pName,
                                    TextureProject* pTexProject) const {
    TextureData* pTexData;

    if (pName[0] == 0) {
        return NULL;
    }

    if (pTexProject != NULL) {
        pTexData = breftSearchName(pTexProject, pName);
        if (pTexData != NULL) {
            return pTexData;
        }
    }

    pTexProject = static_cast<TextureProject*>(ut::List_GetLast(&mBREFTList));

    while (pTexProject != NULL) {
        pTexData = breftSearchName(pTexProject, pName);
        if (pTexData != NULL) {
            return pTexData;
        }

        pTexProject = static_cast<TextureProject*>(
            ut::List_GetPrev(&mBREFTList, pTexProject));
    }

    return NULL;
}

u32 Resource::NumEmitter(EffectProject* pEffProject) const {
    if (pEffProject != NULL) {
        return breffNumEmitter(pEffProject);
    }

    return mNumEmitter;
}

EmitterResource*
Resource::_GetEmitterIndexOf(u32 idx, EffectProject* pEffProject) const {
    if (pEffProject != NULL) {
        u16 numEmitter = breffNumEmitter(pEffProject);

        if (idx < numEmitter) {
            return breffIndexOf(pEffProject, idx);
        }

        return NULL;
    }

    pEffProject = static_cast<EffectProject*>(ut::List_GetFirst(&mBREFFList));

    while (pEffProject != NULL) {
        u16 numEmitter = breffNumEmitter(pEffProject);

        if (idx < numEmitter) {
            return breffIndexOf(pEffProject, idx);
        }

        idx -= numEmitter;

        pEffProject = static_cast<EffectProject*>(
            ut::List_GetNext(&mBREFFList, pEffProject));
    }

    return NULL;
}

TextureProject* Resource::AddTexture(u8* pData) {
    u8* pPtr = pData;

    FileHeader* pFileHeader = reinterpret_cast<FileHeader*>(pPtr);
    pPtr += pFileHeader->headersize;

    BlockHeader* pBlockHeader = reinterpret_cast<BlockHeader*>(pPtr);
    pPtr += sizeof(BlockHeader);

    TextureProject* pTexProject = reinterpret_cast<TextureProject*>(pPtr);

    memset(&pTexProject->projectlink, 0, sizeof(ut::Link));
    ut::List_Append(&mBREFTList, pTexProject);

    mNumTexture += breftNumTexture(pTexProject);
    breftRelocate(pTexProject);

    return pTexProject;
}

u32 Resource::relocateCurveTexture(u8* pCmdList,
                                   const EmitterResource* pResource) {
#pragma unused(pResource)

    u32 error = 0;

    AnimCurveHeader* pHeader = reinterpret_cast<AnimCurveHeader*>(pCmdList);

    AnimCurveKey* pKey = reinterpret_cast<AnimCurveKey*>(
        reinterpret_cast<u8*>(pHeader) + sizeof(AnimCurveHeader));

    AnimCurveRandom* pRandom = reinterpret_cast<AnimCurveRandom*>(
        reinterpret_cast<u8*>(pKey) + pHeader->keyTable);

    AnimCurveRandomTable* pRandomTable =
        reinterpret_cast<AnimCurveRandomTable*>(reinterpret_cast<u8*>(pRandom) +
                                                pHeader->rangeTable);

    AnimCurveNameTable* pNameTable = reinterpret_cast<AnimCurveNameTable*>(
        reinterpret_cast<u8*>(pRandomTable) + pHeader->randomTable);

    if (pHeader->nameTable == 0) {
        return 0;
    }

    u8* pName = reinterpret_cast<u8*>(pNameTable) +
                offsetof(AnimCurveNameTable, datas) +
                pNameTable->count * sizeof(AnimCurveNameTable::Name);

    for (int i = 0; i < pNameTable->count; i++) {
        int size = *reinterpret_cast<u16*>(pName);
        pName += sizeof(u16);

        pNameTable->datas[i].work = reinterpret_cast<u32>(
            _FindTexture(reinterpret_cast<const char*>(pName), NULL));

        if (pName[0] != 0 && pNameTable->datas[i].work == 0) {
            error++;
        }

        pName += size;
    }

    return error;
}

u32 Resource::relocateCurveChild(u8* pCmdList,
                                 const EmitterResource* pResource) {
#pragma unused(pResource)

    u32 error = 0;

    AnimCurveHeader* pHeader = reinterpret_cast<AnimCurveHeader*>(pCmdList);

    AnimCurveKey* pKey = reinterpret_cast<AnimCurveKey*>(
        reinterpret_cast<u8*>(pHeader) + sizeof(AnimCurveHeader));

    AnimCurveRandom* pRandom = reinterpret_cast<AnimCurveRandom*>(
        reinterpret_cast<u8*>(pKey) + pHeader->keyTable);

    AnimCurveRandomTable* pRandomTable =
        reinterpret_cast<AnimCurveRandomTable*>(reinterpret_cast<u8*>(pRandom) +
                                                pHeader->rangeTable);

    AnimCurveNameTable* pNameTable = reinterpret_cast<AnimCurveNameTable*>(
        reinterpret_cast<u8*>(pRandomTable) + pHeader->randomTable);

    if (pHeader->nameTable == 0) {
        return 0;
    }

    u8* pName = reinterpret_cast<u8*>(pNameTable) +
                offsetof(AnimCurveNameTable, datas) +
                pNameTable->count * sizeof(AnimCurveNameTable::Name);

    for (int i = 0; i < pNameTable->count; i++) {
        int size = *reinterpret_cast<u16*>(pName);
        pName += sizeof(u16);

        pNameTable->datas[i].work = reinterpret_cast<u32>(
            _FindEmitter(reinterpret_cast<char*>(pName), NULL));

        if (pNameTable->datas[i].work == 0) {
            error++;
        }

        pName += size;
    }

    return error;
}

u32 Resource::RelocateCommand() {
    u32 error = 0;

    for (u32 idx = 0; idx < NumEmitter(NULL); idx++) {
        u8 localError = 0;

        EmitterResource* pResource = _GetEmitterIndexOf(idx, NULL);
        if (pResource == NULL) {
            continue;
        }

        ParticleParameterDesc* pPtclDesc =
            pResource->GetParticleParameterDesc();
        TextureData** ppWork = pPtclDesc->mTexture;
        u8* pPtr = pPtclDesc->textureNames;

        for (int i = 0; i < TEX_LAYER_MAX; i++) {
            u16 size = *reinterpret_cast<u16*>(pPtr);
            pPtr += sizeof(u16);

            ppWork[i] = _FindTexture(reinterpret_cast<char*>(pPtr), NULL);

            if (pPtr[0] != 0 && ppWork[i] == NULL) {
                localError++;
            }

            pPtr += size;
        }

        for (u16 pidx = 0; pidx < pResource->NumPtclTrack(); pidx++) {
            u8* pPtr = pResource->GetPtclTrack(pidx);

            AnimCurveHeader* pHeader = reinterpret_cast<AnimCurveHeader*>(pPtr);

            if (!(pHeader->magic >= NW4R_EF_MAGIC_ANIMBAKED &&
                  pHeader->magic <= NW4R_EF_MAGIC_ANIMCURVE)) {
                continue;
            }

            switch (pHeader->curveFlag) {
            case AC_TYPE_PARTICLE_TEXTURE: {
                u8 num = relocateCurveTexture(pPtr, pResource);
                localError += num;
                break;
            }

            case AC_TYPE_CHILD:
            case AC_TYPE_EMITTER_CHILD: {
                u8 num = relocateCurveChild(pPtr, pResource);
                localError += num;
                break;
            }
            }
        }

        if (localError != 0) {
            pResource->GetEmitterDesc()->commonFlag &=
                ~EmitterDesc::CMN_FLAG_RELOCATED;
        } else {
            pResource->GetEmitterDesc()->commonFlag |=
                EmitterDesc::CMN_FLAG_RELOCATED;
        }

        error += localError;
    }

    return error;
}

bool Resource::RemoveEffectProject(EffectProject* pEffProject) {
    EffectProject* pIt =
        static_cast<EffectProject*>(ut::List_GetFirst(&mBREFFList));

    for (; pIt != NULL; pIt = static_cast<EffectProject*>(
                            ut::List_GetNext(&mBREFFList, pIt))) {
        if (pIt != pEffProject) {
            continue;
        }

        ut::List_Remove(&mBREFFList, pEffProject);

        mNumEmitter -=
            pEffProject != NULL ? breffNumEmitter(pEffProject) : mNumEmitter;

        return true;
    }

    return false;
}

bool Resource::RemoveTextureProject(TextureProject* pTexProject) {
    TextureProject* pIt =
        static_cast<TextureProject*>(ut::List_GetFirst(&mBREFTList));

    for (; pIt != NULL; pIt = static_cast<TextureProject*>(
                            ut::List_GetNext(&mBREFTList, pIt))) {
        if (pIt != pTexProject) {
            continue;
        }

        ut::List_Remove(&mBREFTList, pTexProject);

        mNumTexture -=
            pTexProject != NULL ? breftNumTexture(pTexProject) : mNumTexture;

        return true;
    }

    return false;
}
}
}
