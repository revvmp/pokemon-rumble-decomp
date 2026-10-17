typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} va_list[1];

typedef enum _va_arg_type {
    arg_ARGPOINTER,
    arg_WORD,
    arg_DOUBLEWORD,
    arg_REAL,
    arg_VECTOR
} _va_arg_type;

#define ALIGN(addr, amount) (((unsigned int)(addr) + ((amount) - 1)) & ~((amount) - 1))

void* __va_arg(va_list ap, _va_arg_type type) {
    char* addr;
    char* reg = &ap->gpr;
    int regIndex = ap->gpr;
    int maxRegs = 8;
    int size = 4;
    int increment = 1;
    int even = 0;
    int fprOffset = 0;
    int regSize = 4;

    if (type == arg_REAL) {
        reg = &ap->fpr;
        regIndex = ap->fpr;
        size = 8;
        fprOffset = 32;
        regSize = 8;
    }

    if (type == arg_DOUBLEWORD) {
        size = 8;
        maxRegs = maxRegs - 1;

        if (regIndex & 1) {
            even = 1;
        }

        increment = 2;
    }

    if (regIndex < maxRegs) {
        regIndex += even;
        addr = ap->reg_save_area + fprOffset + regIndex * regSize;
        *reg = regIndex + increment;
    } else {
        *reg = 8;
        addr = ap->input_arg_area;
        addr = (char*)ALIGN(addr, size);
        ap->input_arg_area = addr + size;
    }

    if (type == arg_ARGPOINTER) {
        addr = *(char**)addr;
    }

    return addr;
}
