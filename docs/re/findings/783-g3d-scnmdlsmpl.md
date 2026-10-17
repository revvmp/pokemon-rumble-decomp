# F-783-1: Complete G3D simple model scene object

Issue #783 owns the complete original `g3d_scnmdlsmpl.cpp`: 23 retained functions / 8,148 function bytes at `0x8026AFA0`–`0x8026CFEC`. Earlier survey blocks #412/#413 receive no separate completion credit. The placeholder at `0x8026CB10` is `ScnMdlSimple::EnableScnMdlCallbackTiming(Timing)`, proven by the original member signature and exact native stream; the other imported method names are confirmed.

## Provenance and interfaces

The full implementation derives from CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`; root and file-level licenses were inspected. Comments are removed and null pointers use `nullptr`. Complete original C++ class hierarchy, virtual order, callbacks, animation attachment, bytecode and rendering methods remain intact. Existing Wii/1.1 `cflags_nw4r` reproduces the unit without added flags or assembly.

The complete ScnMdlSimple declaration is reused unchanged with owner approval from shared header PR #845. Canonical CalcWorld and CalcMaterialDirectly declarations are temporarily tracked from approved prerequisites; merged main declarations must replace duplicates before final integration. These are header-only prerequisites and take no neighboring source ownership.

ScnMdlSimple is `0x138` bytes, extending ScnLeaf (`0xE8`). Members are ResMdl at `0xE8`; world matrices/attributes at `0xEC`/`0xF0`; view position/normal/texture matrix pointers at `0xF4`/`0xF8`/`0xFC`; view-count/current-view bytes at `0x100`/`0x101`; matrix count u16 at `0x102`; flags at `0x104`; calc/mix/opaque/translucent bytecode pointers at `0x108`–`0x114`; draw mode at `0x118`; callback pointer at `0x11C`; timing/delete-option bytes at `0x120`/`0x121`; callback node u16 at `0x122`; Chr/Vis/MatClr/TexPat/TexSrt animation pointers at `0x124`–`0x134`. All consumed offsets are confirmed by the complete matching member streams and vtable.

Seven outside symbols receive name-only changes from the original interfaces and native relocations: CalcMaterialDirectly (`0x80270750`), CalcWorld (`0x80261DC0`), GetValueForSortOpa/Xlu (`0x80268250`/`0x80268260`), SetBoundingVolume (`0x80268300`) and the ScnObj/ScnLeaf type-name objects (`0x8036C960`/`0x8036C970`). Their splits, sizes and types remain unchanged.

## Unit storage

| Section | Native range | Contents |
| --- | --- | --- |
| `.text` | `0x8026AFA0`–`0x8026CFEC` | All 23 retained functions |
| `.rodata` | `0x8036C9A0`–`0x8036C9B4` | Twenty-byte type-name object |
| `.data` | `0x803C6008`–`0x803C6060` | Nine-byte NodeTree string, alignment gap and 72-byte vtable at `0x803C6018` |
| `.sdata` | `0x804B8990`–`0x804B89A8` | Eight-byte NodeMix, DrawOpa and DrawXlu strings |
| `.sdata2` | `0x804BAD38`–`0x804BAD3C` | Float 1.0f |

## Verification

The initial baseline and NonMatching split builds exited zero and explicitly verified their actual DOL outputs. The actual source-selected `build/783-source-eval/wpse-v1` build passed configure, plain Ninja and explicit frozen verification. All five allocated sections score 100%. All 23 native instruction streams / 8,148 bytes and 112 independently paired relocations (96 text, 16 vtable) match. Final ELF addresses and extents match for all 23 functions and all seven named allocations. The linker selects the actual source object and excludes the original fallback. All 463 baseline compiled source objects remain byte-identical. The complete original 23-row issue inventory is independently audited by address and size.

The 4,094,880-byte DOL equals the frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Verification scope is the main executable, excluding boot/data/package contents. Commands: `python configure.py --build-dir build/783-source-eval`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/783-source-eval/wpse-v1`. Each exited zero. Final acceptance requires deduplicating merged prerequisites and repeating the exact-head latest-main clean gate.

## Traps

- Keep main's complete canonical ScnObj hierarchy on shared-header add/add resolution. Independently compiling this whole unit against that header preserves all 23 native streams and 112 relocations, without changing the approved Simple layout.
- Construct reads `mdl.ref().info.need_tex_mtx_array` and also uses GetResMdlInfo. This is a typed public resource-field expression from the pinned reference. Four natural alternatives (both accessor calls, both direct accesses, and mutable/const local resource-info variables) changed ten or eleven constructor words; the retained mixed typed form matches the complete native constructor. The original author's exact expression is not independently established. No volatile storage, raw offsets, assembly or compiler forcing is introduced.
- A tiny IsDerivedFrom difference in objdiff came from the two outside type-name objects retaining anonymous names. Their independently decoded native relocations prove identity; naming those objects restores 100% without changing bytes or ownership.
- Explicit frozen verification must use the actual custom version directory; the default-path Ninja CHECK can inspect a previous default output. Successful plain Ninja plus actual-output comparison is required.
