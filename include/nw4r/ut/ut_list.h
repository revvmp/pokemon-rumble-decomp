#ifndef NW4R_UT_LIST_H
#define NW4R_UT_LIST_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
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

void List_Init(List* pList, u16 offset);

void List_Append(List* pList, void* pObject);
void List_Prepend(List* pList, void* pObject);

void List_Insert(List* pList, void* pTarget, void* pObject);
void List_Remove(List* pList, void* pObject);

void* List_GetNext(const List* pList, const void* pObject);
inline const void* List_GetNextConst(const List* pList, const void* pObject) {
    return List_GetNext(pList, pObject);
}

void* List_GetPrev(const List* pList, const void* pObject);
inline const void* List_GetPrevConst(const List* pList, const void* pObject) {
    return List_GetPrev(pList, pObject);
}

void* List_GetNth(const List* pList, u16 n);
inline const void* List_GetNthConst(const List* pList, u16 n) {
    return List_GetNth(pList, n);
}

inline void* List_GetFirst(const List* pList) {
    return List_GetNext(pList, NULL);
}
inline const void* List_GetFirstConst(const List* pList) {
    return List_GetFirst(pList);
}

inline void* List_GetLast(const List* pList) {
    return List_GetPrev(pList, NULL);
}
inline const void* List_GetLastConst(const List* pList) {
    return List_GetLast(pList);
}

inline u16 List_GetSize(const List* pList) {
    return pList->numObjects;
}

#define NW4R_UT_LIST_LINK_DECL() nw4r::ut::Link link

#define NW4R_UT_LIST_INIT(LIST, T)                                             \
    nw4r::ut::List_Init(&(LIST), offsetof(T, link))

#define NW4R_UT_LIST_GET_LINK(LIST, OBJ)                                       \
    reinterpret_cast<nw4r::ut::Link*>((u8*)(OBJ) + (LIST).offset)

#define NW4R_UT_LIST_FOREACH(TYPE, NAME, LIST, ...)                            \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4r::ut::List_GetNext(&(LIST), NAME))) != NULL) {         \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

#define NW4R_UT_LIST_FOREACH_REV(TYPE, NAME, LIST, ...)                        \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4r::ut::List_GetPrev(&(LIST), NAME))) != NULL) {         \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

#define NW4R_UT_LIST_FOREACH_SAFE(TYPE, NAME, LIST, ...)                       \
    {                                                                          \
        TYPE* NAME;                                                            \
        TYPE* __next__;                                                        \
                                                                               \
        for (NAME = static_cast<TYPE*>(nw4r::ut::List_GetFirst(&(LIST)));      \
             NAME != NULL; NAME = __next__) {                                  \
                                                                               \
            __next__ =                                                         \
                static_cast<TYPE*>(nw4r::ut::List_GetNext(&(LIST), NAME));     \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

}
}

#endif
