# F-777-1: Texture dispatch, work memory and world matrices

Issue #777 owns nine functions and 3,020 native code bytes at `0x80261C80`–`0x8026287C`. Three complete retained translation units reproduce them under Wii/1.1 with the existing `cflags_nw4r` policy. No neighboring source unit is selected or split here.

| Unit | Native text | Other sections |
| --- | --- | --- |
| `g3d_dcc.cpp` | `0x80261C80`–`0x80261D64`, two functions / 220 bytes | None |
| `g3d_workmem.cpp` | `0x80261D70`–`0x80261DBC`, five functions / 64 bytes | `.bss` `0x80434F80`–`0x8044CF80` |
| `g3d_calcworld.cpp` | `0x80261DC0`–`0x8026287C`, two functions / 2,736 bytes | `.data` `0x803C5EC0`–`0x803C5EC9`; `.sdata` `0x804B8970`–`0x804B8978`; `.sdata2` `0x804BAC90`–`0x804BAC98` |

## Source and interfaces

The CC0 reference is `zeldaret/ss` revision `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, the three corresponding source files and their headers. Neither the sources nor headers have a separate file-level license notice. Specific existing resource and math headers replace aggregate includes, null pointers use `nullptr`, and code comments are removed. Original names and source order are preserved, including the retained skinning assembly. No compiler override, executable-byte array or original object supplies these units.

The texture dispatchers select Maya, XSI, 3ds Max or the basic transform, and set identity when the selected transform reports no work. The complete canonical world-attribute and work-memory declarations supplied by merged #830 are reused. This delivery adds only the still-needed native callee prototypes to `g3d_dcc.h`; it changes no class storage, attribute mask or existing helper behavior.

The world-calculation interface has the native eighth root-attribute argument in `r10`. It decodes node-tree commands, evaluates character animation or the resource default, applies translation patches and callbacks, dispatches the appropriate scaling rule, copies duplicate matrices and applies the final scale. The seven-argument reference wrapper and unretained callback defaults remain available in source without receiving native-byte credit. `FuncObjCalcWorld` has a callback pointer at zero, timing byte at four and node ID at six, size eight. Its timing names retain the reference's three proven bits in its own interface, avoiding an unrelated scene-object class declaration. `WorldMtxManip` has three pointers at zero, four and eight, size twelve. The complete character-animation declaration is reused from merged #399 / #761. No temporary header copy remains in this delivery.

Skinning decodes weighted and envelope-matrix commands, blends matrices, and updates the native scale attributes for the selected envelope policy. Its original four groups of paired-single operations total 24 instructions / 96 bytes. The nine functions therefore comprise 2,924 C++ bytes and 96 original inline-assembly bytes; the latter receive no C/C++ reconstruction credit.

## Work bank and names

The work bank is one 128-byte-aligned `0x18000`-byte union. Scale vectors begin at zero and matrix IDs at `0x6000`; depth-sort, skinning, billboard and shape-animation views reuse the same storage. Native accessors load `0x80434F80`, with the matrix-ID accessor adding `0x6000`. Their retained ordering and the native skinning call identify the three previously unnamed accessor entries. The unused shape-result accessor is discarded by the linker.

The original analysis divided the bank at the unaligned `0x80438046`. Scanning the symbol tables of every original split object found that interior label only as a definition in this work-memory object, with no external reference. The actual compiler emits one `0x18000`-byte object at alignment 128, so the owned metadata consolidates those two estimated records into its original `sTemp` symbol. The bank's total extent and every following address remain unchanged.

The two basic-transform callees at `0x80265620` and `0x80265780` receive names only. Their pinned reference bodies independently reproduce the native sizes and every non-relocation instruction, and the owned caller's paired relocations resolve to those addresses. Their sizes, types, splits and ownership remain intact; no reconstruction credit is claimed for them.

## Validation

The original-object split-only configure, Ninja and unchanged frozen-manifest verifier each exit zero before source registration. The units were compiled as `NonMatching` and independently audited. Every allocated object section scores 100% in objdiff. A separate audit compares every instruction byte and resolves all 44 relocations to their native targets, checks all 25 initialized data bytes, and verifies the bank extent and alignment. It modifies no compiler output and supplies no linker input.

After source selection, `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit zero. The actual linker selects all three reconstructed objects, and all nine final ELF functions retain their native addresses and extents. All 439 baseline source objects preserve their code, data and relocation targets. The full DOL is byte-identical to the frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

After rebasing onto main `30f0e3f3`, a fresh build in `build/777-merge6` passes configure, Ninja, the actual-directory frozen verifier, native instruction and relocation audits, and final source-selection checks. All 438 other baseline source objects remain unchanged. The shared GX header added by #833 makes the GC compiler emit fourteen additional unreferenced local helpers in NCD, totaling 196 bytes. Every prior NCD function body and relocation remains identical, as do all its data sections. Each added helper has local binding, has no object relocation referring to it, and exactly duplicates an existing FIFO writer of the same argument types. The original 920 text bytes and the added 196 bytes account for the complete new 1,116-byte text section. No source fallback or frozen hash changes are involved; the final DOL remains byte-identical.

## Traps

- Use the actual initialized-object end for data splits. The compiler emits a nine-byte string ending at `0x803C5EC9`, with three following linker-alignment bytes. An older split layout rejected that boundary, so the initial delivery used `0x803C5ECC`. After later neighboring splits, dtk generated a seven-byte gap symbol starting at `0x803C5EC9` inside that twelve-byte section and objdiff rejected its out-of-bounds extent. The actual nine-byte boundary is accepted in the current layout and avoids the generated overlap. No padding declarations or executable-byte copies are added. The initialized source data totals 25 bytes (9 + 8 + 8); the frozen full-DOL comparison covers alignment bytes separately.
- An unaligned interior BSS label can divide one original union. Establish the full extent from the compiled declaration and inspect original references before consolidating records; the exact source-selected link remains the acceptance gate.
- Name-only callee corrections matter to objdiff too. Native instruction and relocation audits passed while the basic world-transform target still had a placeholder name in the original object. Regenerating the split with the evidenced name restores the full 100% comparison.
