# KPAD motion and pointing reconstruction

## F-503-5: Whole-unit split and partial source

KPAD.c owns 33 retained functions and 16,304 function bytes. Its first twenty functions are the KPAD portion of #503; thirteen later functions belong to #504. The first function belongs to #503, so this branch establishes the complete translation-unit split. Twenty-nine functions are now implemented. Neither issue is complete in this partial reconstruction.

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

The source implements the first twenty retained functions and nine later initialization/callback/control routines, covering 10,304 function bytes. Their names follow the CC0 Petari implementation, reference headers and the corresponding target behavior. Four additional private object helpers, plus acceleration, trigger, and horizon helpers, inline into retained functions.

All twenty-nine implemented functions, totaling 10,304 function bytes, reproduce every non-relocation instruction word at the exact original size with Wii/1.0 and `cflags_sdk`: the four parameter setters, reset, aiming enable, button repeat and reader, acceleration calculation, acceleration horizon and vertical calculation, acceleration reader, four object-selection routines, DPD-variable calculation, DPD reader, both stick clamps, both initialization routines, connection handling, application connection-callback registration, two board completion callbacks, both DPD request setters and DPD control completion. This is a partial diagnostic: relocation words, whole sections, missing later functions, and data reconstruction are not accepted by that comparison. Whole-text objdiff is 63.161186%; data is 85.029945%, BSS 100%, sdata 81.48148%, sbss 61.53846% and sdata2 60.784317%.

The target adds train/guitar handling to button repeat and accepts the guitar acceleration format. Its DPD reader accepts the guitar format only when actual DPD hardware state is enabled. Those version differences, together with [the recovered layout](503-kpad-layout.md), reproduce the nineteen instruction streams without changing compiler policy.

`read_kpad_button` now matches the raw instruction diagnostic after computing the extension-reset-gated classic button word before loading the old held buttons. Its source preserves extension-reset gating and train/guitar handling. The remaining four retained functions are not implemented: extension-stick/board processing, read wrapper and extended reader, and sampling. Their original fallback remains visible in the same unit.

The four newly reconstructed routines are `KPADSetConnectCallback` at `0x80307DE0` (0x58 bytes), `KPADDisableDPD` at `0x80307E80` (0x1C), `KPADEnableDPD` at `0x80307EA0` (0x1C), and `KPADiControlDpdCallback` at `0x80307EC0` (0x94). The connection setter disables interrupts while replacing and returning the application callback. The DPD setters change requested state at 0x556. Completion updates actual hardware state at 0x557 using `WPADIsDpdEnabled`, then clears the issued flag at 0x558; the older Petari completion lacks that hardware-state update. Public declarations are additive and preserve the established shared layout.

The next three routines are `KPADiConnectCallback` at `0x80307C70` (0x164 bytes), `KPADiBalanceControlCallback` at `0x80307E40` (0x18), and `KPADiBalanceCalibrationCallback` at `0x80307E60` (0x18). These private names are descriptive, medium confidence. Connection preserves the application callback, installs the sampling callback under `WPADSetCallbackByKPAD`, resets DPD hardware state and clears all inline/external samples on disconnect. Channel three also resets board state and both four-double arrays. The board callbacks record the specific command result and clear the pending flag. Their identical instruction shapes are distinguished by the target SDA references, not by raw instruction bytes alone. `KPADiSamplingCallback` at `0x80307F60` is named for the proven connection reference but remains unimplemented.

`KPADInit` at `0x80307810` is a 12-byte wrapper calling `KPADInitEx(NULL, 0)`. The 0x444-byte extended initializer at `0x80307820` runs once, saves application callbacks, partitions an optional unified-sample buffer equally among four channels, initializes all ring errors and channel parameters, resets controllers and constructs the freestyle rotation matrix. Reference-derived `KPADSetBtnRepeat`, `KPADSetObjInterval` and `KPADReset` bodies are included because initialization inlines them; they are not additional retained target functions or byte credit. `calc_dpd2pos_scale` is an inlined private reference helper. The complete implementation is still required to prove that all unretained helpers are stripped in the final source link.

The one-byte initialized flag is named `sKpadInitialized` at `0x804B9AE4`. `buttonProcessingMode` replaces the misleading member name `padding` at `KPADInsideStatus + 0x55F`: initialization clears it, and the reader selects per-sample versus aggregate button processing from it. This is a name change only; all offsets and sizes are preserved. A comparison with the saved baseline confirms all 321 pre-existing source objects still have identical code, data and relocation records. The initializer also proves the original eight-byte vector objects `iaccXY_nrm_hori` (`0x804B8E00`), `isec_nrm_hori` (`0x804B8E08`) and `icenter_org` (`0x804B9AD0`); their former component aliases are merged within the owned unit.

## Remaining acceptance work

- Complete the four remaining functions, coordinating with #504 under the established KPAD.c ownership.
- Resolve all relocation/data comparisons.
- Reconstruct the retained banner/table, complete initialized data, remaining board state and constant pool. Both 32-byte board arrays now occupy the proven BSS offsets, reproducing that entire section.
- Preserve the whole-unit `NonMatching` status until every retained section compares exactly and the source-selected full DOL passes frozen verification.

### Extension-reader investigation

The uncommitted scratch investigation of `read_kpad_stick` at `0x80306800` establishes additional train, guitar and balance-board paths. Train inputs use the classic status's trigger outputs; guitar inputs also use its two stick vectors. Board output calls `WBCGetBatteryLevel`, `WBCRead` and `WBCGetTGCWeight`, maintaining four-double arrays at `0x8047C2E0` and `0x8047C300`. The smoothing window at `0x804B8E70` is the float 400.0, and the rejection threshold in the constant pool at `0x804BB5A8` is the double -0.5. A 16-bit sample count is at `0x804B9AF8`, calibration state at `0x804B9AFA`, TGC averaging flag at `0x804B9AFB`, and the double TGC result at `0x804B9AF0`. These roles follow accesses and calls; original private variable names remain unrecovered.

The complete scratch candidate still differs in the averaging instruction schedule and address materialization. One source form produced 0x8C4 bytes against the target's 0x8D0; other forms added floating-register saves. Wii/1.0a, 1.1, 1.3 and 0x4201_127 did not resolve that candidate's differences, nor did removing file-level IPA or explicitly disabling contraction. This is evidence about those candidates only. No compiler policy or accepted source coverage is changed by these experiments. The reader remains among the four unfinished functions.

### Sampling investigation

The sampling callback's format table at `0x803DE870` contains twelve `{DPD command, data format}` pairs, two each for core, freestyle, classic, guitar, board and train. Guitar uses format 11 with either DPD state; board uses format 12 and train format 10 with DPD off in both entries. Sampling extends the inline sixteen-entry ring through the optional external ring, updates gravity scales when requested, and invokes the application sampling callback even after a failed probe.

Board setup starts command 170, starts and polls WBC calibration, sends command zero, waits through the 50-sample settling interval (`u8` at `0x804B8E6C`), then averages raw pressure samples before calling `WBCSetZEROPoint` after the counter exceeds the float 400.0 window. The target marks commands pending only inside a successful-result branch; writing a boolean comparison directly produces a different instruction sequence. Gravity fallback components are separate ascending stores, not chained assignments.

A scratch candidate has the target's complete 0x768-byte size. Its best examined form reproduces 393 of 405 non-relocation instruction words; the remaining twelve differ in the integer registers used for the pressure and prior-count values during averaging. That candidate is not incorporated or counted among the 29 implemented functions. Ring selection, gravity, aiming, format selection, DPD controls and board state transitions reproduce the target instruction sequence, but that is not a whole-function acceptance claim. Scalar temporary forms, pointer/value inline helpers and declaration-order variants are preserved outside the repository as investigation aids; they do not justify changing compiler policy or marking the unit `Matching`.

## Provenance

The motion and pointing algorithms and helper names derive from [Petari KPAD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/kpad/KPAD.c), CC0. Comments are removed, SDK C headers replace the reference's C++ convenience headers, and only the reconstructed portion is retained. Newer-format conditions come from the target instructions. The shared layout's separate provenance is recorded in F-503-4. A copied IDA database outside the repository was used only as a reading aid; no decompiler output is included in source.

## Traps

- **Preserve the inline helper implementations when extracting a partial source.** Omitting `clamp_acc`, `clamp_trigger`, or `calc_horizon` let the C compiler infer external calls and substantially shortened callers. Restoring the actual helpers recovers the original instruction streams; changing compiler flags would have hidden the cause.
- **Raw instruction comparisons exclude relocation evidence.** The nineteen diagnostic matches do not justify `Matching`; the unit's current objdiff score and explicit fallback remain the acceptance record.
- **Expression order affects the button-reader registers.** Loading old classic buttons before forming the reset-gated new word produced nine differing instruction words. An explicit new-button temporary computed first reproduces the original registers without changing flags or inserting assembly. This is a local source-order finding, not a compiler-wide rule.
- **Chained board-array resets change folded BSS accesses.** The connection callback initially had the correct size but referenced the two board arrays in the wrong order. Separate, ordered zero assignments reproduce the original stores and folded offsets `0x1610`/`0x1630` from the channel-state base; adding the two arrays also makes BSS 100%. Do not dismiss non-relocated folded offsets as linker work.
- **Equivalent initializer loops can schedule differently.** A `for` loop produced the right 0x444-byte initializer with one differing non-relocation word: a constant load swapped with a relocated callback-address load. The reference's `do`/`while` form reproduces every word. Check both sides of relocated words rather than masking an adjacent scheduling difference.
