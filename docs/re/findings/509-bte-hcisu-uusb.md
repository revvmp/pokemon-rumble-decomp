# Issue 509: BTE hcisu_h2.c and uusb_ppc.c

## F-509-1: Unit boundaries and sections

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x80312590`–`0x80313A54` holds the end of gki_ppc.c and two complete units.

| Unit | Section ranges | Evidence |
| --- | --- | --- |
| `RVL_SDK/bte/gki_ppc.c` (not split here) | `.text` `0x80312410`–`0x80312720` | Runs from `GKI_init` at `0x80312410` (#508) to `GKI_os_free`, a tail branch to the WUD free function at `0x8030A000`. The seven functions from `0x80312590` belong to this issue: `GKI_send_event`, `GKI_get_taskid`, `GKI_enable`, `GKI_disable`, `GKI_exception`, `GKI_os_malloc` and `GKI_os_free`. The unit owns `gki_cb` (`.bss` `0x8047E4C8`, 0x28AE0 bytes). gki_buffer.c and gki_time.c share that layout, so the unit was split with them under #508 ([F-508-1](508-bte-gki.md#f-508-1-unit-boundaries-and-sections)) |
| `RVL_SDK/bte/hcisu_h2.c` | `.text` `0x80312720`–`0x80312DDC`; `.rodata` `0x80397E30`–`0x80397E44`; `.data` `0x803E0640`–`0x803E06AB`; `.bss` `0x804A6FA8`–`0x804A6FCC`; `.sdata2` `0x804BB5E0`–`0x804BB5F0` | `hcisu_h2` is the `tHCI_IF` that bte_main.c installs (F-510-2). The two `HCIS:` strings, the 0x24-byte control block and the preamble and event tables are used only here |
| `RVL_SDK/bte/uusb_ppc.c` | `.text` `0x80312DE0`–`0x80313A54`; `.bss` `0x804A6FE0`–`0x804A9040`; `.sdata` `0x804B8EA0`–`0x804B8EB0`; `.sbss` `0x804B9B60`–`0x804B9B78` | The USB transport. It holds the control block, two 0x1000-byte fiber stacks for `bta_ci_hci_msg_handler`, `wait4hci`, the IOS descriptor and the `oh0`/`oh1` device names. bte_hcisu.c starts at `0x80313A60` (F-510-1) |

Both units compile with Wii/1.0 and `cflags_base` at default 16-byte alignment, like the neighbouring bte_hcisu.c. Both link from source, and full DOL verification passes.

Data symbol changes in `symbols.txt`:

- `hcisu_h2` shrinks from 0x18 to 0x14 bytes, so that the split can end inside the unit.
- `lbl_804A6FA8` becomes `hcisu_h2_cb` (0x24 bytes).
- `lbl_804BB5E0` is split into `hcisu_preamble_table` (4 bytes) and `hcisu_msg_evt_table`.
- `lbl_804A6FE0` becomes `usb` (0x4C bytes).
- The two stacks become `__uusb_ppc_stack1` and `__uusb_ppc_stack2` (0x1000 bytes each, `align:32`).
- dtk had split the second stack at `lbl_804A8046` and `lbl_804A84FC`, because of two false pointers in `.rodata`. Both labels are removed. The rodata word at `0x803691CE` now relocates against `__uusb_ppc_stack2 + 6`, and its bytes are unchanged.

## F-509-2: Function names

Names follow Petari (`e0ab141`) and match the role of each function in the disassembly. The public `hcisu_h2_*` and `UUSB_*` names are high confidence; the static `uusb_*` callbacks are medium confidence.

| Address | Name |
| --- | --- |
| `0x80312720` | `hcisu_h2_usb_cback` |
| `0x80312740` | `hcisu_h2_receive_msg` |
| `0x80312AE0` | `hcisu_h2_send_msg_now` |
| `0x80312C90` | `hcisu_h2_init` |
| `0x80312CC0` | `hcisu_h2_open` |
| `0x80312D30` | `hcisu_h2_close` |
| `0x80312D70` | `hcisu_h2_send` (inlines `hcisu_h2_send_msg`) |
| `0x80312DA0` | `hcisu_h2_handle_event` |
| `0x80312DE0` | `uusb_CloseDeviceCB` |
| `0x80312E80` | `uusb_ReadIntrDataCB` |
| `0x80313030` | `uusb_ReadBulkDataCB` |
| `0x803131D0` | `uusb_WriteCtrlDataCB` |
| `0x803132C0` | `uusb_WriteBulkDataCB` |
| `0x803133A0` | `UUSB_Register` |
| `0x803135B0` | `UUSB_Open` |
| `0x80313740` | `UUSB_Read` |
| `0x80313750` | `UUSB_Write` |
| `0x80313960` | `UUSB_Close` |
| `0x80313A10` | `UUSB_Unregister` |

Names outside the block, given for the link with no change to sizes or splits:

- GKI (#507/#508): `GKI_get_buf_size` `0x803112D0`, `GKI_create_pool` `0x803119C0` and `GKI_delete_pool` `0x80311D10`. Their order (after `GKI_freebuf` and `GKI_queue_is_empty`) follows Petari's gki_buffer.c. Pool creation takes the arguments 660/45 and 1800/30.
- USB library (#504): `IUSB_OpenLib` `0x80308BB0`, `IUSB_CloseLib` `0x80308CB0`, `IUSB_OpenDeviceIds` `0x80308E60`, `IUSB_CloseDeviceAsync` `0x80308FB0`, `IUSB_ReadIntrMsgAsync` `0x803097A0`, `IUSB_ReadBlkMsgAsync` `0x80309820`, `IUSB_WriteBlkMsgAsync` `0x803098C0` and `IUSB_WriteCtrlMsgAsync` `0x80309F10`. Their order and argument use follow the CC0 key `usb.c`; the control write passes its ninth argument on the stack.

`tHCI_CFG` is completed as `{vid, pid}`. bte_main.c supplies `{0x0A5C, 0x2101}`, which is the Broadcom vendor ID and the BCM2045 product ID, and `hcisu_h2_open` copies the pair into `tUUSB`.

## F-509-3: Code generation notes

- Unlike Petari's reference, this revision inlines `uusb_issue_intr_read`, `uusb_issue_bulk_read`, the pool release helpers, `uusb_set_trace_state` and `uusb_get_devId`.
- `uusb_CloseDeviceCB` repeats the `UUSB_Unregister` body. Without `-ipa file`, the compiler cannot inline a function that is defined later in the file.
- The read loops need `while (!p_buf)`. Comparing against `NULL` keeps a zero in a saved register and changes the allocation.
- `UUSB_Write` and `uusb_ReadIntrDataCB` each need one unused, zero-initialised leading local (`bytes_sent`, `bytes_read`) to reproduce the register numbering.
- The receive loop is `while (TRUE)`, with a `break` when a read returns nothing.

## Traps

- **dtk can split a BSS array at false pointers.** A rodata word that happened to equal an address inside `__uusb_ppc_stack2` created `lbl_804A8046` and `lbl_804A84FC`. With the source object linked, the link failed with `undefined: 'lbl_804A8046'`. Remove such labels and give the real object its full size. dtk then relocates the word against the object plus an offset.
- **`revolution/usb.h` pulled in all of `revolution.h`.** Through `wpad.h` and `sc.h`, that included the legacy `include/revolution/bte.h`, which redeclares `tBT_UUID` against `include/revolution/bte/`. `usb.h` now includes only `revolution/types.h` and `private/iostypes.h`, and `IPCCltInit` is also declared in `ipc/ipcclt.h`.
