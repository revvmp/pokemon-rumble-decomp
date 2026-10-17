# F-512-5: Complete device-manager action source acceptance

## Scope and acceptance

The complete `bta_dm_act.c` supplies 56 retained functions and 9,020 code bytes. The existing `utl.c` supplies one function and 68 bytes. Both units are now source-selected `Matching`, adding 9,088 linked code bytes. The complete action source replaces the earlier partial implementation, including its existing matching #512 functions and inlined helpers; these are not counted again as newly written code. The prerequisite declarations merged first in [PR #674](https://github.com/revvmp/pokemon-rumble-decomp/pull/674), documented in [F-512-4](512-dm-shared-headers.md).

Every function in #512 is now source-linked: three timer operations from #511's complete `ptim.c`, `utl_freebuf`, and the first 21 action-unit functions. Their compiled symbol sizes reproduce all 25 original issue extents, totaling 4,256 code bytes. The action file starts in #512, so this completion also covers its continuation through #513 and the first six functions of #514. Those neighboring issue inventories remain separately tracked; this acceptance does not complete #514's other files.

GC/3.0a3 and the existing `cflags_sdk` reproduce every retained instruction and all data. The compiler choice remains the measured F-512-2 result; no toolchain or global flag changes are made. The existing periodic-timer unit retains its #511 Wii/1.0 configuration. The pinned reference is Petari `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/bta_dm_act.c`, rewritten with project include paths, explicit trace calls, canonical callback types, and names for six unused integer callback arguments. No executable byte payload or decompiler output is used.

## Exact unit ranges and data

| Section | Split extent, exclusive end | Source payload and alignment |
| --- | --- | --- |
| `.text` | `0x80314A44`-`0x80316D80` | 56 functions; 9,020 bytes; original order and every size retained |
| `.rodata` | `0x80397EE8`-`0x80397F90` | 23 halfword UUIDs (46 bytes), 23 word service identifiers (92 bytes), six callback pointers (24 bytes); two and four alignment bytes between tables |
| `.bss` | `0x804AA968`-`0x804AA998` | Five nine-byte compression-service records, 45 bytes; three alignment bytes follow the array |
| `.data` | `0x803E06E8`-`0x803E0824` | Seven diagnostic strings and their original internal padding; payload ends at `0x803E0823`, followed by one alignment byte in this split |
| `.sdata2` | `0x804BB5F8`-`0x804BB5FC` | Four-byte local attribute-list initializer; the following four alignment bytes remain outside this split |

dtk rejects an automatic `.data` object beginning at the raw string end `0x803E0823` with an invalid-alignment error. The split ends at the next four-byte boundary, `0x803E0824`; the following original padding preserves the next referenced data at `0x803E0828`. The compression array's symbol remains exactly 0x2D bytes rather than claiming its three padding bytes as fields. No source array is enlarged to absorb padding. `bta_service_id_to_uuid_lkup_tbl` and `bta_service_id_to_btm_srv_id_lkup_tbl` similarly retain actual symbol sizes 0x2E and 0x5C, instead of their previously recorded padded spans 0x30 and 0x60.

The shared declaration additions preserve the 0x110-byte action message union, 0x104-byte device-manager control block and 0x7C-byte search control block. Six intermediate parameters to the L2CAP compression callback are unused by this target; their source names state that fact without assigning unproven meanings. Existing control blocks, configuration pointers, and callees in other units receive evidenced names only; their storage types, sizes, and splits remain with their owners.

## Function inventory

Names follow the pinned reference, exact instructions, call and callback references, and file order. Five short routines share instruction patterns when relocations are masked: queue clearing versus cancellation completion, and the two disable callbacks, are distinguished by their callees and callback uses. `bta_dm_reset_complete` is the empty callback passed by `bta_dm_send_hci_reset`; `bta_dm_cancel_rmt_name` is the other empty action in the reference position. Complete linked verification checks the final relocation destinations, not only masked instruction patterns.

| Address | Size | Name |
| --- | --- | --- |
| `0x80314A44` | `0x110` | `bta_dm_enable` |
| `0x80314B54` | `0xB0` | `bta_dm_disable` |
| `0x80314C04` | `0xBC` | `bta_dm_disable_timer_cback` |
| `0x80314CC0` | `0x8` | `bta_dm_set_dev_name` |
| `0x80314CC8` | `0x48` | `bta_dm_set_visibility` |
| `0x80314D10` | `0x70` | `bta_dm_bond` |
| `0x80314D80` | `0x9C` | `bta_dm_pin_reply` |
| `0x80314E1C` | `0xD4` | `bta_dm_auth_reply` |
| `0x80314EF0` | `0x60` | `bta_dm_search_start` |
| `0x80314F50` | `0x84` | `bta_dm_search_cancel` |
| `0x80314FD4` | `0x114` | `bta_dm_discover` |
| `0x803150E8` | `0x224` | `bta_dm_inq_cmpl` |
| `0x8031530C` | `0x16C` | `bta_dm_rmt_name` |
| `0x80315478` | `0x64` | `bta_dm_disc_rmt_name` |
| `0x803154DC` | `0x268` | `bta_dm_sdp_result` |
| `0x80315744` | `0x18` | `bta_dm_search_cmpl` |
| `0x8031575C` | `0x50` | `bta_dm_disc_result` |
| `0x803157AC` | `0x88` | `bta_dm_search_result` |
| `0x80315834` | `0x4C` | `bta_dm_search_timer_cback` |
| `0x80315880` | `0x44` | `bta_dm_free_sdp_db` |
| `0x803158C4` | `0x48` | `bta_dm_queue_search` |
| `0x8031590C` | `0x48` | `bta_dm_queue_disc` |
| `0x80315954` | `0x44` | `bta_dm_search_clear_queue` |
| `0x80315998` | `0x44` | `bta_dm_search_cancel_cmpl` |
| `0x803159DC` | `0x5C` | `bta_dm_search_cancel_transac_cmpl` |
| `0x80315A38` | `0x18` | `bta_dm_search_cancel_notify` |
| `0x80315A50` | `0x1A0` | `bta_dm_find_services` |
| `0x80315BF0` | `0xE0` | `bta_dm_discover_next_device` |
| `0x80315CD0` | `0x48` | `bta_dm_sdp_callback` |
| `0x80315D18` | `0x90` | `bta_dm_inq_results_cb` |
| `0x80315DA8` | `0x4C` | `bta_dm_inq_cmpl_cb` |
| `0x80315DF4` | `0x40` | `bta_dm_service_search_remname_cback` |
| `0x80315E34` | `0x8C` | `bta_dm_remname_cback` |
| `0x80315EC0` | `0x88` | `bta_dm_disc_remname_cback` |
| `0x80315F48` | `0x4` | `bta_dm_cancel_rmt_name` |
| `0x80315F4C` | `0xC8` | `bta_dm_authorize_cback` |
| `0x80316014` | `0xC0` | `bta_dm_pinname_cback` |
| `0x803160D4` | `0x130` | `bta_dm_pin_cback` |
| `0x80316204` | `0x8` | `bta_dm_link_key_request_cback` |
| `0x8031620C` | `0x9C` | `bta_dm_new_link_key_cback` |
| `0x803162A8` | `0x84` | `bta_dm_authentication_complete_cback` |
| `0x8031632C` | `0x28` | `bta_dm_local_addr_cback` |
| `0x80316354` | `0x3C` | `bta_dm_signal_strength` |
| `0x80316390` | `0xF4` | `bta_dm_signal_strength_timer_cback` |
| `0x80316484` | `0x70` | `bta_dm_acl_change_cback` |
| `0x803164F4` | `0x280` | `bta_dm_acl_change` |
| `0x80316774` | `0x1C` | `bta_dm_disable_conn_down_timer_cback` |
| `0x80316790` | `0x6C` | `bta_dm_rssi_cback` |
| `0x803167FC` | `0x6C` | `bta_dm_link_quality_cback` |
| `0x80316868` | `0xE4` | `bta_dm_l2cap_server_compress_cback` |
| `0x8031694C` | `0x238` | `bta_dm_compress_cback` |
| `0x80316B84` | `0x108` | `bta_dm_rm_cback` |
| `0x80316C8C` | `0x84` | `bta_dm_keep_acl` |
| `0x80316D10` | `0x1C` | `bta_dm_immediate_disable` |
| `0x80316D2C` | `0x4` | `bta_dm_reset_complete` |
| `0x80316D30` | `0x50` | `bta_dm_send_hci_reset` |

The reference's `bta_sys_check_compress` has no retained target function or diagnostic string, so it is omitted. Helpers such as `bta_dm_adjust_roles`, `bta_dm_init_compress` and the remote-name timer helpers remain in source because their callers inline them. Only standalone target functions contribute to the inventory above.

## Verification

The corrected split-only build and explicit frozen verifier exit 0 before selecting the completed source. The actual `ninja all_source` entry point compiles the entire implementation against the project headers. Default objdiff 3.8.2 comparisons report 100% for `.text`, `.rodata`, `.data`, `.bss` and `.sdata2` in `bta_dm_act.c`, and 100% text for `utl.c` and the complete `ptim.c`.

After selecting the two units as `Matching`, `python configure.py --no-progress --build-dir build/512-completion`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/512-completion/wpse-v1` all exit 0. The source-selected `main.dol` matches frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Verification explicitly names the actual output directory, avoiding the default-path checksum limitation documented in F-511-4. The pull request records final rebased commands and the exact verified head. Scope is the main game executable only, excluding boot content 6, data contents, and WAD packaging.

## Provenance

The action source carries Copyright (C) 2003-2012 Broadcom Corporation and changes by muff1n1634, licensed under Apache-2.0. Applicable notices and modifications are preserved in [the BTE notice](../licenses/510-bte-NOTICE.md#issue-512-additions) and [the complete license](../licenses/510-apache-2.0.txt). New declarations in `l2c_api.h` use the pinned reference's Copyright (C) 1999-2012 Broadcom Corporation material and the same license; the separate shared-header notice is F-512-4. Comments are removed in source as required by this project, while attribution remains in documentation.

## Traps

- **The final C string can end at an invalid automatic-object alignment.** This unit's last diagnostic ends at `0x803E0823`; dtk rejects the following unaligned automatic `.data` object. Retain the natural one-byte alignment tail through `0x803E0824`, preserve the next object's observed start, and verify source payloads and the full executable. This does not justify resizing the actual string or changing expected hashes.
- **Source-level field extents and padded split extents differ.** The UUID table, service table, and compression array were previously described with padded sizes. Compiled symbols prove 0x2E, 0x5C, and 0x2D-byte payloads. Keep those actual sizes and document alignment separately; full source-selected verification confirms the placement.
