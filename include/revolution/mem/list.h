#ifndef REVOLUTION_MEM_LIST_H
#define REVOLUTION_MEM_LIST_H

#include <stddef.h>
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMLink {
    void* prevObject;
    void* nextObject;
} MEMLink;

typedef struct MEMList {
    void* headObject;
    void* tailObject;
    u16 numObjects;
    u16 offset;
} MEMList;

#define MEM_INIT_LIST(list, structName, linkName) MEMInitList((list), offsetof(structName, linkName))

void MEMInitList(MEMList* list, u16 offset);
void MEMAppendListObject(MEMList* list, void* object);
void MEMRemoveListObject(MEMList* list, void* object);
void* MEMGetNextListObject(MEMList* list, void* object);

#ifdef __cplusplus
}
#endif

#endif
