# Prior art: Petari

[Petari](https://github.com/SMGCommunity/Petari) is the Super Mario Galaxy decompilation. It was reviewed at commit `e0ab141914921726a595573dc48d3f13d0922efe` (committed 2026-10-14) as a reference for Wii decompilation tooling and process. It is CC0-licensed. Nothing here is evidence about Pokémon Rumble; it is a catalogue of techniques to evaluate against our own findings. Adoption status is tracked in [D-0007](../DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) and the [roadmap](../ROADMAP.md).

## Ecosystem

Petari is built on [dtk-template](https://github.com/encounter/dtk-template), the scaffold used by nearly all active GameCube and Wii decompilations. Its moving parts:

| Tool | Role in Petari | Pinned tag in `configure.py` |
| --- | --- | --- |
| [decomp-toolkit](https://github.com/encounter/decomp-toolkit) (`dtk`) | Analyzes the DOL, splits it into relocatable objects from `symbols.txt`/`splits.txt`, generates the linker script, checks output hashes | `v1.8.3` |
| [objdiff](https://github.com/encounter/objdiff) | Per-function and per-object diffing of rebuilt versus split original objects; GUI auto-rebuilds on change; CLI produces progress reports | `v3.6.1` |
| Metrowerks CodeWarrior (`mwcceppc`, `mwldeppc`) | Original compilers and linker, downloaded as a versioned compiler bundle | bundle `20251118` |
| GNU binutils (`powerpc-eabi-as`) | Assembles any assembly files | `2.42-2` |
| [wibo](https://github.com/decompals/wibo) | Runs the 32-bit Windows compilers on Linux/macOS; not needed on Windows | `1.0.3` |
| sjiswrap | Converts UTF-8 sources to Shift JIS for the compiler | `v1.2.2` |
| ninja | Build executor; `python configure.py` generates `build.ninja` | unpinned |
| [decomp.me](https://decomp.me) | Shared scratches for collaborating on hard functions | not applicable |
| [decomp.dev](https://decomp.dev) | Hosted progress tracking and badges, fed by CI reports; reported by the project owner to be sunsetting, so this project renders its own images ([D-0008](../DECISIONS.md#d-0008-generate-progress-images-in-repository)) | not applicable |

`dtk` is the important one for us. As documented in its README, it reads WAD containers, U8 and RARC archives, and Yaz0, Yay0, and NLZSS compression through `dtk vfs`, and `dtk wad info|extract|verify` exist. The README lists WAD support under "Wii VC". Subsequent local validation confirms dtk 1.8.4 handles this WAD and U8 archives; LZH8/Huff8 need supporting decoders. See F-0005 and [extraction](EXTRACTION.md).

## Repository mechanics

- **Per-version config.** `config/<GAMEID>/config.yml` names the original object (`sys/main.dol`), its SHA-1, `symbols.txt`, `splits.txt`, and the `mw_comment_version`. `build.sha1` lists expected hashes of the built outputs. Four regions share one source tree; per-version code differences use a `VERSION` preprocessor define and per-object `MatchingFor(versions)` flags.
- **`symbols.txt`.** One line per symbol: `name = section:address; // type:function size:0x5C scope:global align:4`. Attributes include `data:` for disassembly formatting, `force_active` against dead-stripping, `noreloc` for pointer-like non-pointers, and `stripped`.
- **`splits.txt`.** Declares section layout, then each translation unit's address ranges per section: `.text start:0x800070A0 end:0x800071C8`. dtk derives link order from this. The `common` attribute marks common BSS, `rename:` handles `.ctors$10`-style sections, `order:` anchors ambiguous link order.
- **`configure.py` object list.** Every translation unit is an `Object(status, path, extra_cflags=..., mw_version=...)`. `Matching` objects are compiled and linked in place of the split original object. `NonMatching` objects are compiled for diffing only and the original split object is linked instead, so the full build always reproduces the original. `Equivalent` objects link only under `--non-matching`.
- **Library groups.** Objects are grouped into libraries (game, JSystem, RVL_SDK, NW4R, MSL_C, MetroTRK, RVLFaceLib), each with its own base flags and compiler version. Petari mixes compiler versions `GC/2.7`, `GC/3.0a3`, `GC/3.0a3.3`, `GC/3.0a5`, `GC/3.0a5.2`, and `Wii/1.0`, links with `GC/3.0a5`, and overrides flags per object (`-ipa file`, `-opt nolifetimes,nocse`, `-inline noauto`). Library code was rarely built with the game's compiler.
- **Shared library headers.** `libs/<lib>/include` holds headers for RVL_SDK, nw4r, JSystem, MSL, MetroTRK, Runtime, and RVLFaceLib, sourced from other decompilations (ogws, tp, doldecomp). These are the main cross-project accelerator.
- **Embedded asset extraction.** `extract:` in `config.yml` pulls tables embedded in the DOL into `.bcsv` or `.arc` files plus a generated header, so source includes data instead of hex blobs.
- **Analysis corrections.** `block_relocations` and `add_relocations` in `config.yml` fix places where automatic analysis misidentified or missed pointers.
- **Context generation.** `tools/decompctx.py` flattens a source file's includes into a single context for decomp.me. dtk emits `compile_commands.json` for clangd.
- **CI.** GitHub Actions builds every version in a private container holding the original DOL, restores git mtimes for cache correctness, uploads map files, and posts objdiff `report.json` to decomp.dev.

## Matching knowledge worth keeping

These are documented in Petari's `docs/` and apply to any CodeWarrior-built target. Treat each as a hypothesis for our game until our own toolchain evidence supports it.

- **`.comment` section.** `mwcc` objects carry a `.comment` section that tells `mwld` symbol alignment and force-active flags. Its presence enables dead-stripping and alignment adjustment. dtk generates it for split objects according to `mw_comment_version`, which must match the linker generation (known values 8, 10, 11/12, 14/15 for GameCube-era linkers). Assembled objects need `comment:0`.
- **Common BSS.** With `-common on`, uninitialized globals become common symbols placed at the end of `.bss`. They must be marked `common` in splits or link order cannot be resolved. Older linkers inflate the first common symbol in each translation unit to the size of that unit's whole common block; this can reveal translation-unit boundaries without a map.
- **C++ exception runtime split.** `__init_cpp_exceptions.cpp` needs a manual split whose shape depends on linker generation; GC 2.7+ and Wii linkers use `.ctors$10`, `.dtors$10`, and `.dtors$15`.
- **Data pooling.** Symbols like `...data.0` indicate pooled data whose section is force-active.
- **Shared `.sdata2` constant order.** Petari uses a prefix header (`-prefix include/Game/SData2Order.hpp`) to force the original order of float constants shared across game translation units.
- **Exact types matter.** C++ mangled names encode `int` versus `s32`, so original signatures must be reproduced exactly. Virtual functions must follow original vtable order. Known symbol names keep original spelling, typos included.

## Adapting to this project's rules

| Petari practice | Our treatment |
| --- | --- |
| `build.sha1` SHA-1 check by dtk | Useful build-time sanity check only. Acceptance remains `tools/verify/verify.py` with frozen SHA-256 manifests and direct byte comparison. |
| `NON_MATCHING` and decomp.me links in source comments | Not allowed. Record status, scratch link, and suspected cause per function in the [reconstruction ledger](../PROGRESS.md#reconstruction-ledger). |
| Doxygen `/* 0x10 */` field-offset comments | Not allowed. Record layouts in [symbols](SYMBOLS.md). |
| dtk-template `tools/*.py` and `configure.py` contain comments | Conflicts with the no-comments rule. Vendoring requires a recorded decision: strip comments, or a narrow exception for unmodified third-party files. Open item in [D-0007](../DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem). |
| Percentage progress on decomp.dev | Replaced by `tools/progress/progress.py` images. Percentages permitted only once the denominator is established, per [matching](../MATCHING.md). objdiff's report provides a code-byte denominator once splits exist. |
| `NonMatching` objects fall back to split original objects | Matches our model exactly: the build reproduces bytes, while the fallback list tracks what is not yet source. Fallback objects never count as decompiled. |
| AI-generated decompilation banned | Petari's community policy, not ours. Reusing Petari or ogws headers is license-compatible (CC0 for Petari); check each source project's license before reuse and record provenance. |
| Private CI container holding originals | Possible later. Current explicit user authorization permits tracking `orig/`; follow [project rules](../PROJECT_RULES.md) for this exception. |

## Techniques for a target without a symbol map

Petari had version-specific symbol information; WiiWare titles typically ship without maps. Whether ours does is unknown until extraction. Candidate techniques, in rough order of value:

1. **dtk's automatic analysis.** `dtk dol config` produces initial `symbols.txt` and `splits.txt` from control flow and relocation analysis, and identifies standard runtime entry points.
2. **Library version strings.** Nintendo SDK libraries embed banners such as `<< RVL_SDK - OS release build: ... >>`, and NW4R embeds similar build strings. These pin which SDK build was linked and select candidate library source and compiler versions.
3. **Library matching from other decompilations.** Compile known RVL_SDK, NW4R, MSL, and runtime source from existing projects with candidate compiler versions and compare against the DOL. Matching library code supplies hundreds of names, translation-unit boundaries, and the compiler version per library at once.
4. **Assert and debug strings.** `__FILE__` paths and assertion messages name source files and help place translation-unit boundaries.
5. **Common BSS inflation and alignment padding.** Padding between functions and the inflation bug expose translation-unit boundaries.
6. **Related titles.** Other builds of the same engine or developer, if any are found with symbols, can transfer names. None is known; do not assume one exists.
