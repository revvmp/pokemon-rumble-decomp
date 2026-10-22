#ifndef GAME_SCNMDLSIMPLEDATAINLINE_H
#define GAME_SCNMDLSIMPLEDATAINLINE_H
#include <game/ScnMdlSimpleData.h>
inline ScnMdlSimpleData::ScnMdlSimpleData() {}
inline ScnMdlSimpleData::~ScnMdlSimpleData() {}
inline void ScnMdlSimpleData::Reset() {
    mModel.reset();
    std::vector<std::tr1::shared_ptr<CFileData> > empty;
    mFiles.swap(empty);
}
#endif
