#ifndef NWC24MBOXCTRL_H
#define NWC24MBOXCTRL_H

#include "revolution/nwc24.h"

#ifdef __cplusplus
extern "C" {
#endif

NWC24Err NWC24iOpenMBox(void);

typedef struct MountInfoStruct {
      u32 count;
      s32 type;
} MountInfoStruct;

typedef struct MBoxControlHeader {
      u32 magic;
      u32 version;
      u32 msgCount;
      u32 capacity;
      u32 totalMsgSize;
      u32 mailDataOffset;
      u32 nextMsgId;
      u32 nextFreeEntry;
      u32 oldestMsgId;
      u32 freeSpace;
      char padding[0x58];
} MBoxControlHeader;

typedef struct MBoxControlEntry {
      u32 id;
      u32 flags;
      u32 length;
      u32 appId;
      u32 UNK_0x10;
      u32 tag;
      u32 ledPattern;
      u32 nextFreeOrCreationMs;
      u64 fromId;
      u32 createTime;
      u32 UNK_0x2C;
      u8 numTo;
      u8 numAttached;
      u16 groupId;
      u32 packedSubjectText;
      u32 packedTextSubjectSize;
      u32 packedSubjectTextSize;
      u32 packedTextSizeContentType;
      u32 packedContentTypeTransferEnc;
      u32 textPtr;
      u32 textSize;
      u32 attached0Ptr;
      u32 attached0Size;
      u32 attached1Ptr;
      u32 attached1Size;
      u32 attached0OrigSize;
      u32 attached1OrigSize;
      u32 attached0_type;
      u32 attached1_type;
      u32 textOrigSize;
      u32 UNK_0x74;
      u32 UNK_0x78;
      u32 UNK_0x7C;
} MBoxControlEntry;

#ifdef __cplusplus
}
#endif

#endif
