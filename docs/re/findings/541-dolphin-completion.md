# F-541-7: Complete MetroTRK Dolphin startup and issue 541

## Scope and provenance

The complete `Os/dolphin/dolphin_trk.c` unit starts with #541's `InitMetroTRK` and includes six functions supplied in coordination with #542. Its seven retained functions occupy `0x8034E2E8`–`0x8034E60C`, 804 bytes. The fifteen-entry ISR offset table occupies `0x803E62E0`–`0x803E631C`, 60 bytes; `lc_base` occupies `0x804B9C08`–`0x804B9C0C`, four bytes. Padding to the next unit is excluded from both data splits.

The adapted reference is [Petari dolphin_trk.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c), CC0-1.0, with no file-specific license exception found. Original register-save assembly in `InitMetroTRK` and `InitMetroTRK_BBA` is retained as assembly source, totaling 300 bytes. It contains instructions and symbol references, not embedded opcode arrays. The other five retained functions are ordinary C, totaling 504 bytes. The inline vector-copy helper has no retained standalone function in the final executable.

`InitMetroTRK` is 152 bytes, including the original trailing `blr` at `0x8034E37C`. The survey's 148-byte extent omitted that instruction. Retaining it corrects the #541 function-byte denominator by four without moving any function start or borrowing the next function's instructions. The adjacent BBA initializer is 148 bytes and retains its trailing return as well.

## Native behavior and compiler evidence

Translation preserves the locked-cache mapping when the saved DBAT3U enables it. Ordinary physical addresses below `0x03000000` map through `0x80000000`; addresses in `[0x10000000, 0x1C000000)` map through `0x90000000`; other addresses remain unchanged. The two bounds of the second memory window are pointer comparisons in the reconstructed C. With the existing Wii/1.1 MetroTRK flags, these comparisons reproduce both native bounds checks and the inlined vector-copy sequence.

Integer bounds from the comparable source cause Wii compilers to combine the range checks and shorten both routines. Several GC compiler versions reproduce the bounds but reorder two instructions in `TRKInitializeTarget`. Pointer bounds remove that discrepancy under the already evidenced Wii/1.1 compiler, without a compiler override, volatile state, mixed source units, or assembly substitutions for these C routines. Local declaration ordering, optimization, scheduling, processor selection, and integer-type variants did not establish a compiler-only correction.

The target interrupt-enable wrapper branches to `EnableEXI2Interrupts`; it is not an empty return. The ISR table has fifteen entries and includes the original `PPC_FloatingPointUnavaiable` spelling. Complete processor-state declarations are coordinated with the #545 owner and documented in [F-541-6](541-processor-state-layout.md). Their native sizes are `ProcessorState_PPC` 0x430 and `TRKState` 0xA4; this unit declares their external storage and does not take ownership of the #545 storage definitions.

## Validation

Both split-only and source-selected builds exit zero, followed by explicit frozen verification of the actual output directory:

```text
python configure.py --build-dir build/541-final-startup
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/541-final-startup/wpse-v1
```

All three commands exit zero. The final source object scores 100% in text, data, and SBSS. A separate paired native audit verifies all seven function bodies, all 39 relocation targets, the sixty ISR table bytes, and the final addresses and extents. The actual Ninja link rule selects the source object rather than its original fallback.

A consolidated audit covers all thirteen new or extended source units: every compared section scores 100%, all 38 newly supplied functions retain their native addresses and sizes, and the existing `stricmp` remains unchanged. The actual link rule selects all thirteen source objects and excludes their corresponding original objects. At the prerequisite baseline `cf3bfc9ee2b6569908aeecb392e18aad321c6e33`, all 357 unrelated source objects remained byte-identical. After refreshing onto main `0e5f17d0f7b8875e0381f6130204d5c20db53c4a`, a new build in `build/541-final-main` passes the same configure, Ninja, explicit actual-directory verifier and consolidated audit. Its 372 unrelated objects match the 373-object current-main baseline, excluding the intentional extras extension. Old object hashes are not used to reject legitimate intervening main changes. The final 4,094,880-byte executable matches the frozen reference with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Expected hashes and original inputs are unchanged.

## Completion accounting

This completes all 32 #541 functions: 2,644 C/C++ bytes plus 240 original SDK assembly bytes, totaling 2,884 bytes after the evidenced four-byte extent correction. The six #542 functions supplied through the shared startup unit contribute another 504 C bytes and 148 original assembly bytes. Total new source coverage is 38 functions and 3,536 bytes, comprising 3,148 C/C++ bytes and 388 original assembly bytes. Assembly bytes are excluded from C/C++ decompilation credit. The already accepted 160-byte `stricmp` is preserved and receives no duplicate credit. #542 owns the inventory and completion of its remaining functions.

The historical partial-work reports in F-541-1, F-541-2, F-541-3, and F-541-5 describe intermediate verified heads. This finding supersedes their remaining-work statements.

The subsequent D-0014 map-name import is preserved throughout the refreshed tree. Three spelling exceptions retain the canonical interfaces from the complete matching MetroTRK reference and coordinated headers: `TRKSetBufferPosition` at `0x8034F99C`, `TRKDoConnect` at `0x80350080`, and the startup unit's `__TRK_reset` at `0x8034E5FC`, in place of map-inferred `TRK_SetBufferPosition`, `TRK_DoConnect`, and `__TRKreset`. The #544 owner approved retaining the two shared callee names. This is the matched-source exception permitted by D-0014; unrelated imported names, scopes, sizes, and types remain intact.

The final integration incorporates merged header prerequisite #739 at `045d6aa985d0771e4f84ff91d8fb49de7ea1368b`, including complete canonical CPU types and packet interfaces. Repeated shared symbol paragraphs from earlier prerequisite commits are removed; main's complete versions of the shared headers and findings are retained. All main source files and configured objects, including the intervening allocation reconstruction, remain present. The fresh final build uses `build/541-merge739` and explicit verification of `build/541-merge739/wpse-v1`.

## Traps

- A comparable compiler can match two functions while changing another in the same unit. Require a whole-unit instruction, relocation, data, and final-link proof before changing its compiler.
- A survey boundary can omit an original assembly return. Compare complete reference source and the following native function start before trimming an apparently unreachable instruction.
- An out-of-line copy of an inline helper can alter source-object offsets while being stripped from the final link. Check native final addresses and actual selected sections rather than assuming every object offset equals its final native offset.
- A custom build directory's Ninja checksum rule may still name the default output. Explicitly pass the actual version directory to frozen verification after Ninja exits zero.
