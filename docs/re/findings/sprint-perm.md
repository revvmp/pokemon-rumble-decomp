# Sprint perm: bulk permuter runs over the long tail

## Harness

decomp-permuter parses C with pycparser, so it only handles the C units (TMC JPEG, FA `pdm_mbr`). Every other remaining function is C++, which got a separate text-level permuter that reuses decomp-permuter's scorer. Both live outside the repository, in the agent scratchpad directory `perm/`:

- `dp/` is an unmodified decomp-permuter clone. `dp_run.py` starts it with two runtime patches. `preprocess` returns the input unchanged, because there is no `cpp` on Windows; `dpsetup.py` writes `base.c` with `mwcceppc -EP` from the unit's own flags (only for units without `#pragma`, which `-EP` drops). The objdump step disassembles one symbol (`--disassemble=<name>`) from the split target object, rewrites branch targets relative to the function start, and maps `@123` labels to one name, so relocation-only differences score zero. This made `target.o` assembly unnecessary.
- `cxxperm.py setup` reads `report.json`, skips `pad_` entries and functions at 0% (not emitted), finds each function body in its source file by demangled name, and records the unit's exact compile command from `build.ninja` (compiler version, cflags, `sjiswrap` when the rule needs it). It compiles a copy of the whole source file (same basename, so `@unnamed@<file>_cpp@` names stay), against a frozen copy of `include/` and `src/`, with `-i <source dir>` for quoted includes. Work directories need short paths: mwcc rejects file names near `MAX_PATH`.
- `cxxperm.py run` mutates the function body and up to eight same-file callee bodies, which matter because most game functions are inline-heavy: adjacent-statement swaps and moves (only for statements without jumps that share no variables), declaration moves, commutative operand swaps and comparison flips, `!= 0` forms, `if`/`else` inversion, integer type changes, `const`, split and merged initialisation, `++` forms, compound assignment, inlining a single-use local, and `for` to `while`. With `PERM_FAKE=1` it also introduces `__typeof__(e) tmp = e;` temporaries. It hill-climbs for a fixed time and writes each improvement as a diff.
- `cxxperm.py drive --jobs N --minutes M` runs the queue in parallel; `status --improved` prints the summary table. `minimize.py <run>` reapplies a diff hunk by hunk and keeps the smallest subset with the same score, which separates the one change that matters from the drift.

Scores come from decomp-permuter's scorer, not objdiff: an improvement must be rechecked with `tools/re/unitdiff.py`. A whole-unit compile takes one to five seconds, so a 20-minute run tries 250 to 1,000 candidates.

## Runs

- Wave 1: 102 functions (all non-100% functions whose definition is in the unit's own source file), 20 minutes each, 16 in parallel. 86 ran to time, 5 exhausted their mutation space, 6 had no mutable statements (bodies that are a single call into another file), and 18 were templates defined in headers and were skipped.
- Wave 2: 18 functions rerun on updated source, 25 minutes each.
- decomp-permuter on `pdm_mbr_check_master_boot_record` and the seven TMC JPEG converters, 40 minutes each with `-j 2`.

## Landed

- NPpuVM `BinaryOp`, `BitwiseOp`, `CompareOp`: the two operands are `const CValue&` (93.83 to 94.46, 97.23 to 98.76, 97.94 to 98.85%). Found by a single `const` mutation.
- ParticleManager `Calc`: the field affect is scaled by `0.57735026f * VEC3Len(&axisSum)` in the case that uses it instead of a function-level `emitterScale`, and `kind` is `const u8` (94.20 to 96.93%). `minimize.py` showed the inlined length alone halves the permuter score.
- CLayout `CalculateAttachedMtx`: the attachment pane pointer is `const nw4r::lyt::Pane*` (94.94 to 95.62%).
- CLayout `AdvancePaneMotions`: `std::max` takes the first motion's frame directly, without the `startFrame` local (95.40 to 96.28%).
- Fake matches under D-0016, both reached by decomp-permuter and reduced to one construct: `ConvertRGB565MCU420` with a `(u64)` cast on its second texel load, and `ConvertRGBA8MCU422` with the second luma written out and a `(u32)` cast on its AR row term. Both are in the fake-match ledger.

## Fake-only and rejected results

- BUI `OnInput`: the best candidate made the loop counter `bool`. A `u8` counter scores 94.89% against 91.70%, but adds `clrlwi` instructions the target lacks; the real difference is r27 and r28 swapped.
- main `HandleFileError`: the permuter moved the `drive < 2` test after the switch, which removes it. The target tail-merges that test into the shared `return false` block. `if (drive >= 2 && drive < 512)`, `||` forms, a `bool` result, explicit `case 0: case 1:` and comparisons instead of a switch all score lower.
- CGameSession `GetSnapshot`: calling `mOyaInfo.GetCount()` a second time improves the score; using it only in the store gains 0.14%. The target loads the start time before the play time and loads the count before the snapshot stores.
- CRemoteMemory `FinishRead`: the only gain came from swapping the two branches of `FindValidBlock`, which changes behaviour.
- TMC `ConvertRGB565MCU411`: putting the rounding constant first in only the first `dstY[0]` expression gives 94.36%; changing all four gives 89.57%. RGB565 `MCU422` and RGBA8 `MCU411` gains came from rewriting loops as `while` and moving increments, all fake.
- `pdm_mbr_check_master_boot_record`: the best result reorders the byte terms of the 32-bit little-endian loads irregularly ((`0x1C8 << 16) + (0x1C7 << 8) + (0x1C9 << 24) + 0x1C6`); `|`, reversed, nested-shift and multiplication forms all score lower.
- NPpuVM `CollectGarbage`, `Load`; ef_animcurve `AnimCurveExecuteU8`/`F32`/`Rotate`; CMiniMap `CalcView`; CMsgRes `ConvertEscape`: under 5% score change, all from operand swaps and loop rewrites.

## Traps

- **decomp-permuter scores are not objdiff scores.** It ignores branch targets by default and our relocation normalisation hides label differences; recheck every candidate with `unitdiff.py`. A replace-all test edit can also touch a neighbouring function with the same text: restrict test edits to the function's own span.
- **`mwcceppc -EP` drops `#pragma` lines,** including `-ppopt pragma`. Preprocessed sources are only valid for units without pragmas; the C++ harness compiles the original file instead.
- **Statement moves need a jump check.** Without one the permuter moved code past `return` and swapped two returning `if` blocks, which scored better and is wrong.
- **Most remaining C++ differences are register numbering.** Across 120 runs no C++ function reached zero, and most candidates scored the same as the base.
