#ifndef GAME_SCNMDLSIMPLEDATAINLINE_H
#define GAME_SCNMDLSIMPLEDATAINLINE_H
#include <game/graphics/ScnMdlSimpleData.h>
inline ScnMdlSimpleData::~ScnMdlSimpleData() {}
inline void ScnMdlSimpleData::Release() {
    mModel.reset();
    std::vector<std::tr1::shared_ptr<CFileData> > empty;
    mFiles.swap(empty);
}
inline void ScnMdlSimpleData::Reset() {
    Release();
}
#endif
