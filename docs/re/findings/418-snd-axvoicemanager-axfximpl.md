# F-418-1: NW4R snd AxVoiceManager and AxfxImpl units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #418 owns `.text` `0x80275880–0x80276F94`. Its first eight functions close `snd_AxVoice.cpp` ([F-417-1](417-snd-axvoice.md)). The rest form `snd_AxVoiceManager.cpp` and the first four functions of `snd_AxfxImpl.cpp`, whose last two functions lie in #419's estimated range. This completes #418.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_AxVoiceManager.cpp` | `.text` | `0x80276410–0x80276E38` | `GetInstance`, the implicit destructor, `GetRequiredMemSize`, `Setup`, `Shutdown`, `AcquireAxVoice`, `FreeAxVoice`, `ReserveForFreeAxVoice`, `FreeAllReservedAxVoice` |
| | `.bss` | `0x8044D390–0x8044D3C8` | destructor chain entry (0xC) and the local `instance` (0x2C) |
| | `.sbss` | `0x804B9338–0x804B9340` | `instance` guard and padding |
| `nw4r/snd/snd_AxfxImpl.cpp` | `.text` | `0x80276E40–0x80277038` | `CreateHeap`, `DestroyHeap`, `HookAlloc`, `RestoreAlloc`, `Alloc`, `Free` |
| | `.sbss` | `0x804B9340–0x804B9348` | `mCurrentFx`, `mAllocatedSize` |

The constructor, `Alloc`, `Free`, and `ReserveForFree` of `AxVoiceManager` are inlined into their callers and removed by the linker. The next function, `0x80277040`, belongs to #419 and the following unit.

Both units are `Matching`: objdiff reports 100% for all 9 and 6 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision

Both files match the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`); the source keeps the ogws style (CC0, commit `27fa94a593096eed7442efc4e70981fd52025b9c`). Differences from ogws:

- `AxVoiceManager::GetRequiredMemSize` takes the voice count, `(count + 16) * sizeof(AxVoice)`, instead of calling `AXGetMaxVoices`.
- `Setup` constructs each voice with placement new before adding it to the free list (the target calls `AxVoice::AxVoice` in the loop).
- `AxfxImpl` has no `mIsActive` member: `mHeap` is at 0x0 and `mAllocCount` at 0x4 (`Alloc` and `Free` access them there).
- `CreateHeap` rejects a null buffer or zero size and clears `mHeap`, `DestroyHeap` clears `mHeap`, `HookAlloc` resets `mAllocatedSize`, and `RestoreAlloc` returns it.

## Headers

`include/nw4r/snd/snd_AxfxImpl.h` is written for this layout. ogws's `HEAP_SIZE_MIN` and `GetHeapTotalSize` are omitted: the project's MEM headers have no `MEM_FRM_HEAP_MIN_SIZE` or `MEMGetHeapTotalSize` yet, and the owner of the effect units should add them with evidence. `snd_AxVoiceManager.h` declares `GetRequiredMemSize(int)`.

`include/revolution/mem/frameHeap.h` gains the SDK inline wrappers `MEMCreateFrmHeap` (option flags 0) and `MEMAllocFromFrmHeap` (alignment 4); the target calls the `Ex` functions with exactly those arguments. The project has no MSL `<new>`, so `snd_AxVoiceManager.cpp` defines the inline placement `operator new` locally, as the HBM sources do.

## Outside symbols

None were renamed beyond the units' own symbols.

## Traps

- **`revolution/mem/heapCommon.h` defines a `RoundUp` macro.** Including it before `nw4r/ut/ut_algorithm.h` macro-expands the `ut::RoundUp` template and fails with `')' expected`. `snd_AxfxImpl.cpp` includes `ut_algorithm.h` first and calls `ut::RoundUp<u32>`, which the function-like macro does not match.
