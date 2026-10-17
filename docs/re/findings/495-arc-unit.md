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
