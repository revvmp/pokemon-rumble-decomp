# Issue 758: HBMBase.cpp pointer leave event

## F-758-1: startLeftEvent

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The only function of #758 lies inside the `RVL_SDK/hbm/HBMBase.cpp` split of [F-756-1](756-hbm-base.md#f-756-1-hbmbasecpp-unit-boundaries-and-the-first-22-functions). No split changed. Its body was added to the existing source directly after `startPointEvent`, in address and key order.

| Address | Size | Name |
| --- | --- | --- |
| `0x80183230` | `0xC14` | `startLeftEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` (was `fn_80183230`) |

Name evidence: `HomeButtonEventHandler::onEvent` (`0x80185B00`) calls `0x80183230` with the pane for event 2, which is key's `startLeftEvent` ([F-757-1](757-hbm-base-pointer-events.md#f-757-1-update_controller-updatetrigpane-and-startpointevent)). The issue's range end `0x80183E44` is the end of this function; the four bytes before `0x80183E50` are alignment padding.

The function follows key's NAND build without change, with key's `mpLayout2` and `mpAnmControllerSet4` as this project's `mpLayoutEx` and `mpAnmControllerEx`. It decrements the pane counters, then starts the leave animations: animator 2 for the buttons, pair group 12 for the upper bar, groups (5, 3) and (5, 20) for the lower bar, (6..10, 7) for the controller panes, (17, 12) and (18, 12) for the window buttons, and extra controllers 1 and 4 for the third layout, resizing `scBtnWareName[0]` to 324 by 64. The jump table for its `btn_no - mButtonNum` switch is at `.data` `0x803BB3A4`, 12 entries.

With the unit registered `NonMatching`, objdiff reports `startLeftEvent` at 100%. That makes 26 of the unit's 37 functions and 31,036 of its 40,428 code bytes. The other 25 stay at 100%. The full build passes `tools/verify/verify.py verify`. The unit stays `NonMatching` until #759 and #760 add `startTrigEvent` (`0x80183E50`) through `set_text_ex`.

## Traps

No new trap. Evidence: F-758-1.
