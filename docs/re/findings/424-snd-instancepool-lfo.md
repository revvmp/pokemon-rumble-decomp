# F-424-1: NW4R snd InstancePool and Lfo units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #424 owns `.text` `0x8027C050–0x8027CBF4`. Its first three functions close `snd_FxReverbStd.cpp` ([F-423-1](423-snd-fx.md)). This finding adds the next two units. The rest of the range, from `0x8027C8C0`, belongs to `snd_MemorySoundArchive.cpp`, which continues into #425 and stays open.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_InstancePool.cpp` | `.text` | `0x8027C3C0–0x8027C6A8` | `PoolImpl::CreateImpl`, `DestroyImpl`, `CountImpl`, `AllocImpl`, `FreeImpl` |
| `nw4r/snd/snd_Lfo.cpp` | `.text` | `0x8027C6B0–0x8027C8B8` | `LfoParam::Init`, `Lfo::Reset`, `Lfo::Update`, `Lfo::GetValue`, `Lfo::GetSinIdx` |
| | `.rodata` | `0x8036E110–0x8036E138` | `GetSinIdx` function-local `sinTable` (33 entries) and padding |
| | `.sdata2` | `0x804BAEB8–0x804BAEE8` | float constants |

Both units are `Matching`: objdiff reports 100% code for 5 and 5 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision and reconstruction

Both units follow zeldaret/ss (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) without changes to code generation. `PoolImpl` threads a free list through the pool buffer under `ut::AutoInterruptLock`; `CreateImpl` rounds the buffer and stride to four bytes. `Lfo::GetValue` scales the sine table index by `4 * TABLE_SIZE` and divides by `TABLE_SIZE * 4 - 1`. The existing `snd_InstancePool.h` and `snd_Lfo.h` headers were sufficient.

`snd_MemorySoundArchive.cpp` starts with a weak `IOStream::GetOffsetAlign` copy at `0x8027C8C0` and needs `SoundArchive` and `SoundArchiveFileReader` headers that the project does not have yet; it was left to a later claim together with #425.

## Outside symbols

Renamed: `CountImpl` (`0x8027C5A0`, previously `fn_`) and the `sinTable` local (`0x8036E110`, previously `lbl_`).

## Traps

- **`RoundUp` collides with the MEM SDK macro.** `revolution/mem/heapCommon.h` defines a `RoundUp` macro; if it is included before `nw4r/ut/ut_algorithm.h`, the `ut::RoundUp` templates are macro-expanded. Include `ut_algorithm.h` first, as `snd_InstancePool.cpp` does (also seen in F-423-1).
