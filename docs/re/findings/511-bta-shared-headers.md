# F-511-3: Shared BTA system and timer declarations

The earlier connection-unit header `sys.h` independently declared the timer queue, an incomplete timer entry, registration callbacks, and BTA control block. The later BTE headers declare the real timer entry in `gki.h` and public BTA interfaces in `bta_sys.h`. Including them together gives incompatible duplicate typedefs, despite the proven matching memory layout.

This prerequisite consolidates those definitions in the imported `include/revolution/bte/` hierarchy. `ptim.h` uses the existing canonical `TIMER_LIST_Q` from `gki.h`; `bta_sys.h` owns subsystem registration/configuration types and callback APIs; `bta_sys_int.h` owns `tBTA_SYS_CB`; `sys.h` forwards to those declarations for the existing connection source. No function body or storage definition changes.

The pinned reference is Petari `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/{bta_sys.h,bta_sys_int.h,ptim.h}`. The control block retains 26 registration pointers, timer state at `0x68`, flags at `0x7C`-`0x7E`, and callbacks at `0x80`, `0x84`, `0x88`, with total size `0x8C`. Registration event handlers use the existing eight-byte `BT_HDR` layout instead of the duplicate legacy `HC_BT_HDR` structure. Both headers describe the same four halfwords; the connection unit never calls that event handler. The actual connection callback types remain byte arguments and address pointers. `tPTIM_CB` remains `0x14` bytes; `TIMER_LIST_Q` remains `0x0C` bytes. These are declaration consolidations, not inferred layout changes.

The reference files carry Copyright (C) 2003-2012 Broadcom Corporation and changes by muff1n1634 under Apache-2.0. The preserved [BTE notice](../licenses/510-bte-NOTICE.md#issue-511-additions) and [full license](../licenses/510-apache-2.0.txt) apply. Adaptations remove comments, use project include paths, and retain the needed declarations.

Validation compiles every registered source through `ninja all_source`, links the normal source-selected executable, compares all sections of the existing matching `bta_sys_conn.c` object with default objdiff settings, and runs the frozen-manifest verifier. The pull request records command exit codes and final rebased commit. No new code coverage is claimed.

## Traps

The duplicated opaque `TIMER_LIST_ENT` in the legacy header named a different structure tag from `gki.h`'s real entry. Reuse the canonical GKI declaration before combining timer and BTA headers. Preserve the measured offsets and prove existing source object and full executable identity after consolidation.
