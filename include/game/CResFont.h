#ifndef GAME_CRESFONT_H
#define GAME_CRESFONT_H

#include <game/CFileData.h>
#include <nw4r/ut/ut_ResFont.h>

class CResFont : public nw4r::ut::ResFont {
public:
    CResFont();
    virtual ~CResFont();

    bool Load(const char* pPath, bool background);

private:
    std::tr1::shared_ptr<CFileData> mFile;
};

#endif
