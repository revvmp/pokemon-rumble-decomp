# F-778-2: Complete G3D draw source

Issue #778 owns the complete original `g3d_draw.cpp` translation unit. Its thirteen retained functions occupy `0x80262880`â€“`0x80264720`, totaling 7,772 function bytes. The earlier survey blocks #405/#406 are superseded by this exact source-file inventory and receive no separate completion credit.

## Provenance and source

The implementation derives from CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. The root license and each reused file were inspected; no file-level exceptions were present. Comments were removed, C++ null pointers use `nullptr`, and includes use this project's canonical resource, math, state, light, transform, locked-cache and SDK headers. The original namespace structure, overloaded draw loops, material-replacement operations, depth comparators and MSL sorting templates remain idiomatic original source. No assembly or extra compiler flags are used: the existing Wii/1.1 `cflags_nw4r` profile reproduces the complete unit.

The shared canonical declarations are supplied by prerequisite PR #816. Main's existing workmem, light, GPU and resource headers are reused unchanged. [F-778-1](778-g3d-draw-headers.md) documents replacement fields, the twelve-byte depth record, draw-mode enum and header provenance.

## Unit boundaries and storage

| Section | Native range | Retained contents |
| --- | --- | --- |
| `.text` | `0x80262880`â€“`0x80264720` | All thirteen retained functions, including both sort instantiations and sort132 |
| `.sdata` | `0x804B8978`â€“`0x804B8980` | Two four-byte shuffle statics initialized to -4 |
| `.sdata2` | `0x804BAC98`â€“`0x804BACA4` | Float constants 0.0f, -2.0f and 1.0f |

All function names were already imported from original library evidence. The externally owned accessor at `0x80261D90` is named `GetMdlZTemporary` using the canonical workmem header, the native draw call and independently matching accessor source; its original split, size and type are unchanged.

## Verification

The complete scratch and actual compiled source each match all thirteen native instruction streams after applying all 103 independently decoded relocation targets. Both shuffle initializers and all three consumed constants match native bytes. The initial NonMatching split build passed Ninja and explicit frozen verification of its actual `build/778-source-eval/wpse-v1` output. The actual source-selected Ninja build exited zero, followed by explicit frozen verification of the same output directory. All three sections score 100% in objdiff. The final ELF places all thirteen functions and five owned data definitions at their native addresses and sizes. The linker input selects `src/nw4r/g3d/g3d_draw.o` and excludes the original fallback object. All 454 existing compiled source objects retain identical SHA-256 hashes. All six unused MdlZ sorting integral constants are absent from the final ELF. The actual DOL equals the original: 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The complete thirteen-row issue inventory is independently audited by address, name and size.

Commands: `python configure.py --build-dir build/778-source-eval`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/778-source-eval/wpse-v1` each exited zero. Independent native, complete-source and original-issue audits each exited zero. PR #816 must merge before this source PR is rebased for its final latest-main gate.

## Traps

- A tiny `.text` objdiff mismatch was caused by the fallback call retaining the name `fn_80261D90` while the source used canonical `GetMdlZTemporary`. Both relocations independently decoded to the same native address. Naming that outside callee without changing its split, size or type restores symbol pairing.
- MSL's sorting templates emit six unused integral constants after the three consumed float constants in the source object's `.sdata2`. These are compile-time template constants and are unreferenced at runtime. The retained native split ends after the third float at `0x804BACA4`; final linked acceptance must prove the unused constants are stripped rather than extending the split over neighboring data.
- Custom output builds require explicit frozen verification against their actual version directory, because the Ninja SHA-list CHECK still names the default build output; [F-774-1](774-g3d-state-headers.md) records the observed default-path failure.
