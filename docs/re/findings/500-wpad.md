# Issue 500: RVL_SDK WPAD.c (first block)

## F-500-1: WPAD.c unit boundaries

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The WPAD banner at `.data` `0x803DCB68` reads `<< RVL_SDK - WPAD \trelease build: Jun 22 2009 18:33:21 (0x4302_145) >>`. PAD's shutdown record ends at `0x803DCB68`; the WPAD `OSShutdownFunctionInfo` at `0x803DCC18` points to `0x802F6CE0`. `__PADDisableRecalibration` ends at `0x802F6BBC`, so WPAD.c begins at `0x802F6BC0`.

The unit ends at `0x802FE650`. `0x802FE530` and `0x802FE5C0` are the command-queue helpers, `0x802FE620`–`0x802FE640` are three one-instruction accessors over WPAD `.sbss`, and `0x802FE650` is the first WPADHIDParser function: it clears the extension queue and is the first code to reference the HID parser table at `.data` `0x803DCF90`. This boundary falls 0x30 bytes past the #501/#502 survey estimate.

Every data reference from `0x802F6BC0`–`0x802FE650` resolves to these WPAD-owned ranges (plus OS's `__OSIsReturnToIdle`):

| Section | Range |
| --- | --- |
| `.text` | `0x802F6BC0`–`0x802FE650` |
| `.rodata` | `0x80397DD8`–`0x80397E18` |
| `.data` | `0x803DCB68`–`0x803DCF90` |
| `.bss` | `0x804774E0`–`0x8047AC40` |
| `.sdata` | `0x804B8DC8`–`0x804B8DF8` |
| `.sbss` | `0x804B9A50`–`0x804B9AB0` |
| `.sdata2` | `0x804BB500`–`0x804BB508` |

`.sbss` `0x804B9A48` belongs to PAD (used at `0x802F5D0C`), and `.sbss` `0x804B9AB0` onward and `.sdata2` `0x804BB508` belong to WPADHIDParser. The split builds byte-identically. Confidence high.

## F-500-2: Control block layout and static data

`__wpadClearControlBlock` (`0x802F8720`) and `__wpadInitSub` (`0x802F89A0`) fix the control block layout. Each block is 0x9C0 bytes and 32-byte aligned (`_wpd` at `.bss` `0x80478540`, stride `0x9C0`). The SMG-era layout in Petari's CC0 `WPADControlBlock` holds through `pwrSave` at `0x98E`. After it come `blcBattery` (`0x98F`, cleared to 4), `savedDevType` (`0x990`, 253), the extension-disconnect flag and countdown (`0x991`/`0x992`), a DPD busy byte (`0x994`), and the WPADMem block (`0x998`, 0x14 bytes, used by `0x803039E0`). This version has three noise filters (accelerometer, DPD, extension): the counters are `u16[3]` arrays at `0x8F8` and `0x8FE`, and the `.sdata` thresholds are `{6, 4, 6}` and `{30, 30, 30}`. The extension configuration is a 0x1A-byte union with no Motion Plus calibration, so `extConf` sits at `0x882`.

The `.sbss` variables are laid out in reverse declaration order: `_wpadRegisterShutdownFunc` sits at `0x804B9A50`, `_wpadGameType` at `0x804B9AA8`. The getters at `0x802F6BE0`–`0x802F6C10` read `_enabledVSM`, `_enabledTRN`, `_enabledGTR`, and `_enabledDRM`. The HID parser picks the device type with them for extension IDs 4 (VSM) and 16 (train) and for guitar/drum sub-IDs 0 and 1. The WBC entry points at `0x802F6C20`–`0x802F6CDC` are dummies that print `WBC*Dummy` through `DEBUGPrint` (`0x80304600`, an empty variadic). Their `WPAD_ERR_NO_CONTROLLER` result marks them as weak defaults for the balance-board library.

Static function names and the declaration order of the static data follow the 2009-12-11 SDK reconstruction by robojumper (`sdk_2009-12-11`, no license). It was read only as a naming and ordering aid; no text was copied. The reverse-order `.sbss` layout reproduces its declaration order exactly. Control block field names follow Petari CC0 `libs/RVL_SDK/include/revolution/wpad.h` at `e0ab141`. Field names after `pwrSave`, and the VSM status field names, are descriptive and low confidence.

## F-500-3: Reconstructed functions and compiler behavior

With Wii/1.0 and `cflags_sdk`, all 23 functions from `0x802F6BC0` to `0x802F8BFC` match at 100% in objdiff 3.8.2: 8,120 code bytes, `build/wpse-v1/report.json`. The full DOL still verifies with the unit `NonMatching`, because the rest of WPAD.c belongs to #501 and #502.

Observed compiler behavior:

- `-ipa file` inlines static helpers defined later in the file, such as queue access, connection status, `WPADGetAddress` and the SC getters. Unreferenced static functions are dropped from the object, so a partial file needs its real roots. `WPADInit` (`0x802F8C00`) and `WPADGetInfo` (`0x802FA330`) are therefore written too; both match, but their symbols belong to #501.
- Register assignment follows declaration order for values live across calls. `__wpadSendDataSub` matches only when its locals are declared before they are assigned, in the order enabled, rumble, handle, block, report id, buffer, report, status, data, length.
- `__wpadGetDpdSensitivity` needs a `u32` local for its clamp. A `u8` local re-extends after each assignment.
- The reconnect countdown keeps `_wpadOnReconnect` in a register across the debug print only when passed as a parameter. A local copy is re-read from memory.
- `__wpadSendDataSub` returns no value here, unlike the reference, and `__wpadSendData` tests busy status in a separate `if` after sending.

## Traps

- **Partial SDK files drop unreferenced statics.** With `-ipa file`, a WPAD.c holding only one block's static functions compiled to an object without them, because nothing referenced them. Include the real external roots, such as `WPADInit`, and any later helpers the block inlines (F-500-3).

## F-500-4: Shared header adjustments for this SDK version

Following D-0013, the reconstruction uses the imported Petari headers, adjusted in place. In `include/revolution/wpad.h`, the `WPADControlBlock` layout moves to the 2009 layout of F-500-2: a two-dimensional `rxBuf`, three-filter `diffCount`/`hystCount` arrays, and the trailing balance board, extension, DPD-busy and embedded WPADMem fields. The header also gains formats 10–15, device modes, and the TR, BL and VS status structures, along with the `WPADIsEnabled*`, `WBC*`, `WPADGetInfo`, `WPADRecalibrate`, `WPADiClearMemBlock` and `DEBUGPrint` prototypes. `include/revolution/sc.h` now declares `SCGetBtDpdSensibility` as returning `u8`; Petari had `u32`. The callers in `__wpadInitSub` and `__wpadManageHandler` zero-extend the result before clamping it, which matches only with a `u8` return. Other additions: alarm, app and tick declarations in `include/revolution/os.h`; shutdown event values in `OSReset.h`, following the event codes in Petari OSReset/OSExec; `OSError` in `OSError.h`; `__VIResetRFIdle` in `vi.h`; and two WUD prototypes in `wud/wud.h`.
