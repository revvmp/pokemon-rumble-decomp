# MetroTRK dispatch and cache maintenance

## F-541-3: Native dispatch protocol and buffer layout

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. `TRKDispatchMessage` at `0x8034E1C8` has 288 function bytes and a 108-byte jump table at `0x803E6270`. The ordinary C switch adapts [Petari e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/debugger/embedded/MetroTRK/Portable/dispatch.c), CC0-1.0 with no file-specific exception found. This target omits the reference's Versions and SupportMask handlers. Its 27 jump-table entries establish the twelve retained command values and their dispatch targets; every target is paired with the reference's handler order and interface. Outside handler and buffer-position symbols are named without changing their extents, types or splits.

The dispatcher resets the buffer position, reads command byte `data[4]`, and returns `DS_DispatchError` for unsupported commands. The native load at buffer offset `0x10` initially appeared to require a different command index. Tracing `TRKGetBuffer`, buffer reset, allocation and position updates instead proves that this target omits Petari's unused leading mutex word. The complete target `TRKBuffer` occupies `0x88C` bytes: `isInUse` at zero, `length` at four, `position` at eight and `data[0x880]` at `0xC`. `TRKGetBuffer` multiplies its index by `0x88C`; reset passes buffer plus `0xC` and length `0x880` to `TRK_memset`; the position setter reads length at four and writes position at eight. The existing opaque declaration is completed with these proven fields.

## Cache-maintenance source

`TRK_flush_cache` at `0x8034DE04` occupies 56 bytes. Its original inline-assembly implementation adapts [Petari's flush_cache.c at the same pin](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c), CC0-1.0 with no file-specific exception found. All fourteen native instructions match, including the original alignment mask and eight-byte iteration. This is an original assembly routine rebuilt from assembly source, explicitly distinguished from C decompilation credit. It contains no executable-byte arrays or original-object dependency.

## Validation

Both units use the already measured Wii/1.1 MetroTRK flags, automatic inlining, four-byte function alignment and zero SDA limits. The baseline, split-only and source-selected standard configure/build workflows each exit zero. The explicit frozen verifier passes after a successful source-selected Ninja link. The 4,094,880-byte DOL remains byte-identical with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Every compared section scores 100%: dispatch text and data, and cache text. All 346 pre-existing source objects are unchanged, and both final function addresses and extents match. Together with F-541-1 and F-541-2, the branch supplies 23 of 32 issue functions and 2,516 of 2,880 function bytes, including the disclosed 56-byte original assembly routine. Nine functions and 364 bytes remain; this partial work does not close #541.

Commands: `python configure.py`, `.venv/Scripts/ninja`, `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit zero. A separate audit invokes the pinned objdiff executable on both complete objects, checks all 346 baseline object hashes and native final ELF symbols, and compares the complete DOL directly.

## Traps

- A reference structure can shift every field while a local expression still compiles to the expected offset. Verify the complete target layout through independent callees before changing a protocol index to compensate for a mismatched reference structure.
- Original assembly remains assembly. Rebuilding the original cache routine removes its fallback dependency but does not turn those 56 bytes into C decompilation progress.

## Subsequent completion

[F-541-7](541-dolphin-completion.md) supersedes the intermediate remaining-work statement above and records the complete source-selected issue, coordinated neighboring functions, and separate original-assembly accounting.
