#include <game/CDividedGround.h>
#include <string.h>

CDividedGround::CDividedGround(u32 type) : BModelObj(type | TYPE) {}

CDividedGround::~CDividedGround() {}

void CDividedGround::LoadAttr(const char* pName, u32 flags) {
    LoadAttr(mStageID, mStageNo, pName, flags);
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
