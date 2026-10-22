#ifndef GAME_SCNMDLSIMPLEDATA_H
#define GAME_SCNMDLSIMPLEDATA_H
#include <memory>
#include <vector>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
class CFileData;
class ScnMdlSimpleData {
public:
    ScnMdlSimpleData() {}
    ~ScnMdlSimpleData() {}
    nw4r::g3d::ScnMdlSimple* GetModel() { return mModel.get(); }
    nw4r::g3d::ScnMdlSimple* GetModel() const { return mModel.get(); }
private:
    std::tr1::shared_ptr<nw4r::g3d::ScnMdlSimple> mModel;
    std::vector<std::tr1::shared_ptr<CFileData> > mFiles;
};
#endif
