#ifndef NWC24STRUCTS_H
#define NWC24STRUCTS_H

typedef struct NWC24WorkMem {
    char StrBuffer_LineTmp[1024];
    char StrBuffer_WorkTmp[1024];
    char StrBuffer_FilePath[256];
    char FileRdBuffer[512];
    char FileWrBuffer[2048];
    char ConfigCache[1024];
    char MBCHdCache_SendBox[128];
    char MBCHdCache_RecvBox[128];
    char MsgObjCache[256];
    char MimeTable[256];
    char SearchWork[4096];
    char FLHdCache[1024];
    char DWCFLHdCache[2048];
    char DLHdCache[2048];
    char DLBuffer[512];
} NWC24WorkMem;

#endif
