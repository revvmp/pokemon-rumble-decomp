#ifndef GAME_DCIM_H
#define GAME_DCIM_H

enum DcfIndexResult {
    DcfIndexFull = 0,
    DcfRootMissing = 1,
    DcfCreateDirectory = 2,
    DcfUseDirectory = 3,
    DcfIoError = 4
};

DcfIndexResult FindNextDcfIndex(int* pDirectory, int* pFile, bool forceNewDirectory);

#endif
