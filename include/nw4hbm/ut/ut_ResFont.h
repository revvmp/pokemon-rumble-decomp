#ifndef NW4HBM_UT_RES_FONT_H
#define NW4HBM_UT_RES_FONT_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/ut/ut_ResFontBase.h"

namespace nw4hbm {
namespace ut {

struct BinaryFileHeader;

class ResFont : public detail::ResFontBase {
public:
    ResFont();
    virtual ~ResFont();

    bool SetResource(void* pBuffer);
    void RemoveResource();

private:
    static FontInformation* Rebuild(BinaryFileHeader* pHeader);

private:
    static const u32 SIGNATURE = FOURCC('R', 'F', 'N', 'T');
    static const u32 SIGNATURE_UNPACKED = FOURCC('R', 'F', 'N', 'U');

    static const u32 SIGNATURE_FONTINFO = FOURCC('F', 'I', 'N', 'F');
    static const u32 SIGNATURE_TEXGLYPH = FOURCC('T', 'G', 'L', 'P');
    static const u32 SIGNATURE_CHARWIDTH = FOURCC('C', 'W', 'D', 'H');
    static const u32 SIGNATURE_CHARMAP = FOURCC('C', 'M', 'A', 'P');
    static const u32 SIGNATURE_GLYPHGROUP = FOURCC('G', 'L', 'G', 'R');
};

}
}

#endif
