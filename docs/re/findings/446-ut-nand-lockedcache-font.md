# F-446-1: NW4R ut DvdFileStream, NandFileStream, LockedCache, and Font units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #446 owns `.text` `0x80293CD0–0x80294594`, 27 functions. #445 already covered the first three (`FileStream::FilePosition::Append`, `Seek`, and `__sinit_\ut_FileStream_cpp`). This finding reconstructs four further units, covering 23 more functions. The last function, `RomFont::RomFont` at `0x80294540`, opens `ut_RomFont.cpp`, which continues into #447's range; [F-447-1](447-ut-romfont.md) reconstructs it and completes this issue.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ut/ut_DvdFileStream.cpp` | `.text` | `0x80293DC0–0x80293DCC` | `__sinit_\ut_DvdFileStream_cpp` |
| | `.ctors` | `0x80352A74–0x80352A78` | initializer entry |
| | `.sbss` | `0x804B93C8–0x804B93D0` | `DvdFileStream::typeInfo` and padding |
| `nw4r/ut/ut_NandFileStream.cpp` | `.text` | `0x80293DD0–0x802942AC` | `NandAsyncCallback_`, constructor, destructor, `Open`, `Close`, `Read`, `ReadAsync`, `Write`, `WriteAsync`, `Seek`, `GetSize`, `Tell`, static initializer |
| | `.ctors` | `0x80352A78–0x80352A7C` | initializer entry |
| | `.data` | `0x803C6988–0x803C69E4` | `NandFileStream` vtable, 0x5C bytes |
| | `.sbss` | `0x804B93D0–0x804B93D8` | `NandFileStream::typeInfo` and padding |
| `nw4r/ut/ut_LockedCache.cpp` | `.text` | `0x802942B0–0x80294488` | `LC::Enable`, `Disable`, `Lock`, `Unlock`, `LoadBlocks`, `StoreBlocks`, `StoreData`, static initializer |
| | `.ctors` | `0x80352A7C–0x80352A80` | initializer entry |
| | `.bss` | `0x804524A8–0x804524C4` | anonymous-namespace `sLCImpl`, 0x1C bytes |
| `nw4r/ut/ut_Font.cpp` | `.text` | `0x80294490–0x80294534` | `Font::InitReaderFunc` |
| | `.data` | `0x803C69E8–0x803C6A18` | four `CharStrmReader` member-function pointer constants |

The `.ctors` table entries at `0x80352A74`, `0x80352A78`, and `0x80352A7C` point to the static initializers at `0x80293DC0`, `0x802942A0`, and `0x80294470`. Every split ends at its last object and leaves alignment padding to the following unit.

All four units are `Matching`: objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## DvdFileStream is linked only for its type information

Only the static initializer of `ut_DvdFileStream.cpp` survives. It stores `&FileStream::typeInfo` into `DvdFileStream::typeInfo` at `0x804B93C8`. No `DvdFileStream` function or vtable is in the target, so the linker removed the class body. The source keeps only `NW4R_UT_RTTI_DEF_DERIVED(DvdFileStream, FileStream)`, because the stripped members cannot be checked against this binary. The imported `ut_DvdFileStream.h` layout is therefore unverified.

## Differences from the 2006 reference

Sources follow ogws `src/nw4r/ut` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0). `ut_LockedCache.cpp` and `ut_Font.cpp` match unchanged. `NandFileStream` follows a later revision:

- The only constructor takes no arguments, and the former `Initialize_` body is its body. The path and `NANDFileInfo` constructors and the `Open(const NANDFileInfo*, u32, bool)` overload are absent.
- `Read` advances the position only when `NANDRead` returns a positive count (the ogws `VERSION_RSPE01_01` form).
- `Write` returns `s32`. It calls `FilePosition::Append` only for a positive result, then returns the result.
- `GetSize` and `Tell` are out of line at `0x80294280` and `0x80294290`, after `Seek`. Their vtable slots (`+0x40`, `+0x58`) point there.
- The inline virtuals `GetRuntimeTypeInfo`, `IsBusy`, `CanAsync`, `CanSeek`, `CanRead`, `CanWrite`, `CanCancel`, `GetOffsetAlign`, `GetSizeAlign`, and `GetBufferAlign` were emitted once, at `0x8027E210–0x8027E2A8`, in an earlier unit. The vtable at `0x803C6988` identifies each slot; `CanRead`, `CanWrite`, and `IsBusy` read bytes 360, 361, and 362. These addresses are named in `symbols.txt` so the linker discards this unit's weak copies.

The relocation targets of `Open` and `Close` identify `fn_802E5CD0` as `NANDOpen` and `fn_802E5F70` as `NANDClose`. Their declarations were added to `include/revolution/nand.h`.

## Headers

`ut_NandFileStream.h`, `ut_DvdFileStream.h`, and `ut_LockedCache.h` come from the same ogws commit, stripped with `tools/vendor/vendor_strip.py`. `ut_NandFileStream.h` carries the revision above. The SDK includes were narrowed to `revolution/nand.h`, `revolution/dvd.h`, and the `OSCache`, `OSMutex`, and `OSThread` headers. `OS_CACHE_BASE` (`0xE0000000`) was added to `include/revolution/os/OSCache.h`.

## Traps

- **Inline virtuals can be emitted in another unit.** NandFileStream's ten inline virtuals are at `0x8027E210`, far before the unit, where an earlier unit first needed them. Identify them from the vtable slots and name them in `symbols.txt` before linking; unnamed, the unit's weak copies are kept and shift the DOL.
