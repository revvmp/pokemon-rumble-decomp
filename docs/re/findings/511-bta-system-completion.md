# F-511-4: Complete BTU task, address, BTA system and timer units

## Scope and source coverage

The existing `bte_main.c` callback and eight `bta_sys_conn.c` functions supply nine of #511's 25 functions. This change reconstructs its remaining sixteen functions, 1,956 code bytes. Four complete new source units contain nineteen retained functions and 2,360 code bytes: the additional three are the `ptim.c` timer operations in #512. Issue #511 owns `ptim_init`, the file's first function, so it owns that split, configuration and header. The work does not complete #512's other units.

Earlier descriptions of seventeen remaining fallbacks in F-511-1 predate the callback's acceptance by #510. All 25 #511 functions, 2,600 code bytes, are now covered by source-selected units. The header prerequisite is [F-511-3](511-bta-shared-headers.md), merged as PR #666 before these implementations.

| Unit | Exact section ranges, exclusive ends | Retained functions | Code bytes |
| --- | --- | --- | --- |
| `btu_task1.c` | `.text` `0x80313E70`-`0x803142B4`; `.sdata` `0x804B8EF0`-`0x804B8EF8`; `.sbss` `0x804B9B88`-`0x804B9B90`; `.bss` `0x804AA850`-`0x804AA8D4` | 4 | 1088 |
| `bd.c` | `.text` `0x803142B4`-`0x80314388` | 2 | 212 |
| `bta_sys_main.c` | `.text` `0x803145DC`-`0x80314820`; `.bss` `0x804AA8D8`-`0x804AA964`; `.sbss` `0x804B9B90`-`0x804B9B91`; `.data` `0x803E06B0`-`0x803E06E5` | 9 | 580 |
| `ptim.c` | `.text` `0x80314820`-`0x80314A00` | 4 | 480 |

Function order and names follow the pinned Petari reference `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/{btu_task1,bd,bta_sys_main,ptim}.c`, confirmed by target instruction comparison and relocation destinations. Bluetooth addresses are six bytes: `bdcpy` copies all six, and `bdcmp` returns -1 at the first differing byte and zero otherwise. The unlinked address convenience APIs and their unused constants are omitted. The unlinked `bta_sys_disable_timers` API is retained as reference source and discarded by the linker; it is excluded from retained function and byte counts.

## Compiler evidence

Wii/1.0 with the existing `cflags_base` reproduces all nineteen retained instruction streams and every source object's data. `btu_task1.c` needs the compiler's default sixteen-byte function alignment; the other three new units need `-func_align 4`. The BTU task handler is 0x37C bytes with Wii/1.0, exactly the target; GC/3.0a3 with the same optimization options produces 0x38C bytes and different scheduling/control flow. Its three neighboring task/timer functions match with either compiler when considered separately. The target task-unit starts are all sixteen-byte aligned: `0x80313E70`, `0x80313EB0`, `0x80314230`, and `0x803142A0`. The address, BTA system and periodic-timer units match with both tested compilers, so their exact compiler revision is not distinguished; Wii/1.0 is the verified selection, consistent with the existing connection unit. This finding does not replace the measured GC compiler selection for other BTE units.

## Layout and behavior

`btu_cb` occupies 0x84 bytes at `0x804AA850`. The task polls mailbox zero and mailbox two, dispatches HCI and registered events, updates timer queues, and routes expiry callbacks. Its reentrancy guard is the four-byte `execute_btu` at `0x804B8EF4`; `btu_count` is a byte at `0x804B8EF0`. The two four-byte counters are `_btu_g_count` at `0x804B9B88` and `_btu_last_timer_tick` at `0x804B9B8C`. These original reference names and the observed 500-count timer threshold are preserved, as is the target's `OSGetTime` call.

`bta_sys_cb` occupies 0x8C bytes at `0x804AA8D8`, ending at `0x804AA964`. Its previous 0x90-byte symbol included four bytes of following alignment; those bytes remain outside this unit. The canonical control block layout is detailed in F-511-3. `appl_trace_level` is one byte at `0x804B9B90`. The two event diagnostics are independent strings at `0x803E06B0` (19 bytes) and `0x803E06C4` (33 bytes); the data section ends at the final string's actual end, `0x803E06E5`.

The system configuration pointer at `0x804B8ED8` is `p_bta_sys_cfg`; initialization reads timer id at configuration offset 3 and trace level at offset 4, while message sending reads mailbox at offset 2. Only this evidenced external name is changed; its owner's type, size, storage and split remain unchanged. The timer control block retains its queue at zero, signed period at 0x0C, and timer id at 0x10. Period conversion is the reference's integer division by ten (100 ticks per second); expiration callbacks and message dispatch preserve reference order.

## External names needed for the source link

Target branch destinations paired with reference call sequences establish these names. Existing names are reused; newly named external symbols retain their owners' types, extents and splits.

| Address | Name |
| --- | --- |
| `0x800046B4` | `memset` |
| `0x802BD060` | `OSGetTime` |
| `0x80310ED0` | `GKI_getbuf` |
| `0x80311160` | `GKI_freebuf` |
| `0x80311320` | `GKI_send_msg` |
| `0x803114E0` | `GKI_read_mbox` |
| `0x80311F60` | `GKI_start_timer` |
| `0x803120B0` | `GKI_stop_timer` |
| `0x80312140` | `GKI_init_timer_list` |
| `0x80312160` | `GKI_update_timer_list` |
| `0x803121F0` | `GKI_add_to_timer_list` |
| `0x803122E0` | `GKI_remove_from_timer_list` |
| `0x80312640` | `GKI_get_taskid` |
| `0x80312650` | `GKI_enable` |
| `0x80312680` | `GKI_disable` |
| `0x80313B30` | `BTE_InitStack` |
| `0x80313C40` | `LogMsg_1` |
| `0x80316D10` | `bta_dm_immediate_disable` |
| `0x8031B424` | `btm_acl_timeout` |
| `0x8031C930` | `btm_dev_timeout` |
| `0x8031E048` | `btm_discovery_timeout` |
| `0x8031FAA4` | `btm_inq_rmt_name_failed` |
| `0x80321204` | `btm_route_sco_data` |
| `0x803245D0` | `btu_hcif_process_event` |
| `0x80324B88` | `btu_hcif_send_cmd` |
| `0x803255F4` | `btu_hcif_cmd_timeout` |
| `0x8032582C` | `btu_init_core` |
| `0x8032AAC8` | `hidh_proc_repage_timeout` |
| `0x8032F698` | `l2c_link_segments_xmitted` |
| `0x8032F8CC` | `l2c_rcv_acl_data` |
| `0x8033066C` | `l2c_process_timeout` |
| `0x80338828` | `rfcomm_process_timeout` |
| `0x8033C3CC` | `sdp_conn_timeout` |
| `0x804B8ED8` | `p_bta_sys_cfg` |

## Verification

The split-only configuration, normal full build and frozen verifier exit 0 before source registration. The registered `NonMatching` objects compile through the actual `ninja all_source` entry point. Default objdiff 3.8.2 comparisons between the dtk original objects and compiler outputs report 100% for every code/data section of all four units. The existing complete `bte_main.c` and `bta_sys_conn.c` objects are independently rechecked at 100%.

After selecting the four new sources as `Matching`, `python configure.py --no-progress --build-dir build/511-source-clean`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/511-source-clean/wpse-v1` all exit 0 with the source-selected executable. The pull request records final rebased command results and verified commit. Expected frozen SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope is the main game executable only, excluding boot content 6, data contents, and WAD packaging.

## Provenance

See [issue 511 system-source notices](../licenses/511-bta-system-NOTICE.md) for the pinned reference, file-specific Broadcom Apache-2.0 notices, preserved license, and adaptations. `btu_task1.c` has no file-specific notice in that reference and is covered by Petari's root CC0; the associated imported header material retains its Broadcom notices.

## Traps

- **BTE compiler and alignment both vary by unit.** A direct GC/3.0a3 build of the BTU task handler is sixteen bytes too long. Wii/1.0 reproduces its instructions, but four-byte alignment loses four padding bytes before `btu_start_timer`, shifting every later function and failing the frozen comparison first at DOL offset `0x2A734A`, despite whole-unit objdiff 100%. Use Wii/1.0 default sixteen-byte function alignment for this complete unit; retain four-byte alignment only for the packed units. The real linked symbol addresses and full frozen comparison are required.
- **A control-block symbol can include following alignment.** The existing `bta_sys_cb` symbol spanned 0x90 bytes but initialization clears 0x8C and the source object allocates 0x8C. End its split at `0x804AA964` and leave the following four alignment bytes to the next object, preserving the established neighboring address.

- **An alternate build directory leaves the Ninja checksum input path unchanged.** The generated `check` rule invokes `dtk shasum -c config/wpse-v1/build.sha1`, whose immutable entry names `build/wpse-v1/main.dol`, even when the rebuilt output is under `build/511-source-clean/wpse-v1`. During the alignment failure, Ninja printed `build/wpse-v1/main.dol: OK` for the older default output while explicit frozen verification of the new output failed. Keep the original manifest unchanged and always run the frozen verifier with the actual alternate `--build-dir`; a successful alternate-root Ninja checksum alone is not acceptance evidence.
