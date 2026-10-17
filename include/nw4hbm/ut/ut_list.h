#ifndef NW4HBM_UT_LIST_H
#define NW4HBM_UT_LIST_H
#include "nw4hbm/types_nw4hbm.h"

namespace nw4hbm {
namespace ut {

struct List {
    void* headObject;
    void* tailObject;
    u16 numObjects;
    u16 offset;
};

struct Link {
    void* prevObject;
    void* nextObject;
};

void List_Init(List* list, u16 offset);

void List_Append(List* list, void* object);
void List_Prepend(List* list, void* object);

void List_Insert(List* list, void* next, void* object);
void List_Remove(List* list, void* object);

void* List_GetNext(const List* list, const void* object);
inline const void* List_GetNextConst(const List* pList, const void* pObject) {
    return List_GetNext(pList, pObject);
}

void* List_GetPrev(const List* list, const void* object);
inline const void* List_GetPrevConst(const List* pList, const void* pObject) {
    return List_GetPrev(pList, pObject);
}

void* List_GetNth(const List* list, u16 index);
inline const void* List_GetNthConst(const List* pList, u16 n) {
    return List_GetNth(pList, n);
}

inline void* List_GetFirst(const List* list) {
    return List_GetNext(list, NULL);
}
inline const void* List_GetFirstConst(const List* list) {
    return List_GetFirst(list);
}

inline void* List_GetLast(const List* list) {
    return List_GetPrev(list, NULL);
}
inline const void* List_GetLastConst(const List* list) {
    return List_GetLast(list);
}
inline u16 List_GetSize(const List* list) {
    return list->numObjects;
}

#define NW4R_UT_LIST_LINK_DECL() nw4hbm::ut::Link link

#define NW4R_UT_LIST_INIT(LIST, T)                                             \
    nw4hbm::ut::List_Init(&(LIST), offsetof(T, link))

#define NW4R_UT_LIST_GET_LINK(LIST, OBJ)                                       \
    reinterpret_cast<nw4hbm::ut::Link*>((u8*)(OBJ) + (LIST).offset)

#define NW4R_UT_LIST_FOREACH(TYPE, NAME, LIST, ...)                            \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4hbm::ut::List_GetNext(&(LIST), NAME))) != NULL) {       \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

#define NW4R_UT_LIST_FOREACH_REV(TYPE, NAME, LIST, ...)                        \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4hbm::ut::List_GetPrev(&(LIST), NAME))) != NULL) {       \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

#define NW4R_UT_LIST_FOREACH_SAFE(TYPE, NAME, LIST, ...)                       \
    {                                                                          \
        TYPE* NAME;                                                            \
        TYPE* __next__;                                                        \
                                                                               \
        for (NAME = static_cast<TYPE*>(nw4hbm::ut::List_GetFirst(&(LIST)));    \
             NAME != NULL; NAME = __next__) {                                  \
                                                                               \
            __next__ =                                                         \
                static_cast<TYPE*>(nw4hbm::ut::List_GetNext(&(LIST), NAME));   \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

}
}

#endif
