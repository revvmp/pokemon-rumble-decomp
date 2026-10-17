# KPAD source reconstruction

## F-503-5: Whole-unit split and source

KPAD.c owns 33 retained functions and 16,304 function bytes. Its first twenty functions are the KPAD portion of #503; thirteen later functions belong to #504. The first function belongs to #503, so this branch establishes the complete translation-unit split. All 33 functions are implemented and selected from source. Together with the already merged WPADMem.c and debug_msg.c work, this completes the source scope of #503. The thirteen KPAD functions in #504 are covered, but its later libraries remain outside this pull request.

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x80304650` | `0x803086C8` |
| `.data` | `0x803DE828` | `0x803DE888` |
| `.bss` | `0x8047ACD0` | `0x8047C320` |
| `.sdata` | `0x804B8DF8` | `0x804B8E78` |
| `.sbss` | `0x804B9AD0` | `0x804B9B08` |
| `.sdata2` | `0x804BB568` | `0x804BB5D8` |

The first data object is the June 2009 version banner, followed by the sample-format table. BSS contains four channel states, the freestyle rotation matrix, and two balance-board weight arrays. Initialized small data contains the version pointer, motion parameters, clamp ranges, and board averaging parameters. The text split ends at the sampling callback's final instruction, before alignment and the separate routine at `0x803086D0`.

The split-only build passed before source registration. The complete source-linked build now also passes frozen game-DOL verification. Source is registered as `Matching`; no original KPAD object is linked. Other unreconstructed units elsewhere in the executable retain their explicit fallbacks.

## Reconstructed source

All 33 retained functions, totaling 16,304 function bytes, are implemented with Wii/1.0 and `cflags_sdk`. The standard Ninja report records 100% fuzzy code matching, 16,304/16,304 matched code bytes and 33/33 matched functions. Raw comparison also reproduces every non-relocation instruction word at the exact original sizes. Direct comparison of all 33 linked function ranges includes resolved relocations and reproduces every original byte.

Text, data, BSS, sdata and sbss each score 100% in objdiff. The compiled object's sdata2 diagnostic is 91.52542% because it also contains a stripped helper's 200.0f literal and three unused local constants; it is not a final artifact mismatch. The linker removes the unretained helpers and constants. The full source-linked DOL passes the frozen manifest with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This establishes only the main game executable acceptance artifact, excluding boot loader, data contents and package reconstruction.

The target adds train/guitar handling to button repeat and accepts the guitar acceleration format. Its DPD reader accepts the guitar format only when actual DPD hardware state is enabled. Those version differences, together with [the recovered layout](503-kpad-layout.md), reproduce the nineteen instruction streams without changing compiler policy.

`read_kpad_button` now matches the raw instruction diagnostic after computing the extension-reset-gated classic button word before loading the old held buttons. Its source preserves extension-reset gating and train/guitar handling. The four reader/sampling functions are completed in F-503-6 below.

The four newly reconstructed routines are `KPADSetConnectCallback` at `0x80307DE0` (0x58 bytes), `KPADDisableDPD` at `0x80307E80` (0x1C), `KPADEnableDPD` at `0x80307EA0` (0x1C), and `KPADiControlDpdCallback` at `0x80307EC0` (0x94). The connection setter disables interrupts while replacing and returning the application callback. The DPD setters change requested state at 0x556. Completion updates actual hardware state at 0x557 using `WPADIsDpdEnabled`, then clears the issued flag at 0x558; the older Petari completion lacks that hardware-state update. Public declarations are additive and preserve the established shared layout.

The next three routines are `KPADiConnectCallback` at `0x80307C70` (0x164 bytes), `KPADiBalanceControlCallback` at `0x80307E40` (0x18), and `KPADiBalanceCalibrationCallback` at `0x80307E60` (0x18). These private names are descriptive, medium confidence. Connection preserves the application callback, installs the sampling callback under `WPADSetCallbackByKPAD`, resets DPD hardware state and clears all inline/external samples on disconnect. Channel three also resets board state and both four-double arrays. The board callbacks record the specific command result and clear the pending flag. Their identical instruction shapes are distinguished by the target SDA references, not by raw instruction bytes alone. `KPADiSamplingCallback` at `0x80307F60` is supported by the proven connection reference and completed below.

`KPADInit` at `0x80307810` is a 12-byte wrapper calling `KPADInitEx(NULL, 0)`. The 0x444-byte extended initializer at `0x80307820` runs once, saves application callbacks, partitions an optional unified-sample buffer equally among four channels, initializes all ring errors and channel parameters, resets controllers and constructs the freestyle rotation matrix. Reference-derived `KPADSetBtnRepeat`, `KPADSetObjInterval` and `KPADReset` bodies are included because initialization inlines them; they are not additional retained target functions or byte credit. `calc_dpd2pos_scale` is an inlined private reference helper. The final linked ELF contains none of those unretained helper bodies.

The one-byte initialized flag is named `sKpadInitialized` at `0x804B9AE4`. `buttonProcessingMode` replaces the misleading member name `padding` at `KPADInsideStatus + 0x55F`: initialization clears it, and the reader selects per-sample versus aggregate button processing from it. This is a name change only; all offsets and sizes are preserved. A comparison with the saved baseline confirms all 321 pre-existing source objects still have identical code, data and relocation records. The initializer also proves the original eight-byte vector objects `iaccXY_nrm_hori` (`0x804B8E00`), `isec_nrm_hori` (`0x804B8E08`) and `icenter_org` (`0x804B9AD0`); their former component aliases are merged within the owned unit.

## Remaining integration work

- Merge prerequisite header pull request #709 before this source pull request.
- Rebase and verify the exact source commit against the latest main before merging.
- Restore the GitHub Actions billing backstop; failed PR or main checks prohibit merging under the take-issue procedure.
- #504 retains its non-KPAD scope beginning at `0x803086D0`.

### Earlier extension-reader investigation

The following scratch results are historical and superseded by F-503-6.

The uncommitted scratch investigation of `read_kpad_stick` at `0x80306800` establishes additional train, guitar and balance-board paths. Train inputs use the classic status's trigger outputs; guitar inputs also use its two stick vectors. Board output calls `WBCGetBatteryLevel`, `WBCRead` and `WBCGetTGCWeight`, maintaining four-double arrays at `0x8047C2E0` and `0x8047C300`. The smoothing window at `0x804B8E70` is the float 400.0, and the rejection threshold in the constant pool at `0x804BB5A8` is the double -0.5. A 16-bit sample count is at `0x804B9AF8`, calibration state at `0x804B9AFA`, TGC averaging flag at `0x804B9AFB`, and the double TGC result at `0x804B9AF0`. These roles follow accesses and calls; original private variable names remain unrecovered.

The complete scratch candidate still differs in the averaging instruction schedule and address materialization. One source form produced 0x8C4 bytes against the target's 0x8D0; other forms added floating-register saves. Wii/1.0a, 1.1, 1.3 and 0x4201_127 did not resolve that candidate's differences, nor did removing file-level IPA or explicitly disabling contraction. This is evidence about those candidates only. No compiler policy or accepted source coverage is changed by these experiments. This scratch candidate was superseded by the exact implementation in F-503-6.

### Earlier sampling investigation

The sampling callback's format table at `0x803DE870` contains twelve `{DPD command, data format}` pairs, two each for core, freestyle, classic, guitar, board and train. Guitar uses format 11 with either DPD state; board uses format 12 and train format 10 with DPD off in both entries. Sampling extends the inline sixteen-entry ring through the optional external ring, updates gravity scales when requested, and invokes the application sampling callback even after a failed probe.

Board setup starts command 170, starts and polls WBC calibration, sends command zero, waits through the 50-sample settling interval (`u8` at `0x804B8E6C`), then averages raw pressure samples before calling `WBCSetZEROPoint` after the counter exceeds the float 400.0 window. The target marks commands pending only inside a successful-result branch; writing a boolean comparison directly produces a different instruction sequence. Gravity fallback components are separate ascending stores, not chained assignments.

A scratch candidate has the target's complete 0x768-byte size. Its best examined form reproduces 393 of 405 non-relocation instruction words; the remaining twelve differ in the integer registers used for the pressure and prior-count values during averaging. That candidate is not incorporated or counted among the 29 implemented functions. Ring selection, gravity, aiming, format selection, DPD controls and board state transitions reproduce the target instruction sequence, but that is not a whole-function acceptance claim. Scalar temporary forms, pointer/value inline helpers and declaration-order variants are preserved outside the repository as investigation aids; they do not justify changing compiler policy or marking the unit `Matching`.

## F-503-6: Complete readers, sampling and source-linked identity

| Function | Address | Bytes |
| --- | --- | --- |
| `read_kpad_stick` | `0x80306800` | `0x8D0` |
| `KPADRead` | `0x803070D0` | `0xC` |
| `KPADiRead` | `0x803070E0` | `0x72C` |
| `KPADiSamplingCallback` | `0x80307F60` | `0x768` |

`KPADiRead` acquires the per-channel read lock under an interrupt guard, scans the unified ring backwards, retains the newest raw sample for the next read, processes older samples oldest first, and returns processed outputs newest first. It distinguishes per-sample and aggregate button processing through `buttonProcessingMode`. The five-argument helper reports initialization, driver setup, lock, connection and no-data errors, and can preserve the caller's incoming output record on failure. The public three-argument wrapper supplies a null error pointer and disables preservation. `legacy_struct_alignment off`, scoped to the reader region, is required for the observed word-copy sequence; enabling it produced halfword copies and 0x79C bytes instead of 0x72C. Advancing the temporary-output pointer before computing the raw-ring index preserves the target register lifetimes.

The extension reader handles freestyle, classic, guitar, train and balance-board inputs. Sampling maintains the raw ring, gravity scales, sensor-bar origin, DPD state and device-specific format, and drives board calibration before invoking the application's sampling callback. A pinned CC0 reference with the same June 22, 2009 SDK banner supplies the version-specific control structure. Function-scope loop indices reproduce the target board averaging schedule; a single-iteration `do`/`while` with `continue` on an unknown device reproduces the sampling callback without artificial register controls. Both complete instruction streams are exact with this project's established compiler and flags; the reference game's different compiler configuration is not imported.

The initialized clamp maxima, board settling count/window and rotation angle are emitted in their target order. Explicit zero initializers keep the proven uninitialized minima, initialized flag, TGC result and board flags in their target order. The default gravity copy is one six-byte `{1, 1, 1}` object at `0x804BB5C0`, replacing the former four-byte/two-byte aliases.

Five literal symbols receive compiler-generated correspondence names in the split metadata: `@2735` at `0x804BB570` (-1.0f), `@2736` at `0x804BB574` (1.0f), `@2737` at `0x804BB578` (-0.75f), `@2738` at `0x804BB57C` (0.75f), and `@2811` at `0x804BB580` (2.0f). Each four-byte initializer was compared against the compiler object before renaming. These are reconstruction compiler labels, not recovered original symbol names. They disambiguate literal correspondence despite the extra stripped 200.0f object; no expected bytes, addresses, sizes, hashes or compiler flags change.

The standard build report confirms 100% code matching and all 33 retained functions. Text, data, BSS, sdata and sbss compare at 100%. The final linked ELF contains none of `KPADSetBtnRepeat`, `KPADSetObjInterval`, `KPADReset`, `calc_dpd2pos_scale`, `get_kobj`, `clear_balance_weights`, `reset_cl_status` or `get_unified_sample` as standalone function bodies. Direct final-DOL function comparisons include every relocated instruction, and full frozen verification passes. The original 321-object baseline retained identical code, data and relocation records. After refreshing onto main `17b160eee26394f2fc0c9eea28386028fcc7a90f`, a comparison between the rebuilt header prerequisite and source branch preserves all 339 pre-existing source objects despite the additive source-header declarations.

## Provenance

The motion and pointing algorithms and helper names derive from [Petari KPAD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/kpad/KPAD.c), CC0. Comments are removed and SDK C headers replace the reference's C++ convenience headers. Newer-format conditions come from the target instructions. The complete sampling and extension readers also derive from [InusualZ/mhtri-decomp KPAD at ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/blob/ac489be692a1273afff8574b2402aecba2ae2b8f/src/KPAD/kpad.c), under its [CC0 license](https://github.com/InusualZ/mhtri-decomp/blob/ac489be692a1273afff8574b2402aecba2ae2b8f/LICENSE). The source was inspected for file-level license exceptions before reuse; none were found. Its comments and guessed compiler configuration are not imported. The shared layout's separate provenance is recorded in F-503-4. A copied IDA database outside the repository was used only as a reading aid; no decompiler output is included in source.

## Traps

- **Preserve the inline helper implementations when extracting a partial source.** Omitting `clamp_acc`, `clamp_trigger`, or `calc_horizon` let the C compiler infer external calls and substantially shortened callers. Restoring the actual helpers recovers the original instruction streams; changing compiler flags would have hidden the cause.
- **Raw instruction comparisons exclude relocation evidence.** The nineteen diagnostic matches do not justify `Matching`; the unit's current objdiff score and explicit fallback remain the acceptance record.
- **Expression order affects the button-reader registers.** Loading old classic buttons before forming the reset-gated new word produced nine differing instruction words. An explicit new-button temporary computed first reproduces the original registers without changing flags or inserting assembly. This is a local source-order finding, not a compiler-wide rule.
- **Chained board-array resets change folded BSS accesses.** The connection callback initially had the correct size but referenced the two board arrays in the wrong order. Separate, ordered zero assignments reproduce the original stores and folded offsets `0x1610`/`0x1630` from the channel-state base; adding the two arrays also makes BSS 100%. Do not dismiss non-relocated folded offsets as linker work.
- **Equivalent initializer loops can schedule differently.** A `for` loop produced the right 0x444-byte initializer with one differing non-relocation word: a constant load swapped with a relocated callback-address load. The reference's `do`/`while` form reproduces every word. Check both sides of relocated words rather than masking an adjacent scheduling difference.

- **Structured board loops can preserve the target schedule where scalar rewrites fail.** Earlier averaging candidates matched 393/405 sampling words but swapped twelve integer-register operands. The same-version reference's function-scope loop and structured device-mode exit reproduce every word without assembly or compiler changes. Treat those earlier probe residuals as superseded.
- **Struct alignment affects whole-record copies.** A reader candidate under legacy struct alignment emitted halfword copies and grew by 112 bytes. The scoped native alignment setting reproduces word copies and every reader instruction; preserve the scope rather than changing global compiler policy.
- **Objdiff constant correspondence can need metadata names.** A stripped helper's extra literal confused five otherwise identical pool references. Proving and recording compiler-label correspondences restores 100% code scoring; the full source-linked frozen artifact remains the acceptance proof, including all constant-pool bytes.
