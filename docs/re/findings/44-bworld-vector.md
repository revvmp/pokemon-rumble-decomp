# F-44-1: MSL vector layering and the BWorld member helpers

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #44 covers `0x8005B820`–`0x8005D364` in `game/BWorld.cpp`: the member constructors and destructors that the `BWorld` constructor calls, the vector instances it uses, the `BWorld` destructor and three flag setters.

## Matched functions

- The three vector destructors: `vector<weak_ptr<BWorldCamera>>` (`0x8005B860`), `vector<BWorld::ObjEntry>` (`0x8005BB40`) and `vector<shared_ptr<CEffect>>` (`0x8005C1F0`).
- `BWorld::EnableCalc(bool)` (`0x8005D300`), `RequestPause` (`0x8005D330`, sets flag 0x10 and clears 0x20) and `RequestResume` (`0x8005D350`, the reverse).
- `shared_ptr<BWorld::EffectData>::reset` (`0x8005B360`, issue #43), whose dead deletion path inlines the effect vector destructor.

The member constructors at `0x8005B820`–`0x8005BC20` and `0x8005C170`–`0x8005C9E0` already matched with #43.

## Vector structure

`include/vector` now follows the MSL layering that the destructors show:

- `__vec_deleter<T, Allocator>` holds the data pointer, size and a `compressed_pair` of capacity and allocator. Its destructor is defined outside the class, so the compiler emits it at the end of the unit (`dtor_800648B0` is the `ObjEntry` instance, with one `this` check). The vector constructor's `DESTROYBASE` action names it.
- `__vec_constructor<T, Allocator, IsPOD>` derives from it and holds the allocation, construction and growth code; `vector` derives privately from it. The inlined vector destructor therefore checks `this` three times before testing the data pointer.
- `__vec_split_buffer<T, Allocator>` is the temporary used when `resize` reallocates. It derives from `__vec_deleter<T, Allocator&>`, so the allocator is held by reference, and adds the start offset at 0x10. The original destructor of this temporary is out of line too (`dtor_800660C0`).
- Every element-wise helper (`erase_at_end`, `allocate`, `construct_at_end`, `construct_at_front`, `swap`, `append`) takes a `Metrowerks::int2type<is_POD<T>::value>` tag by value. The tag is what leaves the `stb 0` stores to one-byte stack temporaries in the original; without them the stack layout shifts. The POD form of `erase_at_end` only subtracts from the size, as `dtor_80064840` shows.
- `std::length_error` is thrown from a file-local helper. The original keeps one `vector length error` string per unit, placed among that unit's local strings, and holds its address in a saved register; a literal inside the template members instead becomes one weak `@STRING@` object per instance, and its address is reloaded.

`std::logic_error` stores its message in a `shared_ptr<char>` with `default_delete<char[]>`, which matches the deleter type string at `.data` `0x803998C0`.

## Status

objdiff reports 66 of 196 unit functions at 100%. In this issue's range the following remain:

- `vector<ObjEntry>(size_type)` (`0x8005B950`) and `vector<shared_ptr<CEffect>>::resize` (`0x8005C2D0`) have the original control flow, but their frames are 0x20 and 0x50 bytes smaller. The original also stores `r1` to a stack slot each time it copies a `shared_ptr` into new storage. Neither difference has been reproduced: `throw()` on the `shared_ptr` constructors, on `allocator::construct` or on placement `new` changes nothing.
- The `vector` default constructor references the `__vec_constructor` destructor, so the compiler emits that destructor after it. The original does not. The function bodies are unaffected, but the unit cannot link as `Matching` until this is resolved.
- `set<shared_ptr<BWorldObj>>` destructor (`0x8005BC40`): the original checks `this` four times, one more than the current `set`/`tree`/`tree_imp` layering produces. The same header serves `BModelObj`, whose `DrawLayerSet` wrapper supplies a fourth level there, so the shared header is unchanged until the layering is settled.
- `BWorld::~BWorld` (`0x8005C9F0`, 0x90C bytes) is not reconstructed yet.

The unit stays `NonMatching`; the full DOL verification passes with the original object.

## Traps

- **Empty tag arguments leave byte stores.** Symptom: the original has `li r0, 0; stb r0, N(r31)` before a loop or check that has no byte variable. Pass an `int2type` tag by value to the inlined helper.
- **A string literal in a template member becomes a weak per-instance object.** Symptom: the original shares one string between several template instances and keeps its address in a saved register. Throw from a file-local function that holds the literal.
- **The `DESTROYBASE` dtor shows which base destructor is out of line.** Symptom: a constructor's exception action names a destructor emitted at the end of the unit. Define that base destructor outside the class.
