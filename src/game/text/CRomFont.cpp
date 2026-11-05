#include <game/text/CRomFont.h>
#include <game/core/Memory.h>

CRomFont::CRomFont() {
    void* pBuffer = GameMemory::AllocateMEM2(nw4r::ut::RomFont::GetRequireBufferSize(), 32);
    mBuffer.reset(static_cast<u8*>(pBuffer), GameMemory::Free);
    nw4r::ut::RomFont::Load(mBuffer.get());
    SetEncoding(nw4r::ut::FONT_ENCODING_UTF16);
}

CRomFont::~CRomFont() {
    nw4r::ut::RomFont::Unload();
}
