# Child animation executor, 2026-11-04

## Exact isolated function

`nw4r::ef::AnimCurveExecuteChild` at `0x8022CB80` now reproduces all 3,468 original function bytes. Both function byte ranges have SHA-256 `06d690245c90d0defbefd101cbd95e5fd86f841fb24d6a7ea405dd1dd5e9de74`. The direct configured Wii/1.1 compiler reports 100%, and the raw 867 instruction words agree, including branch displacements. All 27 relocations have the same function-relative offsets and types. External and local function calls retain their verified names; floating-point references retain the same section offsets, sizes, and literal bytes.

The narrow change preserves the raw bytes and scores of all seven other real functions. It raises this unit from four to five exact functions. U8, F32, and Rotate remain unfinished, so the complete unit is still `NonMatching`; this evidence does not claim that the unit is source-linked.

The accepted complete source snapshot has SHA-256 `1b4060a29a6b692ac65aef1dbb8ff5accce3d470619a7e257d215c711b5bd992`. The control source has SHA-256 `b4fa0a323c3d396961d01f75a0e35449b7d24e3c7ad5819d98150382de9e690a`. The integration fragment, which contains the two local inline helpers and the executor, has SHA-256 `2c2e9514f8db721408be11a53e95a60b0b21beb8b4f7ca619075941f2ebaf80f`.

Relative to that control, `.text` stays 21,740 bytes, `.rela.text` stays 3,264 bytes, and `.strtab` stays 1,273 bytes. The only changed sections are `.text`, seven bytes in existing text relocation offsets, and compiler-generated names in `.strtab`. Every data section and the symbol table are byte-identical. No new out-of-line function is emitted. The source helper definitions do not change the compiled function inventory.

## The conversion-slot counter

The prior best candidate reached 98.60784%. Each frame calculation required eleven integer-to-float conversions. The original used the same temporary stack-slot order in both calculations, but the source's second calculation reversed all 22 low-word stores and double loads between stack offsets 8 and 16. Extra inline wrappers, argument orders, guard objects, exception settings, and output categories did not control these compiler-generated slots.

Read-only tracing of the configured compiler establishes a different mechanism from ordinary local stack-object ordering. The executable has no main PE COFF symbol table, but its appended CodeView `NB11` debug payload contains public records of type `0x1009`. Resolving those records identifies these addresses in the Wii/1.1 image with preferred base `0x400000`:

| Compiler item | Address | Observed role |
| --- | --- | --- |
| `_convert_unsigned_to_floating` | `0x5B1C00` | Emit an unsigned integer conversion |
| `_convert_integer_to_floating` | `0x5B1DC0` | Emit a signed integer conversion |
| `_setup_floating_conversion` | `0x5B1FA0` | Set up shared conversion scratch objects |
| `_num_int_to_float_converts` | `0x7751AC` | Total conversion inventory for the function |
| Unnamed selector | `0x730298` | Alternate between the two shared scratch objects |
| Scratch-object pointers | `0x73029C`, `0x7302A0` | The selected compiler stack objects |
| Shared-conversion enable flag | `0x7302A4` | Enable the reusable pair |

The symbol named `_num_int_to_float_converts` is not the alternating counter: its value stays 22 while the baseline function is generated. Setup resets the separate selector to zero. Every signed or unsigned conversion consumes its current value and changes it to `(value + 1) & 1`. Eleven conversions therefore reverse the starting slot of the following helper.

The retained D-0016 control is a plain unused local between the two calls:

```cpp
f32 conversionPhase = static_cast<f32>(tick);
```

This conversion survives until native instruction generation. The trace shows an inventory of 23 and an actual twelfth conversion call between the two groups of eleven. The selector starts at zero for both live frame calculations. Later optimization removes every instruction for the unused local. The final function still has the original 867 instructions, 112-byte frame, and all 46 target conversion-scratch accesses. No compiler state, binary, or object was patched.

The two helper definitions used during diagnosis were simplified to a single `CalcChildFrame`; that simplification retains the exact result. Its output-reference form and existing reassociated integer expression `(period * (rLoop + 1) - 1) - (tick - 1)` are inherited source controls from the preceding investigation. The arithmetic identity uses unsigned values and retains their modulo behavior.

## Register and scheduling controls

After fixing the scratch slots, the remaining operands were closed with local source changes:

- The outer declarations are ordered `tickA`, `divA`, `tickB`, `divB`, reproducing registers 27, 26, 25, and 24 respectively.
- The non-fitting turn path gives its cached loop count an explicit `u8` local. Its assignment remains inside the original short-circuit condition, preserving the conditional load. The fitting path declares its byte loop count before its period but assigns it after the period calculation. Both period/count register pairs then agree.
- `CalcChildFrame` initializes `rEnd` immediately after `rLoop`. This preserves the original first helper's two zero loads. The unused second end output disappears during optimization.
- A caller-owned `u32 secondFrame = tickB` before the first division comparison retains the original second-frame truncation, hoisting, and comparison operands. A cast only inside the comparison does not.
- `FindChildCurveKeyIndex` is a local copy of the existing key-search algorithm with a mutable lower-bound reference. The caller declares `s32 searchStart`; the helper initializes it to zero. This preserves the named outer value and corrects the last two `r5`/`r6` pairs. Applying the reference to all five calls is exact and avoids separate call-site variants.

The explicit pointer-to-`u32`-to-pointer command-list identity is retained for the target's 32-bit ABI. The calls also retain `const_cast<AnimCurveHeader*>(pHeader)` even though the helper parameter is const. Removing just those casts regresses the final candidate to 98.65629%; the compiler retains different alias/value metadata through this otherwise redundant expression. This is a measured D-0016 control, not evidence that the original source used that cast. Removing two entirely unused helper definitions and giving both retained helpers internal linkage changes no function bytes.

These named-lifetime, cast, output-reference, and dead-conversion controls are fake-match constructs under D-0016. The naturally spelled reconstruction, ordinary inline-depth changes, output width changes, and declaration-only variants did not reproduce the function. None of the rejected forms is retained.

## Rejected approaches

Manual union conversions can preserve the original frame size and high-word initialization, but ordinary C++ subtraction emits `fsub` followed by `frsp` instead of `fsubs`. The bare `__fmsubs` builtin takes float operands and rounds the union's double value before the fused operation; declaring a double prototype instead makes it an ordinary external call. These variants do not reproduce the required conversion and were rejected.

Pointer-union identities can delay a provably false condition until instruction selection, but the compiler keeps the resulting self-comparison and branch. That does flip the slot selector and also leaves unwanted instructions. The plain unused float local is sufficient and smaller. Source identities with overwritten union storage also reproduced its counter effect, but are unnecessary.

Explicit first-key locals, same-type key reassignments, enum views, parameter permutations, and extra forwarding levels did not correct the remaining search-register pairs. Passing the actual lower-bound state by mutable reference from the executor did.

## Reproduction

Private artifacts live under `tools/local/tail-20261104/smallgame/AnimCurveChild/`. `child_accepted/` contains the final complete source, narrow `child_section.cpp`, compiler log, object, complete objdiff JSON, and `acceptance.json`. `child_accept.py` compiles a canonical control, replaces only the Child section, checks every other function's raw bytes, and validates all 27 child relocations including literal content.

The copied original object is `original.o`, SHA-256 `214d84b4511c8f10d599c044bd912044f85afc5800d11630d30f2ce1c07e24d2`. Compiles use the configured native command with dependency-file emission removed, and have private output paths. The full shared build remains the coordinator's responsibility.

`child_compiler_symbols.py` resolves the embedded public records into `compiler-symbols.json`. `child_conversion_trace.py` uses read-only Frida hooks and the configured flags. The diagnostic traces are `child_control/conversion-trace.txt` and `child_compare_0/conversion-trace.txt`; the latter records all 23 conversion calls and their selector values. The accepted mechanism does not depend on Frida being present during an ordinary build.

`child_dead_value.py`, `child_outer_order.py`, `child_inner_control.py`, `child_period_control.py`, `child_compare_control.py`, `child_search_inputs.py`, `child_simplify.py`, and `child_cleanup_controls.py` reproduce the successful progression and its narrow negative controls. The preceding investigation is recorded in [the animation continuation](animcurve-20261104-continuation.md).

## Traps

- A swapped pair of compiler conversion slots can follow a global alternating selector rather than inline-depth stack-object order. Trace the conversion generator before applying ordinary frame-bucket rules to such slots.
- An unused conversion is not necessarily removed before instruction generation. Here it consumes a scratch-slot selection and only then disappears. Assuming early removal would have rejected the exact source control without testing it.
- A source cast can affect alias/value metadata even when the called helper immediately receives the original const-qualified type. Retain a compiling negative control before deleting a seemingly redundant cast from an exact candidate.
- Public compiler symbols may live in appended CodeView data even when the PE COFF symbol count is zero. Inspect the debug directory; a raw string match alone does not establish a callable address.
