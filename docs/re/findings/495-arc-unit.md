# Issue 495: RVL_SDK arc unit and block boundaries

## F-495-1: `arc.c` occupies 0x802EFCD0–0x802F08D8 and matches Petari's source

Target `wpse-v1`, manifest `config/targets/wpse-v1.json`, artifact `orig/wpse-v1/main.dol` (SHA-256 `ad1a5989…53da`).

Observation. The 13 functions from 0x802EFCD0 to 0x802F08D8 reference the `.data` strings `"ARCInitHandle: bad archive format"` (0x803DCA78) and `"Warning: ARCOpen(): file '%s' was not found under %s in the archive.\n"` (0x803DCA9C), the `.sdata` string `"arc.c"` (0x804B8D78), and MSL's `_current_locale` (0x803E6228, 0x48 bytes). `ESP_InitLib` starts the next file at 0x802F08E0, and ESP's own string `@1687` starts at 0x803DCAE8. Every function except `entryToPath` is referenced from outside the unit, so all twelve are global; `entryToPath` is called only from inside the unit.

Procedure. `powerpc-eabi-objdump -dr` (GNU binutils 2.42) on the dtk 1.8.4 split object `build/wpse-v1/obj/auto_03_802ABD4C_text.o`, and a byte search for each `fn_` name across all split objects to find external references. Source: Petari `src/RVL_SDK/arc/arc.c` (CC0), already identified for this region by F-0007's masked search.

Interpretation and names, high confidence:

| Address | Size | Name | Scope |
| --- | --- | --- | --- |
| 0x802EFCD0 | 0xA0 | `ARCInitHandle` | global |
| 0x802EFD70 | 0x2AC | `ARCOpen` | global |
| 0x802F0020 | 0x50 | `ARCFastOpen` | global |
| 0x802F0070 | 0x27C | `ARCConvertPathToEntrynum` | global |
| 0x802F02F0 | 0x1D0 | `entryToPath` | local |
| 0x802F04C0 | 0x230 | `ARCGetCurrentDir` | global |
| 0x802F06F0 | 0x14 | `ARCGetStartAddrInMem` | global |
| 0x802F0710 | 0x8 | `ARCGetLength` | global |
| 0x802F0720 | 0x8 | `ARCClose` | global |
| 0x802F0730 | 0x58 | `ARCChangeDir` | global |
| 0x802F0790 | 0x7C | `ARCOpenDir` | global |
| 0x802F0810 | 0xBC | `ARCReadDir` | global |
| 0x802F08D0 | 0x8 | `ARCCloseDir` | global |

Differences from Petari's copy: this game keeps `ARCGetCurrentDir` as a global, out-of-line function, with `ARCConvertEntrynumToPath` and part of `entryToPath` inlined into it, and keeps `ARCClose`. It has no `ARCGetStartOffset`. `ARCOpen` needs no `force_active`, because the game calls it.

Result. With Wii/1.0 and the RVL_SDK library's `cflags_sdk` in `configure.py` (`-inline auto,level=3 -ipa file -sdata 8 -sdata2 8`), objdiff-cli 3.8.2 reports all 13 functions and 120 data bytes at 100%, and the linked `main.dol` passes `tools/verify/verify.py verify`.

Boundary detail. The unit's `.text` split must end at 0x802F08D8, after `ARCCloseDir`, not at the 16-byte boundary 0x802F08E0. With the end at 0x802F08E0 the compiled object was 8 bytes shorter than its split, the linker packed the following auto-split object against it, and `.ctors` moved from 0x803529C0 to 0x803529B8. Leaving the 8 zero bytes of padding to the next object reproduced the original.

## F-495-2: The first 16 functions of the block are not ARC

Observation. The functions from 0x802EDB30 to 0x802EFCD0 (16 functions, `fn_802EDB30` through `fn_802EF8F0`) do not reference ARC data. They index a table of 24-byte records at 0x8037D690 by a selector in the range 1–23, return small negative status codes (−3, −5, −6), call helpers at 0x802EA030–0x802EB540 inside issue #493's block, and are reached through the 0xC-byte trampolines at 0x802ED5C0–0x802ED6F0 inside issue #494's block, which load a table address and tail-call 0x802ED120 or 0x802ED320.

Interpretation, medium confidence. This is one library that spans issues #493, #494 and #495. F-493-1 later named the helpers at 0x802EA030–0x802EA170 `ENCi*` from wii-ipl's `encutility.c`, which supports RVL_SDK ENC. F-494-1 then split `RVL_SDK/enc/enclatin.c` as ending at 0x802ED700, so the file holding these 16 functions starts between 0x802ED700 and 0x802EDB30, inside #494's block. Under the shared-file rule in [collaboration](../../COLLABORATION.md), a file whose first function lies in an earlier block belongs to that block's issue. Both neighboring issues were unclaimed when this work started and were claimed while it was under review, so these 16 functions remain unsplit and #495 stays open for them.

Next validation step. Agree on the ENC file boundaries with the owners of #493 and #494, then split and reconstruct the remaining functions from the owning issue.

## F-495-3: Chinese and Korean ENC units complete the block

The five context-conversion functions at the start of #495 were reconstructed and linked by #494 in PR #583. This change reconstructs the remaining eleven ENC functions in two distinct files; the thirteen ARC functions remain Matching from F-495-1. This supersedes the unsplit and ownership uncertainty in F-495-2. Every function listed in #495 is now reconstructed and linked.

Target is `wpse-v1`, frozen `config/targets/wpse-v1.json`, original `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Source provenance: [wii-ipl ENC source](https://github.com/koopthekoopa/wii-ipl/tree/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/enc), pinned revision `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, root CC0-1.0 LICENSE, no additional file-level notice on these files. Comments, unused public wrappers and duplicate weak loaded-flag definitions were omitted. The retained flags belong to `encconvert.c`. Tables remain editable C initializers, with no executable-byte payload.

Both units compile with the existing Wii/1.0 `cflags_sdk`, including file IPA and default 16-byte alignment. `encchinese.c` owns `.text` `0x802EE650–0x802EEEBC` and `.rodata` `0x8037E920–0x80388B68`. `enckorean.c` owns `.text` `0x802EEEC0–0x802EFCD0` and `.rodata` `0x80388B68–0x80397DC0`. The four bytes following the Chinese code belong to the alignment before the next unit.

Evidence: GNU binutils 2.42-2 symbols and relocations, independent byte searches for every source table in the frozen DOL, dtk 1.8.4 splits, objdiff-cli 3.8.2 whole-object comparisons, and full executable verification. Each of the twelve source tables occurs exactly once in the original. `noreloc` marks the table values as integer encoding data, preventing automatic pointer interpretations. No external unit boundaries or expected hashes change.

### Validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0 for the baseline, split-only build and source-linked build.

| Unit | Matching functions | Matching code bytes | Matching data bytes |
| --- | --- | --- | --- |
| `encchinese.c` | 5/5 | 2136/2136 | 41544/41544 |
| `enckorean.c` | 6/6 | 3556/3556 | 62040/62040 |

Every code and data section compares at 100%; the frozen SHA-256 and direct-byte comparison pass. Neighboring #493 still owns the utility and Unicode units.

### Traps

No additional trap emerged beyond F-494-3: preserve explicit Unicode literals, real table sizes, integer-data relocation annotations, and true code ends before alignment padding.
