# Issue 289: HBM homebutton core, block 3

## F-289-1: HBMController.cpp spans 0x8017A010 to 0x8017AC20

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The first 14 functions of this block (`0x80179D50`–`0x8017A008`) end `RVL_SDK/hbm/HBMGUIManager.cpp`, matched by #606; see [F-287-3](287-hbm-core.md#f-287-3-hbmguimanagercpp-is-one-unit-from-0x801787f0-to-0x8017a008). The remaining 12 start `RVL_SDK/hbm/HBMController.cpp`, the `homebutton::Controller` unit. It continues into #290's block and ends at `0x8017AC20`, where `homebutton::RemoteSpk::UpdateSpeaker` (`HBMRemoteSpk.cpp`) begins. #289 owns the whole unit.

Boundaries:

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8017A010`–`0x8017AC20` | 25 functions |
| `.rodata` | `0x80366A58`–`0x80366A68` | `1.0f`, `-10000.0f`, `-15000.0f`, `10.0f` |
| `.bss` | `0x803E99E8`–`0x803E9B80` | `sBatteryFlag` (`0x803E99E8`, 4 bytes, then 4 alignment bytes), `sAlarm` (`0x803E99F0`, 4 `OSAlarm`), `sAlarmSoundOff` (`0x803E9AB0`), `sThis` (`0x803E9B70`, 4 pointers) |

The unit has no `.data`. The next `.bss` object, `0x803E9B80`, is the `RemoteSpk` instance pointer. `getChan`, `getSpeakerVol`, `isPlayingSound`, `soundOff` and `soundOn` are inlined everywhere and absent from the executable. The linker drops the out-of-line copies the compiler still emits, so they do not shift the link.

Evidence: the WPAD and alarm callees take the arguments the reference passes at each call site, `OSSetAlarmUserData`, `OSGetAlarmUserData`, `OSCreateAlarm`, `OSSetAlarm` and `OSCancelAlarm` were already named in the alarm unit, and every function's control flow matches the CC0 Kirby's Epic Yarn reference (`Swiftshine/key` `7f2a9ab9`) except for the differences below. Compiler and flags are `homebuttonLib` per [F-287-1](287-hbm-core.md#f-287-1-hbm-compiler-flags-and-first-unit-boundaries).

Differences from the key reference that this target's code shows:

- No `sSetInfoAsync` static or check in `getInfoAsync`.
- `wpadConnectCallback` does not test `sThis` for null. On success it calls `WPADSetExtensionCallback` without storing the result. On disconnect it clears `mCallbackFlag`, `mCheckSoundTimeFlag` and `mCheckSoundIntervalFlag` but keeps the old extension callback. It chains to the old connect callback only when that callback is non-null and not `wpadConnectCallback`.
- `wpadExtensionCallback` mutes the speaker for 1000 ms on `WPAD_DEV_UNKNOWN`.
- The constructor and `initCallback` set `mRumbleFlag`; `startMotor` tests it.
- `initCallback` and `clearCallback` bracket `WPADSetConnectCallback` with `WPADSetCallbackByKPAD(FALSE)` and `WPADSetCallbackByKPAD(TRUE)`. `clearCallback` first enables the speaker.
- The destructor cancels both alarms and leaves `sThis` unchanged.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`, and objdiff reports `.text`, `.rodata` and `.bss` at 100%:

| Address | Size | Name |
| --- | --- | --- |
| `0x8017A010` | `0x110` | `wpadConnectCallback__Q210homebutton10ControllerFll` |
| `0x8017A120` | `0x10C` | `wpadExtensionCallback__Q210homebutton10ControllerFll` |
| `0x8017A230` | `0x6C` | `soundOnCallback__Q210homebutton10ControllerFP7OSAlarmP9OSContext` |
| `0x8017A2A0` | `0xBC` | `__ct__Q210homebutton10ControllerFiPQ210homebutton9RemoteSpk` |
| `0x8017A360` | `0x80` | `__dt__Q210homebutton10ControllerFv` |
| `0x8017A3E0` | `0xA0` | `initCallback__Q210homebutton10ControllerFv` |
| `0x8017A480` | `0x74` | `clearCallback__Q210homebutton10ControllerFv` |
| `0x8017A500` | `0x184` | `setKpad__Q210homebutton10ControllerFPC11HBMKPadDatab` |
| `0x8017A690` | `0x14` | `clrKpadButton__Q210homebutton10ControllerFv` |
| `0x8017A6B0` | `0x1C` | `setInValidPos__Q210homebutton10ControllerFv` |
| `0x8017A6D0` | `0x10` | `connect__Q210homebutton10ControllerFv` |
| `0x8017A6E0` | `0x4` | `disconnect__Q210homebutton10ControllerFv` |
| `0x8017A6F0` | `0x8` | `setSpeakerVol__Q210homebutton10ControllerFf` |
| `0x8017A700` | `0x9C` | `playSound__Q210homebutton10ControllerFi` |
| `0x8017A7A0` | `0x60` | `isPlayingSoundId__Q210homebutton10ControllerCFi` |
| `0x8017A800` | `0x10` | `initSound__Q210homebutton10ControllerFv` |
| `0x8017A810` | `0x238` | `updateSound__Q210homebutton10ControllerFv` |
| `0x8017AA50` | `0x10` | `isPlayReady__Q210homebutton10ControllerCFv` |
| `0x8017AA60` | `0x4` | `getController__Q210homebutton10ControllerFv` |
| `0x8017AA70` | `0x64` | `startMotor__Q210homebutton10ControllerFv` |
| `0x8017AAE0` | `0x30` | `stopMotor__Q210homebutton10ControllerFv` |
| `0x8017AB10` | `0x84` | `getInfoAsync__Q210homebutton10ControllerFP8WPADInfo` |
| `0x8017ABA0` | `0x24` | `ControllerCallback__Q210homebutton10ControllerFll` |
| `0x8017ABD0` | `0x24` | `getBatteryFlag__Q210homebutton10ControllerCFv` |
| `0x8017AC00` | `0x20` | `clrBatteryFlag__Q210homebutton10ControllerFv` |

The first 12 rows are this issue's; the last 13 are in #290's listed range.

Outside callees renamed for linking:

| Address | Name |
| --- | --- |
| `0x802F8CC0` | `WPADGetRadioSensitivity` |
| `0x802FA010` | `WPADProbe` |
| `0x802FA130` | `WPADSetConnectCallback` |
| `0x802FA1C0` | `WPADSetExtensionCallback` |
| `0x802FA400` | `WPADGetInfoAsync` |
| `0x802FA620` | `WPADControlMotor` |
| `0x802FAF10` | `WPADIsSpeakerEnabled` |
| `0x802FAF60` | `WPADControlSpeaker` |
| `0x802FE620` | `WPADSetCallbackByKPAD` |
| `0x8017B3A0` | `Connect__Q210homebutton9RemoteSpkFl` |
| `0x8017B430` | `Play__Q210homebutton9RemoteSpkFliSc` |
| `0x8017B4D0` | `isPlaying__Q210homebutton9RemoteSpkCFl` |
| `0x8017B4F0` | `isPlayingId__Q210homebutton9RemoteSpkCFli` |
| `0x8017B520` | `isPlayReady__Q210homebutton9RemoteSpkCFl` |

Layouts: `HBController` is 0x20 bytes (`chan`, `spVol`, `x`, `y`, `trig`, `hold`, `release`, `rumble`). `Controller` holds it at 0x0, then `remotespk` 0x20, `mOldConnectCallback` 0x24, `mOldExtensionCallback` 0x28, `mPlaySoundTime` 0x30, `mStopSoundTime` 0x38, and flags `mCallbackFlag`, `mSoundOffFlag`, `mCheckSoundTimeFlag`, `mCheckSoundIntervalFlag`, `mRumbleFlag` at 0x40–0x44. `RemoteSpk` (`include/revolution/hbm/homebutton/HBMRemoteSpk.h`, from CC0 xenoblade `ea25691b`) has four 0x68-byte channel records, its speaker alarm at 0x1A0, the sound handle at 0x1D0, `available` at 0x1EC and the vtable pointer at 0x1F0. Only the members that `HBMController.cpp` reaches are proven; the rest follow the reference until `HBMRemoteSpk.cpp` is matched.

## Traps

- The WPAD speaker constants and `WPADSetCallbackByKPAD` were missing from `include/revolution/wpad.h`, and `OSGetTime`, the alarm user-data accessors and the tick macros from `os.h`. They are now declared there; extend those headers instead of adding local declarations.
- A dtk-guessed `.bss` size can include alignment padding. `sBatteryFlag` was guessed at 8 bytes; the object is 4 bytes followed by padding for the 8-byte-aligned `OSAlarm` array.
