# Issue 759: HBMBase.cpp trigger event, reset, pane lookup, event handler and blackout

## F-759-1: startTrigEvent to update_sound

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). All eight functions of #759 lie inside the `RVL_SDK/hbm/HBMBase.cpp` split of [F-756-1](756-hbm-base.md#f-756-1-hbmbasecpp-unit-boundaries-and-the-first-22-functions). No split changed. The bodies were added to the existing source in address and key order, between the stripped inline functions that key places around them.

| Address | Size | Name |
| --- | --- | --- |
| `0x80183E50` | `0x16E8` | `startTrigEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` (was `fn_80183E50`) |
| `0x80185540` | `0x328` | `reset_btn__Q210homebutton10HomeButtonFv` |
| `0x80185870` | `0x11C` | `getPaneNo__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` |
| `0x80185990` | `0x1C` | `getSelectBtnNum__Q210homebutton10HomeButtonFv` |
| `0x801859B0` | `0x144` | `setAdjustFlag__Q210homebutton10HomeButtonFi` |
| `0x80185B00` | `0xB8` | `onEvent__Q210homebutton22HomeButtonEventHandlerFUlUlPv` (was `fn_80185B00`) |
| `0x80185BC0` | `0x1E8` | `startBlackOut__Q210homebutton10HomeButtonFv` |
| `0x80185DB0` | `0x50` | `update_sound__Q210homebutton10HomeButtonFv` |

Name evidence: `0x80185B00` is the `onEvent` slot (`0x803BB420`) of the `HomeButtonEventHandler` vtable at `.data` `0x803BB418`; it dispatches event 1 to `startPointEvent`, event 2 to `startLeftEvent`, and event 0 to `0x80183E50` when the A button bit (`0x800`) of the controller data is set, which is key's `startTrigEvent` ([F-757-1](757-hbm-base-pointer-events.md#f-757-1-update_controller-updatetrigpane-and-startpointevent)). The other six names were already in `symbols.txt`.

Reference: Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, CC0, `src/hbm/homebutton/HBMBase.cpp`, NAND build), the same reference as F-756-1 to F-758-1, with key's `mpLayout2`, `mpAnmControllerSet4`, `mpResFont2` and `at_0x27c[region][0]` as this project's `mpLayoutEx`, `mpAnmControllerEx`, `mpResFontEx` and `mpTextEx[region]`. Seven functions follow key without change. `startTrigEvent` differs in three places:

- The dialog branch for buttons 0 to 3 and case 10 call `SetFont` without saving and restoring the font size; key's NAND build wraps `SetFont` in `GetFontSize`/`SetFontSize`.
- Case 1 and 9 in `eSeq_Control` does not show `bar_00`, the same difference F-757-1 found in `update_controller`.
- No assertions are compiled in.

The dialog branch uses `mpResFont`, case 10 `mpResFontEx`. The jump table for the `btn_no - mButtonNum` switch is at `.data` `0x803BB3D4`, 12 entries.

The sound constants `scSoundHeapSize_but2` (`0x60000`), `scSoundHeapSize_but3` (`0x6F800`), `scSoundThreadPrio` (4) and `scDvdThreadPrio` (3) were defined before `update_sound`, in key's position. They are emitted at `.rodata` offset `0x360` of the unit, `0x80366E00`, matching the target bytes.

With the unit registered `NonMatching`, objdiff reports all eight functions at 100%. That makes 34 of the unit's 37 functions and 39,096 of its 40,428 code bytes. The other 26 stay at 100%. The full build passes `tools/verify/verify.py verify`. The unit stays `NonMatching` until #760 adds `createInfoEx` (`0x80185E00`), `destroyInfoEx` and `set_text_ex`.

## Traps

No new trap. Evidence: F-759-1.
