#ifndef NW4R_EF_RESOURCE_H
#define NW4R_EF_RESOURCE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_res_emitter.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

struct TextureData;
class ResTexture;

struct EffectProject {
    u32 headersize;
    ut::Link projectlink;
    u16 projectnameLen;
    u16 PADDING_0xE;
    u8 projectname[4];
};

struct TextureProject {
    u32 headersize;
    ut::Link projectlink;
    u16 projectnameLen;
    u16 PADDING_0xE;
    u8 projectname[4];
};

class Resource {
private:
    static const u16 VERSION = 7;

public:
    ut::List mBREFFList;
    u32 mNumEmitter;
    ut::List mBREFTList;
    u32 mNumTexture;

public:
    static u16 GetVersion() {
        return VERSION;
    }

    static Resource* GetInstance();

    EffectProject* Add(u8* pData);
    TextureProject* AddTexture(u8* pData);

    ResEmitter FindEmitter(const char* pName, EffectProject* pEffProject) const;
    EmitterResource* _FindEmitter(const char* pName,
                                  EffectProject* pEffProject) const;

    u32 NumEmitter(EffectProject* pEffProject) const;
    EmitterResource* _GetEmitterIndexOf(u32 idx,
                                        EffectProject* pEffProject) const;

    ResTexture FindTexture(const char* pName,
                           TextureProject* pTexProject) const;
    TextureData* _FindTexture(const char* pName,
                              TextureProject* pTexProject) const;

    u32 RelocateCommand();

    bool RemoveEffectProject(EffectProject* pEffProject);
    bool RemoveTextureProject(TextureProject* pTexProject);

private:
    Resource();

    void Initialize();

    u32 relocateCurveTexture(u8* pCmdList, const EmitterResource* pResource);
    u32 relocateCurveChild(u8* pCmdList, const EmitterResource* pResource);

private:
    static Resource mResource;
};

}
}

#endif
