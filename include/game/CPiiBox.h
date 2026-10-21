#ifndef GAME_CPIIBOX_H
#define GAME_CPIIBOX_H

#include <revolution/types.h>
#include <list>
#include <memory>

class CPiiPersonalData;

class CPiiBox {
public:
    CPiiBox(int type, int capacity);
    ~CPiiBox();

private:
    int mType;
    int mCapacity;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPersonalDataList;
};

#endif
