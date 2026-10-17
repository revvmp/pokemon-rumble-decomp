# Issue 756: HBM homebutton core, HBMBase.cpp

## F-756-1: HBMBase.cpp unit boundaries and the first 22 functions

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). `RVL_SDK/hbm/HBMBase.cpp` starts with `initgx` at `0x8017C470`, after the padding that ends `HBMCommon.cpp` ([F-291-3](291-hbm-axsound.md#f-291-3-hbmcommoncpp-and-the-homebutton-layout)), and ends with `set_text_ex` at `0x801862B0`. The unit owns these ranges:

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8017C470`–`0x80186334` | 37 functions, including the weak `ArcResourceAccessor` destructor |
| `.rodata` | `0x80366AA0`–`0x80366E38` | a black `GXColor`, `scAnmTable`, `scGroupAnmTable`, the `HomeButton::sc*` integer and float constants, the float pool, the sound constants and `createInfoEx`'s `"arc_"` buffer initializer |
| `.data` | `0x803BA6A0`–`0x803BB4E0` | the string tables and their strings, the strings used by the code, four switch jump tables, and the `HomeButtonEventHandler`, `gui::EventHandler` and `gui::Interface` vtables and RTTI |
| `.bss` | `0x803E9BA0`–`0x803E9BC0` | `spHomeButtonObj` (`0x803E9BA0`), `sMutex` (`0x803E9BA8`, 0x18 bytes) |

`.text` ends at the end of `set_text_ex`. The twelve bytes after it are padding before `lyt_animation.cpp` at `0x80186340`. `.rodata` and `.data` end where `lyt_animation.cpp` begins. The four bytes at `0x803E9BA4` are alignment padding before `sMutex`.

The remaining 15 functions, `update_controller` (`0x80181380`) to `set_text_ex`, belong to #757, #758, #759 and #760. They are not reconstructed yet, so the unit stays `NonMatching`.

Reference: CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/homebutton/HBMBase.cpp`), the NAND build of the library. Its function order and member offsets match after the `mWpadInfo` change of F-291-3. This target differs from key as follows:

- `create` scales only the root child `back_01`, by `1.2f`, and stops at the first match. It does not scale `bar_line_00` or `bar_line_10`.
- `init` does not touch `bar_00`, `bar_10`, or the alpha of the second layout's `bar_00`.
- `draw` does not set fog, texture coordinate scale or the black bands. It draws the second layout, the main layout, the cursors from 3 to 0, then the fader.
- In `calc`, state 16 neither clears `mBatteryCheck` nor fades `bar_00`. State 17 stops the remote speaker and motors and clears the callbacks. State 21 hides only `N_cntrl_01`. `update` runs whenever controller data is passed.
- The battery level is read from `mWpadInfo[chan]` (`0x1B4`, stride 0x18), not from a static array.

Out-of-line functions in this range, with names from key, xenoblade (`xbret/xenoblade` `ea25691b`, CC0) and the reference symbol maps:

| Address | Size | Name |
| --- | --- | --- |
| `0x8017C470` | `0x1B8` | `initgx__10homebuttonFv` |
| `0x8017C630` | `0x110` | `__ct__Q210homebutton10HomeButtonFPC11HBMDataInfo` |
| `0x8017C740` | `0x2D8` | `__dt__Q210homebutton10HomeButtonFv` |
| `0x8017CA20` | `0x6C` | `__dt__Q36nw4hbm3lyt19ArcResourceAccessorFv` (weak) |
| `0x8017CA90` | `0x4C` | `createInstance__Q210homebutton10HomeButtonFPC11HBMDataInfo` |
| `0x8017CAE0` | `0x44` | `deleteInstance__Q210homebutton10HomeButtonFv` |
| `0x8017CB30` | `0xC` | `getInstance__Q210homebutton10HomeButtonFv` |
| `0x8017CB40` | `0x8B0` | `create__Q210homebutton10HomeButtonFv` |
| `0x8017D3F0` | `0x170` | `set_config__Q210homebutton10HomeButtonFv` |
| `0x8017D560` | `0x84` | `set_text__Q210homebutton10HomeButtonFv` |
| `0x8017D5F0` | `0x540` | `init__Q210homebutton10HomeButtonFv` |
| `0x8017DB30` | `0x200` | `init_volume__Q210homebutton10HomeButtonFv` |
| `0x8017DD30` | `0x130` | `init_sound__Q210homebutton10HomeButtonFv` |
| `0x8017DE60` | `0x26C` | `init_battery__Q210homebutton10HomeButtonFPC17HBMControllerData` |
| `0x8017E0D0` | `0x23A0` | `calc__Q210homebutton10HomeButtonFPC17HBMControllerData` |
| `0x80180470` | `0x204` | `calc_battery__Q210homebutton10HomeButtonFi` |
| `0x80180680` | `0x170` | `draw__Q210homebutton10HomeButtonFv` |
| `0x801807F0` | `0xE4` | `SpeakerCallback__10homebuttonFP7OSAlarmP9OSContext` |
| `0x801808E0` | `0x24` | `MotorCallback__10homebuttonFP7OSAlarmP9OSContext` |
| `0x80180910` | `0xCC` | `RetrySimpleSyncCallback__10homebuttonFP7OSAlarmP9OSContext` |
| `0x801809E0` | `0x38` | `SimpleSyncCallback__10homebuttonFll` |
| `0x80180A20` | `0x954` | `update__Q210homebutton10HomeButtonFPC17HBMControllerData` |

These functions have no out-of-line copy in the target. They are inlined into every caller, and the linker strips the unreferenced copies: `init_msg` (into `create`), `init_vib`, `calc_fadeoutAnm`, `isActive`, `isUpBarActive`, `isDownBarActive`, `isThirdBarActive`, `findAnimator`, `findGroupAnimator`, `reset_control`, `reset_window`, `reset_battery`, `reset_guiManager`, `update_posController`, `setSpeakerAlarm`, `setSimpleSyncAlarm`, `callSimpleSyncCallback`, `setVolume`, `getVolume`, `setVibFlag`, `getVibFlag`, `play_sound`, `fadeout_sound`, and `BlackFader`'s `init`, `calc`, `isDone` and `draw`. The `HomeButtonEventHandler` and `PaneManager` constructors are inlined into `create`.

Outside callees named for comparison, with evidence from call sites and key's order: `update_controller` (`0x80181380`, called per controller from `update`), `updateTrigPane` (`0x80181D10`, called after the trigger targets are set in `init`), `reset_btn` (`0x80185540`, called after the inlined `reset_control` in `calc`) and `destroyInfoEx` (`0x80186190`, frees the second layout, its accessor, links, font and seven controllers).

`HBMDataInfoEx` is now defined from the accesses in `calc` state 20, `createInfoEx` and `set_text_ex`: `layoutBuf` `0x0`, `msgBuf` `0x4`, `texImage` `0x8`, `texImageWidth` `0x10`, `texImageHeight` `0x12` and `texImageFormat` `0x14`. Names follow key. Nothing in the unit reads the word at `0xC`, so it is declared `reserved`.

The `.rodata` pool starts with a black `GXColor` (`0x80366AA0`) that no linked function loads. Key and xenoblade reproduce it with a placeholder function. Here it comes from `setBlackPlateColor`, a global function before the tables that sets TEV register 0 to that colour. The linker strips the function, and its body cannot be verified. The tables are 8-byte aligned, which gives the four zero bytes at `0x80366AA4`.

Compiler and flags: Wii/1.0a with `cflags_hbm` (F-287-1). With the unit registered `NonMatching`, objdiff 3.8.2 reports all 22 functions above at 100%, 20,124 of the unit's 40,428 code bytes, and `.bss` at 100%. The split-only build and the build with the source object both pass `tools/verify/verify.py verify`.

## Traps

- **The rodata tables are 8-byte aligned.** Trigger: every float offset in the rebuilt `.rodata` was 8 bytes short of the target. The pool starts with a 4-byte `GXColor`, and the `AnmControllerTable` arrays that follow are 8-byte aligned. Emitting that colour from a global function defined before the tables restores the layout. Evidence: F-756-1.
- **NAND HBM versions differ in more than layout.** Trigger: key's `create`, `init`, `draw` and `calc` set panes, fog and alpha that this target never touches. Compare each state of `calc` with the target before reusing key's code. Evidence: F-756-1.
