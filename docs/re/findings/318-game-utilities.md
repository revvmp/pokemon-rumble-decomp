# F-318-1: Math, display and render-mode consumers

## Scope and status

The original #318 inventory has 23 functions and 5,648 code bytes, from `0x801B64B0` through the CModel constructor at `0x801B7AA0`. The survey title does not establish a source-file boundary. The constructor alone does not establish the complete CModel class or translation unit; later methods and nested animation classes extend into neighboring inventories.

The bounded first two functions are reconstructed, source-selected and verified against the frozen DOL: two owned functions and 76 code bytes. The remaining 21 original issue entries, totaling 5,572 code bytes, are incomplete. Diagnostic results below do not add to this accepted source coverage. The isolated worktree started at `fb63a24627e72e30f4f48badb7e1b1b82cb04bef`. Native comparisons read the frozen original DOL without changing its manifest.

## Compiler and reproducibility

The diagnostic driver uses the actual captured BController command in `C:/MassStorage/GitHub/326-game-command.json`: Wii/1.1, the existing game flags, exceptions and RTTI enabled, hardware floating point, contraction disabled and file IPA. It compiles against this claim's canonical includes. No reference include fallback or compiler-option search is used.

Run the following outside scripts from the claim worktree:

```text
python C:/MassStorage/GitHub/318-compile-probe.py 318-math-display-probe
python C:/MassStorage/GitHub/318-audit-math-display.py
python C:/MassStorage/GitHub/318-compile-probe.py 318-power-probe
python C:/MassStorage/GitHub/318-audit-power.py
python C:/MassStorage/GitHub/318-compile-probe.py 318-render-mode-probe
python C:/MassStorage/GitHub/318-audit-render-mode.py
python C:/MassStorage/GitHub/318-compile-probe.py 318-angle-probe
python C:/MassStorage/GitHub/318-audit-angle.py
python C:/MassStorage/GitHub/318-audit-metadata.py
```

The signed-angle audit intentionally fails on its four unresolved words. Its positive-angle sibling is exact. The other listed instruction audits and the metadata audit pass. These are native compiler consumers, not full-build acceptance gates.

## Exact instruction consumers

Every relocation in the accepted consumers is resolved to its actual native callee or constant address before comparison. No relocation-bearing instruction is ignored. The 13 exact diagnostic functions account for 3,544 owned code bytes; this is an inventory of experiments, not decompilation credit.

| Native address | Bytes | Proven behavior | Descriptive diagnostic name |
| --- | --- | --- | --- |
| `0x801B64B0` | 20 | Sine of a normalized quarter turn, through NW4R SinFIdx | SinQuarterTurn |
| `0x801B64D0` | 56 | One minus cosine of a normalized quarter turn | OneMinusCosQuarterTurn |
| `0x801B6510` | 116 | Pixel aspect selected from system TV format and aspect setting | GetPixelAspectRatio |
| `0x801B6590` | 68 | Pixel aspect selected from explicit TV mode and aspect setting | GetPixelAspectRatio |
| `0x801B65E0` | 180 | Current render-mode display width times pixel aspect, divided by height | GetDisplayAspectRatio |
| `0x801B66A0` | 1,176 | Select SDK render template, adjust horizontal width and center it | ChooseDefaultRenderMode |
| `0x801B6B40` | 148 | Maximum display dimensions for a VI TV mode | GetDisplaySize |
| `0x801B6BE0` | 140 | Safe display dimensions for TV mode and aspect setting | GetSafeDisplaySize |
| `0x801B6C70` | 224 | Decompress an LZ buffer into shared MEM2 storage and flush it | DecompressLZ |
| `0x801B6D50` | 224 | Decompress a Huffman buffer into shared MEM2 storage and flush it | DecompressHuffman |
| `0x801B6E30` | 356 | Recursive exponentiation by squaring, positive integer exponent | PositiveIntegerPower |
| `0x801B6FA0` | 732 | Zero, negative and positive integer exponent wrapper | IntegerPower |
| `0x801B7310` | 104 | Reduce a floating angle modulo 360, adjusting a negative result | NormalizePositiveAngle |

The names are descriptive. They have not been recovered from original symbols and have not been published as canonical interfaces.

The power routine uses ordinary recursion and signed division/remainder. The existing compiler naturally inlines four recursive levels into the 356-byte body, and the signed wrapper naturally emits 732 bytes. There is no manual recursion expansion.

Eleven retained exception entries belonging to these consumers reproduce 152 native extab bytes and eleven 12-byte extabindex records. The two remaining exact leaf consumers have no exception records. Constant relocations are paired with the demonstrated native values and addresses; a complete unit-wide constant/data extent is not yet accepted.

The decompression driver is `318-decompression-probe.cpp`, compiled by `318-compile-probe.py 318-decompression-probe` and audited by `318-audit-decompression.py`. Both 224-byte bodies and all 18 relocations are exact. Embedded RTTI at `0x803A42C0`, reached through the native control vtable at `0x803A42AC` and RTTI record `0x804B6940`, identifies `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>`. The ordinary current canonical `shared_ptr<u8>` custom-deleter constructor is therefore used without altering the standard headers. The helper's private name is not recovered from this evidence.

The current canonical header also emits six matching outside support functions: the 108-byte custom guard, 132-byte shared destructor, 64-byte common destructor, 76-byte get_deleter, 20-byte dispose and 64-byte control destructor. Their actual targets and exception records are independently paired, including the native guard at `0x800BE5D0` and shared destructor at `0x800657E0`. They receive no owned-byte credit here. The two owned consumers each have 40 exact extab bytes, including both destructor callbacks. This result does not validate unrelated default-deleter construction policies.

## F-318-2: Bounded quarter-turn unit proposal

The isolated `318-quarter-turn-probe.cpp` uses the canonical NW4R SinDeg and CosDeg helpers, with ordinary quarter-turn inputs. `318-audit-quarter-turn.py` proves both resolved bodies, the complete 88-byte text extent from `0x801B64B0` to `0x801B6508`, and the entire 12-byte constant pool from `0x804BA3B8` to `0x804BA3C4`. The exception entry is eight bytes at `0x80038DA8`, with its index at `0x80050A30`. No initialized data, RTTI or zero-initialized storage is emitted by this isolated unit.

The next aspect helpers begin at `0x801B6510`, with their float pool starting at `0x804BA3C8`. Combining the two groups into one source moves the latter constants four bytes early; the native gap instead agrees with the following unit's alignment. These complete section comparisons support the bounded proposal. `QuarterTurn.cpp` and its function names are descriptive; no original filename or spelling is claimed.

The initial split attempt was rejected because the automatic `lbl_804BA3C0` symbol covered eight bytes, merging the demonstrated four-byte `1.0f` with the following four zero bytes. The exact source pool ends at `0x804BA3C4`. The owned symbol extent is corrected to four bytes; the following alignment bytes are left outside the source split. No outside symbol size or original artifact is changed.

At `1d8155cc`, rebased onto `82571e1d`, the fresh split-only configure, full default Ninja, explicit all_source and actual-directory frozen verifier all exit zero. Ninja compiled the current game NonMatching sources too. The next step registers the source NonMatching and compares the actual compiler object before source selection.

At `004028e7`, the actual registered NonMatching source compiles with the real pipeline; both functions and all four sections score 100%. The strict quarter-turn driver also accepts the actual `build/wpse-v1/src/game/QuarterTurn.o`, resolving every instruction target and comparing the complete text, float pool and exception records. At `e80e4464`, selecting that source as Matching passes configure, the real default link, the SHA-1 build check, explicit all_source, a separate plain Ninja invocation and actual-directory frozen verification. The linked DOL SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The generated Ninja graph explicitly lists `build/wpse-v1/src/game/QuarterTurn.o` as linked, replacing the original fallback for this unit. No force-active source pragma, hand padding, raw executable bytes or flag change is needed. A full frozen-DOL scan also finds incoming branches to SinQuarterTurn at `0x8009816C`, `0x8009A314`, `0x800B0768`, and to OneMinusCosQuarterTurn at `0x8009917C`, `0x80099CD4`; the incomplete IDA view missed these references. The names remain descriptive rather than recovered original spellings.

## Render-mode interface remains unresolved

An ordinary `GXRenderModeObj` value return with a local mode reproduces every byte of the 1,176-byte producer and all 23 relocations. Its exception record is also exact. The choices reference the actual SDK templates at `0x803D6300`, `0x803D633C`, `0x803D63B4`, `0x803D63F0` and `0x803D642C`; the remaining calls pair with VIGetScanMode, VIGetTvFormat, SCGetEuRgb60Mode, GetDisplaySize and SCGetAspectRatio.

The sole native code caller found so far is the graphics initializer at `0x801C6DB0`, owned by #326. The earlier ordinary output-pointer caller candidate reproduces its complete 476-byte body, but this alone does not prove the producer's declaration. Rewriting the exact producer as an ordinary output pointer plus final structure assignment emits 1,212 bytes. Thus the producer is discriminating evidence against treating that caller-only fixture as a recovered interface.

Ordinary value-return consumer forms remain unaccepted: branch-local value initialization emits 592 bytes, branch-local const-reference binding emits 480, and a const reference bound through the conditional operator emits 884. These measurements explain the current limitation; none is accepted or used to force a declaration. The complete producer/caller relationship must be explained before adding a canonical render-mode interface or selecting either unit as Matching.

A further outside experiment takes the address of the returned temporary within the SetRenderModeObj full expression. CodeWarrior accepts this extension with two `not an lvalue` warnings, and the complete six-function graphics consumer then reproduces all 1,904 code bytes, 132 targets, 48 extab bytes and initialized data. This demonstrates a possible ABI explanation, but no evidence establishes that nonstandard source form as original. It is not published, source-selected or used to override the unresolved canonical interface.

## Remaining work

The original inventory is accounted for as follows. Only the first two entries are linked reconstructed source. The diagnostic column does not imply source selection.

| Address | Bytes | Source selected | Diagnostic status |
| --- | --- | --- | --- |
| `0x801B64B0` | 20 | Yes | Complete actual object and frozen link |
| `0x801B64D0` | 56 | Yes | Complete actual object and frozen link |
| `0x801B6510` | 116 | No | Resolved native instructions and EH exact |
| `0x801B6590` | 68 | No | Resolved native leaf exact |
| `0x801B65E0` | 180 | No | Resolved native instructions and EH exact |
| `0x801B66A0` | 1176 | No | Exact producer; interface/caller source unresolved |
| `0x801B6B40` | 148 | No | Resolved native instructions and EH exact |
| `0x801B6BE0` | 140 | No | Resolved native instructions and EH exact |
| `0x801B6C70` | 224 | No | Current canonical custom shared consumer exact |
| `0x801B6D50` | 224 | No | Current canonical custom shared consumer exact |
| `0x801B6E30` | 356 | No | Recursive consumer and EH exact |
| `0x801B6FA0` | 732 | No | Recursive consumer and EH exact |
| `0x801B7280` | 136 | No | Four instruction words differ |
| `0x801B7310` | 104 | No | Resolved native instructions and EH exact |
| `0x801B7380` | 60 | No | Yield loop and flag mask inspected; class unresolved |
| `0x801B73C0` | 12 | No | Animation-program flag access; class unresolved |
| `0x801B73D0` | 216 | No | Animation callback and bad_function_call path inspected |
| `0x801B74B0` | 220 | No | Shared-value list destructor; templates incomplete |
| `0x801B7590` | 492 | No | CModel nested partial-animation destructor inspected |
| `0x801B7780` | 12 | No | Animation resource access; class unresolved |
| `0x801B7790` | 676 | No | Animation resource construct/bind path inspected |
| `0x801B7A40` | 84 | No | Animation length/override selection inspected |
| `0x801B7AA0` | 196 | No | CModel constructor and two base vptrs inspected |

The signed-angle routine at `0x801B7280` now has the correct 136-byte extent but retains four arithmetic/register words. Its positive-angle sibling is exact, including the original ordering of floating comparisons and boolean materialization. Neither NaN behavior nor comparison operand order may be changed merely to obtain a score.

The later list, callback and CModel animation helpers require complete evidenced class interfaces and unit-boundary work. They are not counted by the table above. The utility unit's complete initialized data, retained support selection, original source boundary and full linked artifact remain unaccepted.

## Traps

- A caller that matches an output-parameter declaration can still belong to a value-return ABI. Here the ordinary output-pointer producer is 36 bytes larger while the value-return producer is fully exact. Preserve both sides' evidence and leave the declaration unresolved until the caller is explained.
- VI mode classification and branch placement must preserve the actual mode set. The explicit-size helper distinguishes EURGB60, MPAL, PAL and default groups; treating every mode as a single generic height loses native behavior even when several constants coincide.
- Instruction coverage does not establish the enclosing CModel translation unit. Keep the original issue inventory, source-file boundary evidence and compiler-consumer results separate.
