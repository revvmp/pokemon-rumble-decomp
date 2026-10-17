# F-496-1: ESP and IPC main reconstruction

Target `wpse-v1`, frozen `main.dol` SHA-256 `ad1a5989` prefix. Issue #496 owns 21 functions from `0x802F08E0` through `0x802F1318`; final text padding extends through `0x802F1320`.

## Units and provenance

Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, CC0, supplies `src/RVL_SDK/esp/esp.c`, `src/RVL_SDK/ipc/ipcMain.c`, and the ESP/iostypes declarations. Comments were removed. Four ESP content-file operations absent from that reference were independently reconstructed from the original instructions, preserving the aligned request buffers and ioctl vector layout. Their names are corroborated by the boot loader strings described in F-0009.

| Unit | Text range | Data range |
| --- | --- | --- |
| esp.c | `0x802F08E0`-`0x802F1230` | `.sdata` `0x804B8D80`-`0x804B8D90` |
| ipcMain.c | `0x802F1230`-`0x802F1320` | `.sbss` `0x804B99D8`-`0x804B99F0` |

The ESP static descriptor is at `0x804B8D80`; the string `/dev/es` starts at `0x804B8D88`. IPC accesses establish five statics at `0x804B99D8`, `0x804B99DC`, `0x804B99E0`, `0x804B99E4`, and `0x804B99E8`. The last is a four-byte pointer, followed by four bytes of alignment padding; the previous eight-byte automatic symbol incorrectly included the padding. A split-only full build and exact verifier both pass.

## Source and code generation

Wii/1.0 with SDK inline level 3, file IPA, and eight-byte small-data thresholds reproduces all 21 functions. `IPCReInit` inlines `IPCInit`. ESP buffers require 32-byte alignment and vectors begin at byte 208 of the 256-byte buffer, matching Petari.

A pointer macro for IPC registers generated `slwi/addis/lwz`, while the original uses `slwi/lis/lwzx`. The reference's CodeWarrior absolute-address array declaration, `volatile u32 __IPCRegs[4] : 0xCD000000`, reproduces both register-access functions exactly. This is verified code-generation evidence, not a general claim that pointer macros are interchangeable with absolute arrays.

## Remaining external-name dependencies

Both units deliberately remain `NonMatching`, retaining their original split objects in the final link. Their source imports use evidenced original names, while four external symbols outside #496 still have automatic labels:

| Owning issue | Address | Required name | Evidence |
| --- | --- | --- | --- |
| #470 | `0x802BD980` | `__OSGetIPCBufferHi` | IPCInit first call, Petari ipcMain |
| #470 | `0x802BD990` | `__OSGetIPCBufferLo` | IPCInit second call, Petari ipcMain |
| #497 | `0x802F1DF0` | `IOS_Close` | ESP_CloseLib call and Petari esp |
| #497 | `0x802F2A10` | `IOS_IoctlvReboot` | ESP_LaunchTitle call and Petari esp |

No outside-claim symbols were edited. After the owning issues rename these symbols, switch the units to Matching only after rebuilding and passing the frozen verifier. Direct objdiff retains relocation-name differences in four functions; the generated project report ignores those label mismatches and reports 21/21 functions, 2492/2492 code bytes, and 40/40 data bytes matched. Zero bytes from these units are linked from source yet. Full-link verification currently validates the split/fallback build, not source-link identity.

## Validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0 after both splits and source registration. Direct `objdiff-cli diff` comparisons were also run for both units. Existing unrelated exception-table warnings remain.

No source fallback was removed and #496 remains incomplete until the four external names are available and both units pass source-linked verification.
