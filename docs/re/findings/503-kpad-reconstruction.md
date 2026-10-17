# KPAD motion and pointing reconstruction

## F-503-5: Whole-unit split and partial source

KPAD.c owns 33 retained functions and 16,304 function bytes. Its first twenty functions are the KPAD portion of #503; thirteen later functions belong to #504. The first function belongs to #503, so this branch establishes the complete translation-unit split. Neither issue is complete in this partial reconstruction.

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

The source implements the first twenty retained functions, covering 8,504 function bytes. Their names follow the CC0 Petari implementation and the corresponding target behavior. Four additional private object helpers, plus acceleration, trigger, and horizon helpers, inline into retained functions.

Nineteen functions, totaling 8,344 function bytes, reproduce every non-relocation instruction word at the exact original size with Wii/1.0 and `cflags_sdk`: the four parameter setters, reset, aiming enable, button repeat, acceleration calculation, acceleration horizon and vertical calculation, acceleration reader, four object-selection routines, DPD-variable calculation, DPD reader, and both stick clamps. This is a partial diagnostic: relocation words, whole sections, missing later functions, and data reconstruction are not accepted by that comparison.

The target adds train/guitar handling to button repeat and accepts the guitar acceleration format. Its DPD reader accepts the guitar format only when actual DPD hardware state is enabled. Those version differences, together with [the recovered layout](503-kpad-layout.md), reproduce the nineteen instruction streams without changing compiler policy.

`read_kpad_button` has the original size and behavior structure but still differs in register allocation. Its source preserves extension-reset gating and train/guitar handling. The remaining thirteen retained functions are not implemented: extension-stick/board processing, read wrappers and extended reader, initialization, connection handling, application connection-callback registration, two board callbacks, two DPD request setters, DPD control completion, and sampling. Their original fallback remains visible in the same unit.

## Remaining acceptance work

- Complete all thirteen later functions, coordinating with #504 under the established KPAD.c ownership.
- Resolve button-reader register allocation and all relocation/data comparisons.
- Reconstruct the retained banner/table, complete initialized data, board globals, and constant pool; current source includes only the reference globals needed for the motion portion.
- Preserve the whole-unit `NonMatching` status until every retained section compares exactly and the source-selected full DOL passes frozen verification.

## Provenance

The motion and pointing algorithms and helper names derive from [Petari KPAD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/kpad/KPAD.c), CC0. Comments are removed, SDK C headers replace the reference's C++ convenience headers, and only the reconstructed portion is retained. Newer-format conditions come from the target instructions. The shared layout's separate provenance is recorded in F-503-4. A copied IDA database outside the repository was used only as a reading aid; no decompiler output is included in source.

## Traps

- **Preserve the inline helper implementations when extracting a partial source.** Omitting `clamp_acc`, `clamp_trigger`, or `calc_horizon` let the C compiler infer external calls and substantially shortened callers. Restoring the actual helpers recovers the original instruction streams; changing compiler flags would have hidden the cause.
- **Raw instruction comparisons exclude relocation evidence.** The nineteen diagnostic matches do not justify `Matching`; the unit's current objdiff score and explicit fallback remain the acceptance record.
