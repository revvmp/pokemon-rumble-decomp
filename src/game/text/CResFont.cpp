#include <game/text/CResFont.h>

bool CResFont::Load(const char* pPath, bool background) {
    RemoveResource();
    mFile = LoadResourceFile(pPath, 0x4000, background);
    if (!mFile || !mFile->IsValid()) {
        return false;
    }
    SetResource(mFile->GetData());
    return true;
}
