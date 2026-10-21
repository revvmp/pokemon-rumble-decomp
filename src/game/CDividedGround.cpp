#include <game/CDividedGround.h>
#include <game/CFileData.h>
#include <game/CPpuEngine.h>
#include <game/StringFormat.h>
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

std::string FormatString(const char* pFormat, ...) {
    std::string result;
    va_list args;
    va_start(args, pFormat);
    VFormatString(result, pFormat, args);
    va_end(args);
    return result;
}
