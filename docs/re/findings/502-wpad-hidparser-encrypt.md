# Issue 502: RVL_SDK WPADHIDParser.c and WPADEncrypt.c

## F-502-1: Unit boundaries and sections

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x802FE620`–`0x80303A54` starts with the three WPAD.c accessors that #501 already owns (F-501-1). After the padding at `0x802FE648`–`0x802FE650`, it holds two translation units and the start of a third.

| Unit | Section ranges | Evidence |
| --- | --- | --- |
| `RVL_SDK/wpad/WPADHIDParser.c` | `.text` `0x802FE650`–`0x80302914`; `.rodata` `0x80397E18`–`0x80397E30`; `.data` `0x803DCF90`–`0x803DD500`; `.bss` `0x8047AC40`–`0x8047ACA8`; `.sbss` `0x804B9AB0`–`0x804B9AC8`; `.sdata2` `0x804BB508`–`0x804BB568` | The 32-entry report parser table at `0x803DCF90` points only at functions in this range. The DEBUGPrint strings that follow it in `.data` are all used by these functions. `.text` ends at the 4-byte `__a1_unused_report` (`blr`), and the padding to `0x80302920` goes to the next object, as in F-495-1 |
| `RVL_SDK/wpad/WPADEncrypt.c` | `.text` `0x80302920`–`0x803039E0`; `.data` `0x803DD500`–`0x803DE828`; `.sbss` `0x804B9AC8`–`0x804B9ACC` | `.data` holds two 42-byte seed tables and two 0x900-byte substitution tables, followed by the `"ans"`, `"rand"`, `"t0"`, `"key"`, `"ft"` and `"sb"` print formats. `.sbss` holds the four one-byte generator states. `WPADiClearMemBlock` at `0x803039E0` begins WPADMem.c (#503) |

The `.rodata` object is the 21-byte `_cExtInvalidData` (all `0xFF`) plus padding. `.sdata2` holds the parser's float and double constants.

Both units compile with Wii/1.0 and `cflags_sdk` at the default 16-byte function alignment, like the rest of WPAD (F-0007). Both link from source, and full DOL verification passes. No `align:` attribute was needed on `lbl_803DD500`: the source `.data` of WPADHIDParser.c ends six bytes short of the split, and the link pads to the original start of WPADEncrypt.c.

## F-502-2: Function names

Public names are high confidence, from the call in `__wpadReceiveCallback` (F-501-2), from WPAD.c's calls to the key and decode functions, and from SDK convention. Static names are medium confidence. They follow the CC0 key reconstruction of the WPAD library (Swiftshine/key), the 2009-12 robojumper reconstruction (unlicensed; read only as a naming and ordering aid), and Petari CC0 `src/RVL_SDK/wpad/WPADHIDParser.c` at `e0ab141`.

| Address | Name | Evidence |
| --- | --- | --- |
| `0x802FE650` | `__wpadAbortInitExtension` | Extension command failure path: clears `extq`, retries `__wpadInitExtension` up to 32 times while attached, then reports `WPAD_DEV_NOT_FOUND` or `WPAD_DEV_NOT_SUPPORTED` through `extensionCallback` |
| `0x802FE7B0` | `__wpadGetDevConfig` | Prints the `dpd`/`acc` calibration checksums; fills `conf` |
| `0x802FF2A0` | `__wpadGetExtConfig` | Checksums the 16-byte extension blocks; fills `extConf` for the Nunchuk and Classic |
| `0x802FF7E0` | `__wpadGetExtType` | Prints `devId: %d, subId: %d`; maps the extension ID bytes to `_wpadDevType` and `_wpadDevMode` |
| `0x802FFC20` | `__wpadGetGameInfo` | Checksums the 47-byte game info read (plus `0x55`) and copies it to `gameInfo`, setting `gameInfoErr[index]` |
| `0x802FFDD0` | `WPADiHIDParser` | Dispatches through `__a1_input_reports_array` |
| `0x802FFF00` | `__a1_20_status_report` | Report `0x20`: battery, attachment, extension (re)initialisation |
| `0x80300240` | `__a1_21_user_data` | Report `0x21`: memory read data, `base addr` and `length` prints |
| `0x80300570` | `__a1_22_ack` | Report `0x22`: acknowledgement, `err` print |
| `0x803006B0` | `__parse_dpd_data` | Standard and basic DPD formats, then rotation by `_wpadRolag` |
| `0x80300B50` | `__parse_dpdex_data` | Extended DPD object |
| `0x80300DD0` | `__parse_cl_data` | Classic Controller formats; `_wpadCLCompt` |
| `0x803012A0` | `__parse_vs_data` | VSM status |
| `0x80301460`–`0x80302760` | `__a1_30_data_type` … `__a1_3f_data_type` | Reports `0x30`–`0x37`, `0x3D`, `0x3E`, `0x3F`, in table order |
| `0x80302910` | `__a1_unused_report` | `blr`; fills the unused table entries |
| `0x80302920` | `WPADiCreateKey` | Seeds from `OSGetTime`, builds `key`, `ft` and `sb` from `base` and `tb` |
| `0x80303010` | `WPADiCreateKeyFor3rd` | Same structure with `base3`, `tb3` and rotate-left |
| `0x80303710` | `WPADiDecode` | `data[i] = ft[k] + (data[i] ^ sb[k])`, `k = (offset + i) % 8` |

`__wpadIsExtEncMain`, `__wpadInitExtension`, `__wpadInvertRxBuffer`, `__wpadGetWorkRxBuffer`, `__wpadGetLastRxBuffer`, `__wpadRetrieveButton`, `__parse_acc_data`, `__clamp`, `__parse_fs_data`, `__parse_tr_data`, and WPADEncrypt.c's `random`, `ror` and `rol` are inlined at every use and have no copy in the DOL.

## F-502-3: Code generation notes

- `__parse_dpd_data`: the target keeps the object address from the `fy` load in a saved register and stores `x` through it after the `sin` and `cos` calls. It still reloads `*status` for the `y` store. Writing every access as `status[0]->obj[i]` reloads after the calls (one fewer saved register). A pointer for the whole object CSEs with the loop condition. Only a `DPDObject*` assigned after the `fy` line and used for the `x` store alone reproduces the target. Compiler versions (Wii/1.0a to 1.5, GC/3.0a3 and 3.0a5.2), `__attribute__((const))`, `#pragma side_effects` and system-path includes for `math.h` had no effect.
- `__wpadGetExtConfig`: the Nunchuk and Classic parses are written inline. Separate static helpers containing DEBUGPrint calls exceed the auto-inline limit and are emitted out of line.
- Register order in several report handlers depends on local declaration order. `__a1_3e_data_type` and `__a1_3f_data_type` declare `status` before the control block pointer; `__a1_22_ack` declares `enabled` before `status`.
- `.sbss` objects are laid out in reverse declaration order.
- In this revision, `WPADiDecode` has no `extEnc` check.

## Source provenance

Written from the target disassembly. Structure and names follow Swiftshine/key (CC0) and Petari (CC0, `e0ab141`). robojumper's sdk_2009-12-11 was read only as a naming aid; no text was copied from it. The WPADEncrypt.c tables were generated from the DOL bytes at `0x803DD500`.

## Traps

- **A `.sbss` split can end inside a dtk-guessed object.** dtk refused the WPADEncrypt.c `.sbss` end at `0x804B9ACC` because `lbl_804B9ACB` was guessed at five bytes. It is the fourth one-byte generator state; size `0x1` fixes the split.
- **Shell heredocs corrupt C string escapes.** Python heredocs that rewrote source turned `\n` inside DEBUGPrint formats into real newlines. Edit such files with the Edit tool or with a script file, not with inline heredocs that contain backslashes.
- **A reload around calls can be a source-shape difference, not a compiler one.** See F-502-3. Try pointer temporaries scoped to a single store before changing compiler versions.
