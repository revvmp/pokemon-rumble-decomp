# F-514-4: Complete device-manager API, state machines, and power management

Issue #514 owns 25 functions and 4,848 code bytes at `0x8031694C`-`0x80317C3C`. Its first six functions (1,076 bytes) are already source-linked in `bta_dm_act.c` through PR #679. This delivery completes the other nineteen functions and two trailing #515 power-management functions. The three newly selected source units contain 21 retained functions and 4,072 code bytes; no bytes from the earlier action-unit delivery are counted again.

The prerequisite canonical interfaces merged in PR #685; see [F-514-3](514-device-manager-headers.md). The complete implementations adapt Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/bta_dm_api.c`, `bta_dm_main.c`, and `bta_dm_pm.c`. Source-file order, instruction comparisons, static table contents, callback relocations, and diagnostic strings establish the names and extents. These files retain the Broadcom Copyright (C) 2003-2012 and muff1n1634 provenance in the [BTE notice](../licenses/514-bte-NOTICE.md), with the [full Apache-2.0 license](../licenses/510-apache-2.0.txt). Comments are documented here rather than copied into source; includes and logging use existing project interfaces.

## Compiler and source coverage

All three units use measured `GC/3.0a3`, `BTE_COMPILER`, and `cflags_sdk`. A direct Wii/1.0 probe of the completed API source with SDK flags and four-byte function alignment reproduces seven of its eleven retained functions; `BTA_DmSetDeviceName`, `BTA_DmSearch`, and `BTA_DmPinReply` differ, and `BTA_EnableBluetooth` has no same-size candidate. GC/3.0a3 reproduces all eleven and the complete relocated object. Main and power-management also compare at 100% with the selected compiler; this does not establish compiler uniqueness for those two units.

Five additional public API bodies from the reference remain ordinary source and are dead-stripped by the link: `BTA_DmDiscover`, `BTA_DmBond`, `BTA_DmAuthorizeReply`, `BTA_DmSignalStrength`, and `BTA_DmKeepACLLinks`. They add no retained target functions or reconstructed byte credit. Inline power helpers remain idiomatic C and preserve the observed code generation.

| Unit | Retained functions / code bytes | Exact section extents |
| --- | --- | --- |
| `bta_dm_api.c` | 11 / 1,132 | `.text` `0x80316D80`-`0x803171EC`; `.sdata2` `0x804BB600`-`0x804BB610` |
| `bta_dm_main.c` | 2 / 352 | `.text` `0x803171EC`-`0x8031734C`; `.rodata` `0x80397F90`-`0x803980B8`; `.bss` `0x804AA998`-`0x804AAB18` |
| `bta_dm_pm.c` | 8 / 2,588 | `.text` `0x8031734C`-`0x80317D68`; `.data` `0x803E0828`-`0x803E0874`; `.bss` `0x804AAB18`-`0x804AAB48` |

Main owns the 0x7C-byte search control block at `0x804AA998` and the 0x104-byte device-manager block at `0x804AAA14`. Power management owns the 0x2E-byte connected-service structure at `0x804AAB18`; the following two bytes are alignment. Its two diagnostics have exact lengths 0x29 and 0x1D at `0x803E0828` and `0x803E0854`; preserve their original `bta_dm_act` spelling. Both target and compiled power `.data` sections require eight-byte alignment. Source payload is 73 bytes while the split includes 76 bytes. The next referenced data is outside this unit at `0x803E0878`.

## Issue acceptance inventory

| Address | Size | Recovered name | Source |
| --- | --- | --- | --- |
| `0x8031694C` | `0x238` | `bta_dm_compress_cback` | `bta_dm_act.c` |
| `0x80316B84` | `0x108` | `bta_dm_rm_cback` | `bta_dm_act.c` |
| `0x80316C8C` | `0x84` | `bta_dm_keep_acl` | `bta_dm_act.c` |
| `0x80316D10` | `0x1C` | `bta_dm_immediate_disable` | `bta_dm_act.c` |
| `0x80316D2C` | `0x4` | `bta_dm_reset_complete` | `bta_dm_act.c` |
| `0x80316D30` | `0x50` | `bta_dm_send_hci_reset` | `bta_dm_act.c` |
| `0x80316D80` | `0x88` | `BTA_EnableBluetooth` | `bta_dm_api.c` |
| `0x80316E08` | `0x38` | `BTA_DisableBluetooth` | `bta_dm_api.c` |
| `0x80316E40` | `0x38` | `BTA_DmIsDeviceUp` | `bta_dm_api.c` |
| `0x80316E78` | `0x64` | `BTA_DmSetDeviceName` | `bta_dm_api.c` |
| `0x80316EDC` | `0x58` | `BTA_DmSetVisibility` | `bta_dm_api.c` |
| `0x80316F34` | `0x84` | `BTA_DmSearch` | `bta_dm_api.c` |
| `0x80316FB8` | `0x38` | `BTA_DmSearchCancel` | `bta_dm_api.c` |
| `0x80316FF0` | `0x8C` | `BTA_DmPinReply` | `bta_dm_api.c` |
| `0x8031707C` | `0xF0` | `BTA_DmAddDevice` | `bta_dm_api.c` |
| `0x8031716C` | `0x48` | `BTA_DmRemoveDevice` | `bta_dm_api.c` |
| `0x803171B4` | `0x38` | `BTA_DmSendHciReset` | `bta_dm_api.c` |
| `0x803171EC` | `0x98` | `bta_dm_sm_execute` | `bta_dm_main.c` |
| `0x80317284` | `0xC8` | `bta_dm_search_sm_execute` | `bta_dm_main.c` |
| `0x8031734C` | `0x68` | `bta_dm_init_pm` | `bta_dm_pm.c` |
| `0x803173B4` | `0x18` | `bta_dm_disable_pm` | `bta_dm_pm.c` |
| `0x803173CC` | `0x380` | `bta_dm_pm_cback` | `bta_dm_pm.c` |
| `0x8031774C` | `0x380` | `bta_dm_pm_set_mode` | `bta_dm_pm.c` |
| `0x80317ACC` | `0x78` | `bta_dm_pm_btm_cback` | `bta_dm_pm.c` |
| `0x80317B44` | `0xF8` | `bta_dm_pm_timer_cback` | `bta_dm_pm.c` |

The complete power unit also supplies `bta_dm_pm_btm_status` at `0x80317C3C` (0x120 bytes) and `bta_dm_pm_timer` at `0x80317D5C` (0xC bytes) for #515. This does not complete #515's remaining ACL functions.

## Acceptance evidence

The adjusted split-only baseline and then the full source-selected link each exit 0 and pass the frozen verifier. `python configure.py --no-progress --build-dir build/514-completion`, `.venv/Scripts/ninja all_source`, and `.venv/Scripts/ninja` exit 0. Default objdiff 3.8.2 compares every code/data section at 100% for all three units and the preceding complete `bta_dm_act.c`. An independent audit verifies every original issue address and size, 25 functions / 4,848 bytes. `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/514-completion/wpse-v1` exits 0 against frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope is the main game DOL only, excluding boot content 6, data contents, and WAD packaging. The pull request records the final rebased verification and exact head.

## Traps

Naming the next eight-byte-aligned power-management `.data` unit exposed a dtk gap symbol beginning at `0x803E0823` with five bytes. The prior action split ended at `0x803E0824`, so objdiff rejected the target symbol as out of section bounds (`320 > 316`). Extend that preceding split to the next natural boundary, `0x803E0828`. The action source payload remains 315 bytes and all string sizes stay unchanged. This supersedes only the earlier 0x803E0824 split-end guidance in [F-512-5](512-dm-completion.md); it does not alter that historical acceptance evidence. With the full gap in its original unit, all four whole-object comparisons and both full frozen builds pass. Do not insert synthetic padding arrays or resize strings to hide this condition.

A later whole-PR review found that an older refresh commit carried unrelated DVD reversions even after its rebase completed. The branch would have replaced the current main DVD unit and its named metadata with an earlier tail fragment, while original fallbacks could still reproduce the DOL. Restore the current main DVD sources, headers, split, symbols and evidence, then reapply only the owned BTA changes before fresh verification. Review the complete PR diff and union-merged documentation, not only the lines where Git reports a conflict; exact artifact identity alone cannot detect lost source coverage.
