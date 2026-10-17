# Issue 757: HBMBase.cpp controller update, trigger panes and pointer events

## F-757-1: update_controller, updateTrigPane and startPointEvent

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The three functions of #757 lie inside the `RVL_SDK/hbm/HBMBase.cpp` split of [F-756-1](756-hbm-base.md#f-756-1-hbmbasecpp-unit-boundaries-and-the-first-22-functions). No split changed. Their bodies were added to the existing source in key's order: `update_controller` after `update`, then the already present inline `update_posController`, then `updateTrigPane` and `startPointEvent`.

| Address | Size | Name |
| --- | --- | --- |
| `0x80181380` | `0x984` | `update_controller__Q210homebutton10HomeButtonFi` |
| `0x80181D10` | `0x484` | `updateTrigPane__Q210homebutton10HomeButtonFv` |
| `0x801821A0` | `0x1084` | `startPointEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4PanePv` (was `fn_801821A0`) |

Name evidence for `startPointEvent`: `HomeButtonEventHandler::onEvent` (`0x80185B00`, still `fn_80185B00`) calls `0x801821A0` with the pane and the event data for event 1, `0x80183230` for event 2 and `0x80183E50` for event 0 when the A button bit is set. That matches key's `startPointEvent`, `startLeftEvent` and `startTrigEvent`. The three sizes also follow key's body lengths.

`startPointEvent` calls `0x80185870` first and compares the result with -1 and `mButtonNum`. That is key's `getPaneNo`, so `fn_80185870` is renamed `getPaneNo__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` in `symbols.txt` for the relocation comparison. Its body and split stay with #759.

The NAND build uses two more sequences, so `HomeButton::eSeq` gains `eSeq_Seq3` (3) and `eSeq_Seq4` (4), named after key. `update_controller` tests `mSequence == 3`. `updateTrigPane` has five cases, and `startPointEvent` tests 3 and 4 in its `btn_no - mButtonNum` cases 7, 8, 10 and 11.

This target differs from key in one place. On the HOME button in `eSeq_Control`, `update_controller` does not show `bar_00`. It moves the pointer to (0, -180), starts pair group animations 4, 2 and 11, sets state 10 and plays `HBMSE_CLOSE_CONTROLLER`. Everything else in the three functions follows key, with key's `mpLayout2` and `mpAnmControllerSet4` as this project's `mpLayoutEx` and `mpAnmControllerEx`. The jump table for `startPointEvent`'s switch is at `.data` `0x803BB374`, 12 entries.

`setForcusSE` has no out-of-line copy. It is inlined into `startPointEvent` and stripped. Its body is the inline `mForcusSEWaitTime` check followed by `play_sound(HBMSE_FOCUS)`. It was added after `findGroupAnimator`, among the other stripped functions, in key's order.

With the unit registered `NonMatching`, objdiff reports all three functions at 100%. That makes 25 of the unit's 37 functions and 27,944 of its 40,428 code bytes. The other 22 stay at 100%. The full build passes `tools/verify/verify.py verify`. The unit stays `NonMatching` until #758, #759 and #760 add `startLeftEvent` (`0x80183230`) through `set_text_ex`.

## Traps

No new trap. The float pool tail (`608.0f`, `456.0f`, `-180.0f`, `356.4f`, `70.4f`, `3.0f` at `.rodata` `0x80366DE8`–`0x80366DFC`) was emitted in target order once the functions were added in address order. Evidence: F-757-1.
