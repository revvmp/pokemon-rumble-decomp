#include <game/text/CArchiveFontWithBuf.h>

#include <game/io/CFileData.h>
#include <game/core/Memory.h>

CArchiveFontWithBuf::CArchiveFontWithBuf() {}

CArchiveFontWithBuf::~CArchiveFontWithBuf() {
    if (mBuffer.get() != nullptr) {
        Destroy();
    }
}

bool CArchiveFontWithBuf::Load(const char* pPath, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pPath, 0x4000, background);
    if (!file || !file->IsLoaded()) {
        return false;
    }
    u32 size = GetRequireBufferSize(file->GetData());
    mBuffer.reset(static_cast<u8*>(GameMemory::AllocateMEM2(size, 32)), GameMemory::Free);
    if (Construct(mBuffer.get(), size, file->GetData())) {
        return true;
    }
    mBuffer.reset();
    return false;
}
