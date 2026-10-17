#ifndef NW4HBM_LYT_PICTURE_H
#define NW4HBM_LYT_PICTURE_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_pane.h"

namespace nw4hbm {
namespace lyt {

struct ResBlockSet;

namespace res {

struct Picture : Pane {
    static const u32 SIGNATURE = FOURCC('p', 'i', 'c', '1');

    u32 vtxCols[VERTEXCOLOR_MAX];
    u16 materialIdx;
    u8 texCoordNum;
    u8 PADDING_0x5F;
};

}

class Picture : public Pane {
public:
    NW4R_UT_RTTI_DECL(Picture);

public:
    Picture(const res::Picture* pRes, const ResBlockSet& rBlockSet);
    virtual ~Picture();

    virtual void DrawSelf(const DrawInfo& rInfo);

    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color color);

    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    virtual void Append(TPLPalette* pTplRes);
    virtual void Append(const GXTexObj& rTexObj);

protected:
    ut::Color mVtxColors[VERTEXCOLOR_MAX];
    detail::TexCoordAry mTexCoordAry;

protected:
    void ReserveTexCoord(u8 num);
    void SetTexCoordNum(u8 num);

private:
    void Init(u8 num);
};

}
}

#endif
