#include "revolution/os/OSAlloc.h"

#define TRUNC(n, a) (((u32)(n)) & ~((a) - 1))
#define ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))

#define MIN_CELL_SIZE 64

volatile OSHeapHandle __OSCurrHeap = -1;

typedef struct Cell Cell;
typedef struct HeapDesc HeapDesc;

struct Cell {
    Cell* prev;
    Cell* next;
    long size;
};

struct HeapDesc {
    long size;
    Cell* free;
    Cell* allocated;
};

static HeapDesc* HeapArray;
static int NumHeaps;
static void* ArenaStart;
static void* ArenaEnd;

static Cell* DLAddFront(Cell* list, Cell* cell) {
    cell->next = list;
    cell->prev = NULL;

    if (list != NULL) {
        list->prev = cell;
    }

    return cell;
}

static Cell* DLExtract(Cell* list, Cell* cell) {
    if (cell->next) {
        cell->next->prev = cell->prev;
    }

    if (cell->prev == NULL) {
        return cell->next;
    } else {
        cell->prev->next = cell->next;
        return list;
    }
}

static Cell* DLInsert(Cell* list, Cell* cell) {
    Cell* prev;
    Cell* next;

    for (next = list, prev = NULL; next; prev = next, next = next->next) {
        if (cell <= next) {
            break;
        }
    }

    cell->next = next;
    cell->prev = prev;

    if (next != NULL) {
        next->prev = cell;

        if ((char*)cell + cell->size == (char*)next) {
            cell->size += next->size;
            cell->next = next = next->next;

            if (next != NULL) {
                next->prev = cell;
            }
        }
    }

    if (prev != NULL) {
        prev->next = cell;

        if ((char*)prev + prev->size == (char*)cell) {
            prev->size += cell->size;
            prev->next = next;

            if (next != NULL) {
                next->prev = prev;
            }
        }

        return list;
    } else {
        return cell;
    }
}

void* OSAllocFromHeap(OSHeapHandle heap, u32 size) {
    HeapDesc* desc;
    Cell* cell;
    Cell* newCell;
    long leftSize;

    desc = &HeapArray[heap];
    size += 32;
    size = ROUND(size, 32);

    for (cell = desc->free; cell != NULL; cell = cell->next) {
        if ((long)size <= cell->size) {
            break;
        }
    }

    if (cell == NULL) {
        return NULL;
    }

    leftSize = cell->size - size;

    if (leftSize < (u32)MIN_CELL_SIZE) {
        desc->free = DLExtract(desc->free, cell);
    } else {
        cell->size = size;
        newCell = (Cell*)((char*)cell + size);
        newCell->size = leftSize;
        newCell->prev = cell->prev;
        newCell->next = cell->next;

        if (newCell->next != NULL) {
            newCell->next->prev = newCell;
        }

        if (newCell->prev != NULL) {
            newCell->prev->next = newCell;
        } else {
            desc->free = newCell;
        }
    }

    desc->allocated = DLAddFront(desc->allocated, cell);
    return (void*)((char*)cell + 32);
}

void OSFreeToHeap(OSHeapHandle heap, void* ptr) {
    HeapDesc* desc;
    Cell* cell;

    cell = (Cell*)((char*)ptr - 32);
    desc = &HeapArray[heap];
    desc->allocated = DLExtract(desc->allocated, cell);
    desc->free = DLInsert(desc->free, cell);
}

OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap) {
    OSHeapHandle prev;
    prev = __OSCurrHeap;
    __OSCurrHeap = heap;
    return prev;
}

void* OSInitAlloc(void* arenaStart, void* arenaEnd, int maxHeaps) {
    u32 size;
    int i;
    size = maxHeaps * sizeof(HeapDesc);
    HeapArray = arenaStart;
    NumHeaps = maxHeaps;

    for (i = 0; i < NumHeaps; i++) {
        HeapDesc* desc = &HeapArray[i];
        desc->size = -1;
        desc->free = desc->allocated = NULL;
    }

    __OSCurrHeap = -1;
    arenaStart = (void*)((char*)HeapArray + size);
    arenaStart = (void*)ROUND(arenaStart, 32);
    ArenaStart = arenaStart;
    ArenaEnd = (void*)TRUNC(arenaEnd, 32);
    return arenaStart;
}

OSHeapHandle OSCreateHeap(void* start, void* end) {
    OSHeapHandle heap;
    HeapDesc* desc;
    Cell* cell;
    start = (void*)ROUND(start, 32);
    end = (void*)TRUNC(end, 32);

    for (heap = 0; heap < NumHeaps; heap++) {
        desc = &HeapArray[heap];

        if (desc->size < 0) {
            desc->size = (char*)end - (char*)start;
            cell = (Cell*)start;
            cell->prev = NULL;
            cell->next = NULL;
            cell->size = desc->size;
            desc->free = cell;
            desc->allocated = NULL;
            return heap;
        }
    }

    return -1;
}
