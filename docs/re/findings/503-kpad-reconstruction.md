# KPAD motion and pointing reconstruction

## F-503-5: Whole-unit split and partial source

KPAD.c owns 33 retained functions and 16,304 function bytes. Its first twenty functions are the KPAD portion of #503; thirteen later functions belong to #504. The first function belongs to #503, so this branch establishes the complete translation-unit split. Twenty-four functions are now implemented. Neither issue is complete in this partial reconstruction.

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x80304650` | `0x803086C8` |
| `.data` | `0x803DE828` | `0x803DE888` |
| `.bss` | `0x8047ACD0` | `0x8047C320` |
| `.sdata` | `0x804B8DF8` | `0x804B8E78` |
| `.sbss` | `0x804B9AD0` | `0x804B9B08` |
| `.sdata2` | `0x804BB568` | `0x804BB5D8` |

The first data object is the June 2009 version banner, followed by the sample-format table. BSS contains four channel states, the freestyle rotation matrix, and two balance-board weight arrays. Initialized small data contains the version pointer, motion parameters, clamp ranges, and board averaging parameters. The text split ends at the sampling callback's final instruction, before alignment and the separate routine at `0x803086D0`.

The split-only build and frozen game-DOL verification pass. Source is registered as `NonMatching`; the final executable continues to use the original KPAD object. This dependency is deliberate and contributes no linked source coverage.

## Reconstructed portion

The source implements the first twenty retained functions and four later callbacks/control routines, covering 8,796 function bytes. Their names follow the CC0 Petari implementation, reference headers and the corresponding target behavior. Four additional private object helpers, plus acceleration, trigger, and horizon helpers, inline into retained functions.

All twenty-four implemented functions, totaling 8,796 function bytes, reproduce every non-relocation instruction word at the exact original size with Wii/1.0 and `cflags_sdk`: the four parameter setters, reset, aiming enable, button repeat and reader, acceleration calculation, acceleration horizon and vertical calculation, acceleration reader, four object-selection routines, DPD-variable calculation, DPD reader, both stick clamps, application connection-callback registration, both DPD request setters and DPD control completion. This is a partial diagnostic: relocation words, whole sections, missing later functions, and data reconstruction are not accepted by that comparison. Whole-text objdiff is 53.868988%; data remains 0%, BSS 98.039215%, sdata 43.90244%, sbss 30.76923% and sdata2 55.813957%.

The target adds train/guitar handling to button repeat and accepts the guitar acceleration format. Its DPD reader accepts the guitar format only when actual DPD hardware state is enabled. Those version differences, together with [the recovered layout](503-kpad-layout.md), reproduce the nineteen instruction streams without changing compiler policy.

`read_kpad_button` now matches the raw instruction diagnostic after computing the extension-reset-gated classic button word before loading the old held buttons. Its source preserves extension-reset gating and train/guitar handling. The remaining nine retained functions are not implemented: extension-stick/board processing, read wrapper and extended reader, initialization wrapper and extended initializer, connection handling, two board callbacks, and sampling. Their original fallback remains visible in the same unit.

The four newly reconstructed routines are `KPADSetConnectCallback` at `0x80307DE0` (0x58 bytes), `KPADDisableDPD` at `0x80307E80` (0x1C), `KPADEnableDPD` at `0x80307EA0` (0x1C), and `KPADiControlDpdCallback` at `0x80307EC0` (0x94). The connection setter disables interrupts while replacing and returning the application callback. The DPD setters change requested state at 0x556. Completion updates actual hardware state at 0x557 using `WPADIsDpdEnabled`, then clears the issued flag at 0x558; the older Petari completion lacks that hardware-state update. Public declarations are additive and preserve the established shared layout.

## Remaining acceptance work

- Complete the nine remaining functions, coordinating with #504 under the established KPAD.c ownership.
- Resolve all relocation/data comparisons.
- Reconstruct the retained banner/table, complete initialized data, board globals, and constant pool; current source includes only the reference globals needed for the motion portion.
- Preserve the whole-unit `NonMatching` status until every retained section compares exactly and the source-selected full DOL passes frozen verification.

## Provenance

The motion and pointing algorithms and helper names derive from [Petari KPAD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/kpad/KPAD.c), CC0. Comments are removed, SDK C headers replace the reference's C++ convenience headers, and only the reconstructed portion is retained. Newer-format conditions come from the target instructions. The shared layout's separate provenance is recorded in F-503-4. A copied IDA database outside the repository was used only as a reading aid; no decompiler output is included in source.

## Traps

- **Preserve the inline helper implementations when extracting a partial source.** Omitting `clamp_acc`, `clamp_trigger`, or `calc_horizon` let the C compiler infer external calls and substantially shortened callers. Restoring the actual helpers recovers the original instruction streams; changing compiler flags would have hidden the cause.
- **Raw instruction comparisons exclude relocation evidence.** The nineteen diagnostic matches do not justify `Matching`; the unit's current objdiff score and explicit fallback remain the acceptance record.
- **Expression order affects the button-reader registers.** Loading old classic buttons before forming the reset-gated new word produced nine differing instruction words. An explicit new-button temporary computed first reproduces the original registers without changing flags or inserting assembly. This is a local source-order finding, not a compiler-wide rule.
