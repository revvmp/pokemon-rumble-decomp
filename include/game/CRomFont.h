#ifndef GAME_CROMFONT_H
#define GAME_CROMFONT_H

#include <memory>
#include <nw4r/ut/ut_RomFont.h>
#include <game/AnyEncFont.h>

class CRomFont : public CAnyEncFont<nw4r::ut::RomFont> {
public:
    CRomFont();
    virtual ~CRomFont();

private:
    std::tr1::shared_ptr<u8> mBuffer;
};

#endif
