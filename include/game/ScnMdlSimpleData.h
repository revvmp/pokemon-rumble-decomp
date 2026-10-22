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
    bool IsLoaded() const { return mModel; }
    std::vector<std::tr1::shared_ptr<CFileData> >& GetFiles() { return mFiles; }
    bool Load(const char* pName, u32 flags, const u32* pOptions, bool background);
    void Reset() {
        mModel.reset();
        std::vector<std::tr1::shared_ptr<CFileData> > empty;
        mFiles.swap(empty);
    }
    const std::vector<std::tr1::shared_ptr<CFileData> >& GetFiles() const { return mFiles; }
private:
    std::tr1::shared_ptr<nw4r::g3d::ScnMdlSimple> mModel;
    std::vector<std::tr1::shared_ptr<CFileData> > mFiles;
};
#endif
