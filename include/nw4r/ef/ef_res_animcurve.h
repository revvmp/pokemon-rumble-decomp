#ifndef NW4R_EF_RES_ANIM_CURVE_H
#define NW4R_EF_RES_ANIM_CURVE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>

#define NW4R_EF_MAGIC_ANIMCURVE 0xAC
#define NW4R_EF_MAGIC_ANIMBAKED 0xAB

namespace nw4r {
namespace ef {

enum AnimCurveType {
    AC_TYPE_PARTICLE_U8 = 0,
    AC_TYPE_PARTICLE_F32 = 3,
    AC_TYPE_PARTICLE_ROTATE = 6,
    AC_TYPE_PARTICLE_TEXTURE = 4,
    AC_TYPE_CHILD = 5
};

enum AnimCurveTarget {
    AC_TARGET_COLOR0PRI,
    AC_TARGET_ALPHA0PRI,
    AC_TARGET_COLOR0SEC,
    AC_TARGET_ALPHA0SEC,
    AC_TARGET_COLOR1PRI,
    AC_TARGET_ALPHA1PRI,
    AC_TARGET_COLOR1SEC,
    AC_TARGET_ALPHA1SEC,
    AC_TARGET_ACMPREF0,
    AC_TARGET_ACMPREF1,

    AC_TARGET_SIZE,
    AC_TARGET_SCALE,
    AC_TARGET_12,
    AC_TARGET_TEXTURE1SCALE = 17,
    AC_TARGET_TEXTURE1ROTATE,
    AC_TARGET_TEXTURE1TRANSLATE,
    AC_TARGET_TEXTURE2SCALE,
    AC_TARGET_TEXTURE2ROTATE,
    AC_TARGET_TEXTURE2TRANSLATE,
    AC_TARGET_TEXTUREINDSCALE,
    AC_TARGET_TEXTUREINDROTATE,
    AC_TARGET_TEXTUREINDTRANSLATE,

    AC_TARGET_UNK32 = 32,
    AC_TARGET_FIELD_SPEED,
    AC_TARGET_UNK34,
    AC_TARGET_UNK35,
    AC_TARGET_UNK36,
    AC_TARGET_UNK38 = 38,
    AC_TARGET_UNK39,
    AC_TARGET_EMIT_COMMONPARAM = 64,
    AC_TARGET_EMIT_SCALE,
    AC_TARGET_EMIT_ROTATE,
    AC_TARGET_EMIT_TRANSLATE,
    AC_TARGET_EMIT_SPEED_ORIG,
    AC_TARGET_EMIT_SPEED_YAXIS,
    AC_TARGET_EMIT_SPEED_RANDOM,
    AC_TARGET_EMIT_SPEED_NORMAL,
    AC_TARGET_EMIT_SPEED_SPECDIR,
    AC_TARGET_EMIT_EMISSION,

    AC_TARGET_ROTATE = 12,

};

struct AnimCurveHeader {
    enum ProcessFlag {
        PROC_FLAG_SYNC = (1 << 2),
        PROC_FLAG_STOP = (1 << 3),
        PROC_FLAG_TIMING = (1 << 4),
        PROC_FLAG_INFLOOP = (1 << 5),
        PROC_FLAG_TURN = (1 << 6),
        PROC_FLAG_FITTING = (1 << 7),
    };

    u8 magic;
    u8 kindType;
    u8 curveFlag;
    u8 kindEnable;
    u8 processFlag;
    u8 loopCount;
    u16 randomSeed;
    u16 frameLength;
    u16 PADDING_0xA;
    u32 keyTable;
    u32 rangeTable;
    u32 randomTable;
    u32 nameTable;
    u32 infoTable;
};

struct AnimCurveKey {
    u16 count;
    u16 PADDING_0x2;
    u8 datas[1];
};

struct AnimCurveRandom {
    u16 count;
    u16 PADDING_0x2;
    u8 datas[1];
};

struct AnimCurveRandomTable {
    u16 count;
    u16 PADDING_0x2;
    u8 datas[1];
};

struct AnimCurveNameTable {
    struct Name {
        u32 work;
    };

    u16 count;
    u16 PADDING_0x2;
    Name datas[1];
};

class ResAnimCurve {
private:
    u8* mAnimCurveData;

public:
    explicit ResAnimCurve(u8* pData = NULL) : mAnimCurveData(pData) {}
    ResAnimCurve(const ResAnimCurve& rOther)
        : mAnimCurveData(rOther.mAnimCurveData) {}

    bool IsValid() const {
        return mAnimCurveData != NULL;
    }

    AnimCurveHeader* ptr() {
        return reinterpret_cast<AnimCurveHeader*>(mAnimCurveData);
    }

    void SetStop(bool stop);
};

}
}

#endif
