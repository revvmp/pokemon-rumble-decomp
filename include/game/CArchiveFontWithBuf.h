#ifndef GAME_CARCHIVEFONTWITHBUF_H
#define GAME_CARCHIVEFONTWITHBUF_H

#include <memory>
#include <nw4r/ut/ut_ArchiveFont.h>

class CArchiveFontWithBuf : public nw4r::ut::ArchiveFont {
public:
    CArchiveFontWithBuf();
    virtual ~CArchiveFontWithBuf();

    bool Load(const char* pPath, bool background);

private:
    std::tr1::shared_ptr<u8> mBuffer;
};

#endif
