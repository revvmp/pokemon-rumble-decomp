#include <stddef.h>
#include <typeinfo>

typedef struct TypeInfo TypeInfo;

typedef struct TypeInfoBaseList {
    TypeInfo* baseTypeInfo;
    long offset;
} TypeInfoBaseList;

typedef struct TypeInfoAmbigHead {
    TypeInfo* baseTypeInfo;
    long offset;
    long bases;
} TypeInfoAmbigHead;

struct TypeInfo {
    const char* typeName;
    TypeInfoBaseList* baseList;
};

typedef struct RTTIVTableHeader {
    TypeInfo* typeInfo;
    long offset;
} RTTIVTableHeader;

extern "C" {
void* __get_typeid(void* obj, long offset);
void* __dynamic_cast(void* obj, int offset, TypeInfo* typeInfo, TypeInfo* subtypeInfo, short isReference);
}

static TypeInfo unknown_type = {"???"};

static int strequal(const char* s1, const char* s2) {
    for (; *s1 == *s2; s1++, s2++) {
        if (*s1 == 0) {
            return 0;
        }
    }

    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void* __get_typeid(void* obj, long offset) {
    if (obj == NULL) {
        throw std::bad_typeid();
    }

    if ((obj = **(void***)((char*)obj + offset)) == 0) {
        return &unknown_type;
    }

    return obj;
}

void* __dynamic_cast(void* obj, int offset, TypeInfo* typeInfo, TypeInfo* subtypeInfo, short isReference) {
    RTTIVTableHeader* header;
    TypeInfoBaseList* list;
    void* result;
    int i;
    int bases;
    long baseOffset;

    if (obj == NULL) {
        return NULL;
    }

    header = *(RTTIVTableHeader**)((char*)obj + offset);

    if (header->typeInfo != NULL) {
        result = (char*)obj + header->offset;

        if (typeInfo == NULL || strequal(header->typeInfo->typeName, typeInfo->typeName) == 0) {
            return result;
        }

        list = header->typeInfo->baseList;

        if (list != NULL) {
            for (; list->baseTypeInfo; list++) {
                if (list->offset & 0x80000000) {
                    baseOffset = list->offset & 0x7FFFFFFF;
                    bases = ((TypeInfoAmbigHead*)list)->bases;

                    if (header->offset + baseOffset == 0 && strequal(list->baseTypeInfo->typeName, typeInfo->typeName) == 0) {
                        list = (TypeInfoBaseList*)((TypeInfoAmbigHead*)list + 1);

                        for (i = 0; i < bases; i++) {
                            if (header->offset + list->offset == 0 && strequal(list->baseTypeInfo->typeName, subtypeInfo->typeName) == 0) {
                                return (char*)result + list->offset;
                            }
                            list++;
                        }

                        break;
                    } else {
                        list = (TypeInfoBaseList*)((TypeInfoAmbigHead*)list + 1);
                        list += bases - 1;
                    }
                } else if (strequal(list->baseTypeInfo->typeName, typeInfo->typeName) == 0) {
                    return (char*)result + list->offset;
                }
            }
        }
    }

    if (isReference) {
        throw std::bad_cast();
    }

    return NULL;
}
