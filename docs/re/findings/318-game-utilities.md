# F-318-1: Math, display and render-mode consumers

## Scope and status

The original #318 inventory has 23 functions and 5,648 code bytes, from `0x801B64B0` through the CModel constructor at `0x801B7AA0`. The survey title does not establish a source-file boundary. The constructor alone does not establish the complete CModel class or translation unit; later methods and nested animation classes extend into neighboring inventories.

This checkpoint changes documentation only. No function is source-selected, no progress is credited, and no source-unit boundary is accepted. The isolated worktree started at `fb63a24627e72e30f4f48badb7e1b1b82cb04bef`. Native comparisons read the frozen original DOL without changing its manifest.

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

Every relocation in the accepted consumers is resolved to its actual native callee or constant address before comparison. No relocation-bearing instruction is ignored. The 11 exact diagnostic functions account for 3,096 owned code bytes; this is an inventory of experiments, not decompilation credit.

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
| `0x801B6E30` | 356 | Recursive exponentiation by squaring, positive integer exponent | PositiveIntegerPower |
| `0x801B6FA0` | 732 | Zero, negative and positive integer exponent wrapper | IntegerPower |
| `0x801B7310` | 104 | Reduce a floating angle modulo 360, adjusting a negative result | NormalizePositiveAngle |

The names are descriptive. They have not been recovered from original symbols and have not been published as canonical interfaces.

The power routine uses ordinary recursion and signed division/remainder. The existing compiler naturally inlines four recursive levels into the 356-byte body, and the signed wrapper naturally emits 732 bytes. There is no manual recursion expansion.

Nine retained exception entries belonging to these consumers reproduce 72 native extab bytes and nine 12-byte extabindex records. The two remaining exact leaf consumers have no exception records. Constant relocations are paired with the demonstrated native values and addresses; a complete unit-wide constant/data extent is not yet accepted.

## Render-mode interface remains unresolved

An ordinary `GXRenderModeObj` value return with a local mode reproduces every byte of the 1,176-byte producer and all 23 relocations. Its exception record is also exact. The choices reference the actual SDK templates at `0x803D6300`, `0x803D633C`, `0x803D63B4`, `0x803D63F0` and `0x803D642C`; the remaining calls pair with VIGetScanMode, VIGetTvFormat, SCGetEuRgb60Mode, GetDisplaySize and SCGetAspectRatio.

The sole native code caller found so far is the graphics initializer at `0x801C6DB0`, owned by #326. The earlier ordinary output-pointer caller candidate reproduces its complete 476-byte body, but this alone does not prove the producer's declaration. Rewriting the exact producer as an ordinary output pointer plus final structure assignment emits 1,212 bytes. Thus the producer is discriminating evidence against treating that caller-only fixture as a recovered interface.

Ordinary value-return consumer forms remain unaccepted: branch-local value initialization emits 592 bytes, branch-local const-reference binding emits 480, and a const reference bound through the conditional operator emits 884. These measurements explain the current limitation; none is accepted or used to force a declaration. The complete producer/caller relationship must be explained before adding a canonical render-mode interface or selecting either unit as Matching.

## Remaining work

The signed-angle routine at `0x801B7280` now has the correct 136-byte extent but retains four arithmetic/register words. Its positive-angle sibling is exact, including the original ordering of floating comparisons and boolean materialization. Neither NaN behavior nor comparison operand order may be changed merely to obtain a score.

The decompression routines at `0x801B6C70` and `0x801B6D50` use a shared result, MEM2 allocation and a function-pointer deleter. Their faithful generic smart-pointer construction still needs full code, target and exception acceptance against current canonical templates. The later list, callback and CModel animation helpers require complete evidenced class interfaces and unit-boundary work. They are not counted by the table above.

## Traps

- A caller that matches an output-parameter declaration can still belong to a value-return ABI. Here the ordinary output-pointer producer is 36 bytes larger while the value-return producer is fully exact. Preserve both sides' evidence and leave the declaration unresolved until the caller is explained.
- VI mode classification and branch placement must preserve the actual mode set. The explicit-size helper distinguishes EURGB60, MPAL, PAL and default groups; treating every mode as a single generic height loses native behavior even when several constants coincide.
- Instruction coverage does not establish the enclosing CModel translation unit. Keep the original issue inventory, source-file boundary evidence and compiler-consumer results separate.
