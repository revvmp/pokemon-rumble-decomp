# Sprint a15: script bindings units

## Traps

- **Bindings keep the base engine parameter.** A `CPpuEngine*` local changes register allocation; take `NPpu::CEngine* pEngine` and cast at each `PopWorldObj`/`GetWorld` call (moveTo, isInKeepOutArea, camera and world-object bindings).
- **A for loop with a pointer and a counter matches the original element scan.** `CountMergedIds` matched only with the pointer declared before the counter and `++pSlice, ++count` in the increment.
- **String-pop-first bindings stay about 4% off.** Ordinary play, request, requestNext and getMotionLength bindings of camera, light set, null object and model units (16 functions) use `lwzux` and compute the Slice address late in the original. Tried without effect: member `PopString`, free `PopValue`, returned-by-value `Pop`, `CValue(mStack[...])` temporaries, pointer-returning `Pop`, copy-initialised locals.
- **The world-object bindings share one .sdata2 pool from 0x804BA280.** Splitting the pool across units made every float load differ by symbol; one unit `ScriptWorldObj.cpp` covers 0x8014B030-0x8014FB10.
- **Auto-pairing callee names can duplicate a global name.** Two addresses given the same mangled name fail the original-object link (`multiply-defined`); check duplicates in symbols.txt after bulk renames.
- **File-local statics that share a global name elsewhere need `scope:local`** in symbols.txt (`Format` overloads in ScriptMessages).
- **`std::list<BWorldObj*>` iterators are returned in r3 in the original.** The repo `include/list` returns them through a hidden pointer, leaving PushObjectIds, MergeObjectIds and the `std::set<int>` and list helpers at 0x8014DA60-0x8014E5F0 unmatched.
