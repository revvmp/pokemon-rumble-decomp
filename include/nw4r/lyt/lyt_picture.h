#ifndef NW4R_LYT_PICTURE_H
#define NW4R_LYT_PICTURE_H

#include "nw4r/lyt/lyt_common.h"
#include "nw4r/lyt/lyt_pane.h"
#include "nw4r/lyt/lyt_resources.h"

namespace nw4r {

namespace lyt {

class TexMap;

inline bool operator==(const Size& rLeft, const Size& rRight) {
    return rLeft.width == rRight.width && rLeft.height == rRight.height;
}

namespace res {
struct Picture : public Pane {
    u32 vtxCols[VERTEXCOLOR_MAX];
    u16 materialIdx;
    u8 texCoordNum;
    u8 padding[1];
};
}
class Picture : public Pane {
public:
    Picture(const res::Picture *pResPic, const ResBlockSet &resDataSet);

    virtual ~Picture();
    NW4R_UT_RTTI_DECL(Picture);
    virtual void DrawSelf(const DrawInfo &drawInfo);
    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);
    virtual void Append(const TexMap &);

    void Init(u8 texNum);

    void ReserveTexCoord(u8 texNum);

    void SetTexCoordNum(u8 texNum);

protected:
    ut::Color mVtxColors[VERTEXCOLOR_MAX];
    detail::TexCoordAry mTexCoordAry;
};

}

}

#endif
