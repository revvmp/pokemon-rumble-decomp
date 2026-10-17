# Issue 508: BTE GKI (gki_buffer.c, gki_time.c, gki_ppc.c)

## F-508-1: Unit boundaries and sections

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x80311320`–`0x80312584` lies inside three GKI units. The units are split whole, so the work also covers the start of gki_buffer.c in #507 and the end of gki_ppc.c in #509.

| Unit | Section ranges | Evidence |
| --- | --- | --- |
| `RVL_SDK/bte/gki_buffer.c` | `.text` `0x803109E0`–`0x80311E78`; `.data` `0x803E04F8`–`0x803E063A` | Starts with `gki_init_free_queue` and `gki_buffer_init`, and ends with `GKI_delete_pool`. The thirteen exception strings, from `getbuf: Size is zero` to `Deleting bad pool`, appear in source order. `GKI_get_buf_start:: bad addr` belongs to `GKI_find_buf_start`, which the linker removed, so that function was compiled into the original unit |
| `RVL_SDK/bte/gki_time.c` | `.text` `0x80311E80`–`0x80312404` | From `gki_timers_init` to `GKI_remove_from_timer_list`. No data |
| `RVL_SDK/bte/gki_ppc.c` | `.text` `0x80312410`–`0x80312714`; `.bss` `0x8047E4C8`–`0x804A6FA8` | From `GKI_init` to `GKI_os_free`. The unit owns `gki_cb` (0x28AE0 bytes, the size of `tGKI_CB` with this revision's pool counts). hcisu_h2.c starts at `0x80312720` (F-509-1) |

All three units compile with Wii/1.0, `cflags_base` and `-ipa file`, at default 16-byte alignment. All three link from source, and full DOL verification passes.

## F-508-2: Function names

Names follow Petari (`e0ab141`) and the bluedroid GKI. Every name is high confidence: the exception strings, the pool sizes and the call graph match the reference.

| Address | Name |
| --- | --- |
| `0x803109E0` | `gki_init_free_queue` (static) |
| `0x80310C00` | `gki_buffer_init` |
| `0x80311E80` | `gki_timers_init` |
| `0x80312560` | `GKI_sched_lock` |
| `0x80312570` | `GKI_sched_unlock` |
| `0x80312590` | `GKI_send_event` |
| `0x803126D0` | `GKI_exception` |
| `0x803126E0` | `GKI_os_malloc` |
| `0x80312710` | `GKI_os_free` |

The other GKI functions already had names. Outside names for the link: `App_MEMalloc` `0x80309FB0` and `App_MEMfree` `0x8030A000`, which `GKI_os_malloc` and `GKI_os_free` call. `lbl_8047E4C8` becomes `gki_cb`.

## F-508-3: Code generation notes

- Without `-ipa file`, the compiler did not inline `gki_chk_buf_damage`, `gki_adjust_timer_count`, `GKI_exit_task`, `GKI_enable` or `GKI_disable` into earlier callers. The target inlines all five, and the function order stays as in the reference. With `-ipa file`, all functions in the three units compare at 100%.
- The revision has no asserts in `GKI_enable` and `GKI_disable`.
- Functions absent from the DOL with no data evidence are left out of the source: `GKI_igetpoolbuf`, `GKI_poolfreecount`, `GKI_change_buf_owner`, `GKI_ready_to_sleep`, `GKI_timer_update`, `GKI_init_timer_list_entry`, `GKI_create_task`, `GKI_suspend_task`, `GKI_resume_task`, `GKI_wait`, `GKI_isend_event`, `GKI_map_taskname`, `GKI_get_time_stamp` and `GKI_register_mempool`. `GKI_map_taskname` would add a `BAD` string that the target lacks.
- Some functions are kept even though the linker removes them, because other code inlines them or their strings are present: `GKI_set_pool_permission`, `gki_chk_buf_damage`, `GKI_find_buf_start`, `gki_adjust_timer_count` and `GKI_exit_task`.
- This revision's pool constants are pools 0–4 of 64/48, 128/26, 660/45, 1800/30 and 0x2000/9. There are five fixed and nine total pools, and the default permission mask is `0xFFF0`.

## Traps

- **A prune regex can match a call site.** A pattern that found `name(...) {` also matched `if (gki_chk_buf_damage(p_buf)) {` inside `GKI_freebuf`, and deleted part of the caller. Anchor function-definition patterns at a line start with a return type.
- **Inlining of later-defined functions needs `-ipa file`.** The GKI units, unlike hcisu_h2.c and uusb_ppc.c, inline functions that are defined later in the file. Try `-ipa file` before reordering source when a later function is inlined in the target.
