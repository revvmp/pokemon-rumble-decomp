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

# F-424-2: NW4R snd MemorySoundArchive unit

`nw4r/snd/snd_MemorySoundArchive.cpp` follows `snd_Lfo.cpp` and ends with the first part of #425's range.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8027C8D0–0x8027CD70` | constructor, destructor, `Setup`, `detail_GetFileAddress`, `detail_GetWaveDataFileAddress`, `OpenStream`, `OpenExtStream`, `detail_GetRequiredStreamBufferSize`; `MemoryFileStream::Close`, `Read`, `Seek`; weak `FileStream::GetRuntimeTypeInfo` and the `MemoryFileStream` inline virtuals and destructor |
| `.data` | `0x803C62C0–0x803C6340` | `MemoryFileStream` vtable (0x60) and `MemorySoundArchive` vtable (0x20) |

The unit is `Matching`: objdiff reports 100% code and data for all 20 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`). `Shutdown` is compiled but dead-stripped.

## Reconstruction

The code is the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) without changes to code generation. Both vtables confirm its virtual order: `SoundArchive`'s destructor, `detail_GetFileAddress`, `detail_GetWaveDataFileAddress`, `detail_GetRequiredStreamBufferSize`, `OpenStream`, `OpenExtStream`; `MemoryFileStream` overrides `Close`, `Read`, `Seek`, `CanAsync`, `CanRead`, `CanWrite`, `GetSize`, `CanSeek`, `CanCancel`, and `Tell`, and inherits the rest from `ut::IOStream` and `ut::FileStream`. Neither class carries RTTI here: the vtables' first two words are zero, and the stream's `GetRuntimeTypeInfo` slot holds `FileStream`'s.

`MemoryFileStream`'s constructor is defined in the class. Defined out of line, as in zeldaret/ss, the compiler also emits weak `IOStream` and `FileStream` destructors after it, which the target range does not contain. `Seek` is 0x48 bytes; dtk split its trailing unreachable `blr` into `fn_8027CCA4`, which is removed.

The new header `snd_MemorySoundArchive.h` follows zeldaret/ss. `snd_SoundArchive.h` and `snd_SoundArchiveFile.h` (with `SoundArchiveFileReader`) are taken unchanged from the #426 interface pull request (#817), so both changes add identical files; this unit matches with them as it did with a direct zeldaret/ss import. The target confirms `SoundArchive` at 0x108 bytes, with `mFileReader` at 0x4 and `mExtFileRoot` from 0x8 (the constructor stores `'/'` and a terminator), and the reader's 0x44 bytes: its constructor clears 0x28 to 0x40 and `Setup` reads the info and symbol offsets and sizes at 0x18, 0x1C, 0x10, and 0x14 of the header copy.

## Outside symbols

Renamed: six `SoundArchive` functions and four `SoundArchiveFileReader` functions called from this unit (see `docs/re/SYMBOLS.md`), and the game-side weak `IOStream::GetSizeAlign` (`0x801A4DD0`) and `GetBufferAlign` (`0x801A4DE0`) that the `MemoryFileStream` vtable references. Naming those lets the linker discard this unit's weak copies.

## Open: `IOStream::GetOffsetAlign` at `0x8027C8C0`

The 8-byte function before the constructor is the weak `ut::IOStream::GetOffsetAlign`: it fills vtable slot 11 of `MemoryFileStream` and returns 1. No game-side copy exists, so this unit's copy is the one the linker keeps, but the compiler emits it among the trailing weak functions, after `Seek`, not before the constructor. zeldaret/ss has no function between `snd_Lfo.cpp` and the constructor either. A probe shows that a direct reference from a function makes the compiler emit the weak function immediately after that function. The original file therefore probably began with a function, dead-stripped in this game, that called `GetOffsetAlign`, but no reference shows it. The split starts at `0x8027C8D0`, and `0x8027C8C0` stays in the fallback assembly named `GetOffsetAlign__Q34nw4r2ut8IOStreamCFv`, which displaces the unit's weak copy.

## Traps

- **A weak inline can be emitted before a unit's first function.** CodeWarrior emits a weak inline function right after the first function that references it in code, and otherwise among the trailing weak functions. If a weak copy sits before a unit's first function, look for a dead-stripped caller at the head of the original file rather than another source unit.
- **Out-of-line constructors can emit base destructors.** An out-of-line `MemoryFileStream` constructor emitted weak `IOStream::~IOStream` and `FileStream::~FileStream` after itself; the in-class constructor did not.
- **Nested-class arguments mangle with their full qualification.** `SoundArchive::GroupInfo*` mangles as `PQ44nw4r3snd12SoundArchive9GroupInfo`, not `PQ34...`; a `Q3` prefix in `symbols.txt` gives undefined-symbol link errors.

## Ordinary inline placement controls, 2026-10-20

The source-only acceptance of the separate EF stripe units showed that ordinary inline declaration and definition placement can affect retained weak-function order. Two bounded controls tested whether that lesson resolves this unit's remaining `IOStream::GetOffsetAlign` at `0x8027C8C0`.

Both controls compile the current `snd_MemorySoundArchive.cpp` with the established Wii/1.1 options, without changing compiler flags. First, the existing getter body was moved from its class to an ordinary out-of-class inline definition, tested before the first source body and after the final source body. Second, the existing nested `MemoryFileStream` constructor was declared in its class and defined inline at the same two positions. No signature, field, vtable entry, function behavior or new caller was introduced.

All four compilations exit 0. In every object, `GetOffsetAlign` remains a weak function at `.text+0x4D0`, after `Seek`, rather than before the archive constructor. The late nested-constructor definition moves the discarded base destructors after `Seek`; it does not move the getter. These results reject these specific placement repairs and do not establish a general restriction on CodeWarrior inline emission.

The controls and their complete objects are retained outside the checkout in `424-inline-placement.py`, `424-constructor-placement.py`, and `424-probe/ordinary-inline/` and `424-probe/ordinary-constructor/`. The `nm -n` outputs establish the observed order. No canonical source or header change was accepted, no source-selected build was attempted, and no new source credit or original-issue closure is claimed. The missing earlier caller or source-unit evidence described above remains the blocker.

## D-0016 early getter emission, 2026-11-04

The ordinary placement controls above did not recover an original early caller. Under D-0016, the source now declares a deliberately unreferenced `GetMemoryStreamOffsetAlignment` function before the archive constructor. It calls the qualified `ut::IOStream::GetOffsetAlign` under a local `dont_inline` pragma. The compiler emits its four-byte branch, then the eight-byte getter, then the archive constructor. The linker is expected to discard the unreferenced probe while keeping the getter through the stream vtable.

A direct Wii/1.1 compile with the exact configured NW4R flags confirms that all existing 20 functions remain 100% and that getter ordering is correct. No header, symbol name, behavior of retained functions or original data was changed. The probe is a documented fake, not a recovered original function. The maintainer extended the text split to `0x8027C8C0`, rebuilt successfully, and verified the full DOL against its frozen SHA: PASS. The unit now has 21/21 exact functions. The synthetic caller is dead-stripped and the getter is retained at the original address.

After this acceptance, an audit of the generated report found no non-padding function in an automatic fallback unit: every remaining real function belongs to a named source unit. Automatic padding units still exist and are deliberately excluded from this conclusion. This is an ownership inventory, not a claim that every named unit is source-linked or that every function matches.
