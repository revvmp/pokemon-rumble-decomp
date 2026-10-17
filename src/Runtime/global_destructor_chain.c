typedef struct DestructorChain {
    struct DestructorChain* next;
    void* dtor;
    void* obj;
} DestructorChain;

DestructorChain* __global_destructor_chain;

void* __register_global_object(void* obj, void* dtor, void* regmem) {
    DestructorChain* chain = (DestructorChain*)regmem;

    chain->next = __global_destructor_chain;
    chain->dtor = dtor;
    chain->obj = obj;
    __global_destructor_chain = chain;
    return obj;
}

void __destroy_global_chain(void) {
    DestructorChain* chain;

    while ((chain = __global_destructor_chain) != 0) {
        __global_destructor_chain = chain->next;
        ((void (*)(void*, short))chain->dtor)(chain->obj, -1);
    }
}
