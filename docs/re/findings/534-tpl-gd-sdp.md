# Issue 534: RVL_SDK TPL, GDBase, and BTE SDP

## F-534-1: TPL and GDBase unit boundaries and source

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). GNU objdump 2.42 disassembly of `build/wpse-v1/main.elf` and the dtk 1.8.4 symbol list show:

- `RVL_SDK/tpl/TPL.c` owns `.text` `0x8033E1B0`–`0x8033E3D0`: `TPLBind` (`0x118`), `TPLGet` (`0x20`) and `TPLGetGXTexObjFromPalette` (`0xE0`), which calls `GXInitTexObj` at `0x802CE820` and `GXInitTexObjLOD` at `0x802CEA80`. Its data are the panic message at `.data` `0x803E5CC0` (0x2B bytes, padded to `0x803E5CF0`), `__FILE__` `"TPL.c"` at `.sdata` `0x804B8F40`–`0x804B8F48`, and the unsigned-to-float conversion constant at `.sdata2` `0x804BB638`–`0x804BB640`.
- `RVL_SDK/gd/GDBase.c` owns `.text` `0x8033E3D0`–`0x8033E538`: `GDInitGDLObj`, `GDFlushCurrToMem`, `GDPadCurr32` and `GDOverflowed` (`0x8033E520`, listed in #535's block), and `.sbss` `0x804B9B98`–`0x804B9BA0` (`__GDCurrentDL`, then the static `overflowcb`). The file's first function is in #534's block, so #534 owns the whole unit, including `GDOverflowed`.

Function order and bodies follow Petari's CC0 `src/RVL_SDK/tpl/TPL.c` and `src/RVL_SDK/gd/GDBase.c` at `e0ab141`. Petari's TPL.c lacks `TPLGetGXTexObjFromPalette`; its body was written from the disassembly with the RVL SDK's public `GXInitTexObj`/`GXInitTexObjLOD` argument order. The names of the two GX callees are inferred from that argument use; confidence medium-high.

Compiled with Wii/1.0 and `cflags_sdk`, all seven functions and both units' data match in objdiff 3.8.2, and `tools/verify/verify.py verify` passes with both units `Matching`.

Trap: the first link with GDBase's `.text` ending at `0x8033E540` failed `build.sha1`. The compiled object ends at `0x8033E538`, and the following dtk auto-split object (`fn_8033E540`) has only 8-byte alignment, so it moved up to `0x8033E538` and shifted the rest of `.text`. Ending the split at the last instruction, `0x8033E538`, leaves the padding to the auto object and links identically. When the next unit is still auto-split, end a 16-byte-aligned unit at its real code end, not at the next function's start.

## F-534-2: BTE SDP is built with a GameCube compiler, not Wii/1.0

The first 34 functions of #534 (`0x8033AA00`–`0x8033E1A4`) are not 16-byte aligned, unlike the Wii/1.0 RVL_SDK code identified in F-0007. Their strings (`SDP - ...`) identify the Broadcom BTE Service Discovery Protocol.

Petari's `src/RVL_SDK/bte/sdp_discovery.c`, `sdp_main.c`, `sdp_server.c` and `sdp_utils.c` (commit `e0ab141`) were compiled outside the repository with Petari's `cflags_sdk` and compared with the DOL, masking every relocation-bearing word:

| Compiler | Result |
| --- | --- |
| GC/3.0a3, default alignment | 38 of 38 present functions match: all of `sdp_main.c` (11), `sdp_server.c` (4) and `sdp_utils.c` (13) and the 10 `sdp_discovery.c` functions from `0x8033A670` |
| GC/3.0a5.2, default alignment | All 13 `sdp_utils.c` functions match; other files not tried |
| Wii/1.0, default alignment | `sdpu_build_attrib_entry`, `sdpu_extract_uid_seq`, `sdpu_extract_attr_seq`, `sdpu_is_base_uuid` differ in size |
| Wii/1.0 `-func_align 4` | `sdpu_extract_uid_seq` (`0x378` vs `0x350`) and `sdpu_extract_attr_seq` differ |

Unit boundaries from the match:

| File | `.text` | Notes |
| --- | --- | --- |
| `sdp_discovery.c` | `0x8033A670`–`0x8033B8E8` | Starts in #533's block (`sdpu_build_uuid_seq`), so #533 owns the split |
| `sdp_main.c` | `0x8033B8E8`–`0x8033C454` | `sdp_set_max_attr_list_size` is absent (dead-stripped) |
| `sdp_server.c` | `0x8033C454`–`0x8033D164` | |
| `sdp_utils.c` | `0x8033D164`–`0x8033E1A4` | `sdpu_compare_bt_uuids` is absent; `0x8033E0C4` is `sdpu_compare_uuid_with_attr` |

Conclusion: the BTE SDP units need their own library entry with a GameCube compiler (GC/3.0a3, or a later 3.0a build), high confidence for the code; data layout is untested. F-510-1 found the small BTE transport and logging functions at 100% with Wii/1.0, so the library's compiler must be checked per unit; those functions may compile identically under both. These SDP units are not reconstructed in this change. Petari's BTE files are bluedroid derivatives under the Apache License 2.0, not CC0; reuse them only with notices kept under `docs/re/licenses/`, as F-510-1 does.

## F-534-3: BTE SDP main, server and utility units

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/sdp_main.c`, `sdp_server.c` and `sdp_utils.c`. This supersedes the "not reconstructed" note in F-534-2.

Data boundaries come from the references each function makes in the dtk split assembly:

| File | `.text` | Data |
| --- | --- | --- |
| `sdp_main.c` | `0x8033B8E8`-`0x8033C454` | `.data` `0x803E5828`-`0x803E5B88` (trace strings; `sdp_discovery.c` data ends at `0x803E5824` with its jump table); `.bss` `0x804AEA40`-`0x804B3074` (`sdp_cb`, 0x4634 bytes) |
| `sdp_server.c` | `0x8033C454`-`0x8033D164` | `.data` `0x803E5B88`-`0x803E5BC8` |
| `sdp_utils.c` | `0x8033D164`-`0x8033E1A4` | `.rodata` `0x80398298`-`0x803982A8` (`sdp_base_uuid`); `.data` `0x803E5BC8`-`0x803E5CC0` (four jump tables and two strings) |

Compiled with GC/3.0a3 (`BTE_COMPILER`) and the library `cflags_sdk`, all 28 functions and every data section score 100% in objdiff 3.8.2. With the three objects `Matching` and the 19 external callees named in `symbols.txt` (table in [symbols](../SYMBOLS.md#bte-sdp-main-server-and-utilities-534)), `ninja` exits 0 and `tools/verify/verify.py verify` passes.

Two source details decide the match:

- The SDP status codes must be preprocessor constants, as in bluedroid, not enumerators. With enumerators, `sdp_disconnect_ind`'s `con_state == SDP_STATE_CONNECTED ? SDP_SUCCESS : SDP_CONN_FAILED` compiles to a compare and branch; with `#define` constants GC/3.0a3 emits the target's branchless mask sequence. `include/revolution/bte/sdp_api.h` now defines them as macros.
- `sdpu_build_attrib_seq` must test `if (p_attr)` first; the reverse test or a conditional expression inverts the target's branch.

dtk inferred `sdp_cb` as 0x4638 bytes including alignment padding; the object is 0x4634 bytes. Ending the `.bss` split at `0x804B3074` and recording size 0x4634 gives 100% `.bss`, and the following `fragmentinfo` keeps its 8-byte-aligned address.

## Traps

- **Enum constants change conditional-expression code generation in BTE.** Trigger: a ternary between two SDP status enumerators scored 89% with a compare-and-branch. Bluedroid defines the status codes as macros; with `#define` constants GC/3.0a3 emits the target's branchless select. Evidence: F-534-3.
- **Declaring a function in a shared header can move its definition to another section.** Trigger: adding `strlen` to `include/string.h` moved the weak `strlen` in `Runtime/__mem.c`, which begins with `#pragma section code_type ".init"` and includes that header, from `.text` `0x8033F974` into `.init` at `0x800046E4`; `.init` grew 0x20 bytes and `build.sha1` failed although every unit scored 100%. CodeWarrior places a function in the section active at its first declaration. `sdp_utils.c` declares `strlen` locally instead. Evidence: F-534-3.
- **A corrupt `.ninja_deps` makes `build.ninja` regenerate forever.** Trigger: `ninja: warning: premature end of file; recovering`, then `manifest 'build.ninja' still dirty after 100 tries`. Deleting `.ninja_deps` and `.ninja_log` in the worktree fixed it; the build then verified. Evidence: F-534-3 session.
- **BTE SDP is not Wii/1.0.** Trigger: SDP functions at `0x8033A670`–`0x8033E1A4` start at packed 4-byte addresses. Petari's SDP sources match with GC/3.0a3 but four `sdp_utils.c` functions differ under Wii/1.0. Give SDP a GC library entry and carry the Apache-2.0 notices as F-510-1 does. Evidence: F-534-2.
- **An object that ends before the next function's aligned start moves an auto-split neighbour.** Trigger: GDBase `.text` ending at `0x8033E540` failed `build.sha1`. End the split at the last instruction, `0x8033E538`. Evidence: F-534-1.
