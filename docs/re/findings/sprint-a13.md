# Sprint a13

## Traps

- **Unit scores near 0% with fully written source can be a symbol-name mismatch.** The #863 rename gave every script binding the placeholder `ScriptBinding_<name>`, but reconstructed sources define mangled names (`<name>__FPQ24NPpu7CEngine`). Unitdiff then shows "missing" for every function. ScriptGame went from 1/170 to 140/170 at 100%, and ScriptEffect, ScriptLayout and ScriptCameraMotion gained 22, 12 and 25 functions, by renaming the `ScriptBinding_` symbols in the unit's range to the mangled names its object produces (match by `x__F` prefix, unique). Check this before assuming code is missing in any script unit.
- **Out-of-line helper placeholders pair by size.** A `fn_` that unitdiff lists as missing is often a static helper the source already emits under a mangled name (`GetWazaEffectParam`, `LayoutMotion`, `PopCameraMotion`). Rename it by address once the unique same-size function in the rebuilt object is identified.
- **Do not rename template instantiations that another object already defines.** Naming CLayout `shared_ptr_deleter`/`move_ptr` functions in ScriptLayout, or `shared_ptr<CSimpleGround>::~shared_ptr` in ScriptGame, gave multiply-defined linker errors. Keep their placeholders.
- **name_callees applied outside the unit can fail the link.** Its 16-17 renames of addresses such as 0x8014FB20 and 0x80155D00 broke `ninja`; they were left out.

## CPpuEnv

`0x801035D0`-`0x8010623C` is `game/CPpuEnv.cpp`: `CPpuEnv` (derived from `NPpu::CEnvironment` and `enable_shared_from_this`, a `std::map<std::string, shared_ptr<CCode>>` script cache at `0x2C`, a loading flag at `0x28`), `CPpuCode` (a `CCode` with a file reference and name), `CreateScriptEnvironment`, `LoadScriptCode` and the `CEnvironment::CollectGarbage` cache clear. The unit also owns the 670-entry native binding table in `.rodata` and the binding-name strings in `.data`; those are not reconstructed. `LoadScriptCode` is at 89.8%, `CreateScriptEnvironment` and the map find/insert/deleter instantiations are 100%. The tree `destroy` instantiation is 84.8% and `CollectGarbage` 0% because the rebuilt `destroy` inlines the pair destructor at one extra site (inline budget); not resolved.

## Session 2 findings

- **A library inline can be too small to reproduce the original inlining.** `std::map<std::string, shared_ptr<CCode>>` tree `destroy`, the map destructor and `CollectGarbage` inlined the pair destructor at too many sites. The cost model (F-i1-1) counted our `basic_string::~basic_string` as cheaper than the original's. Reading the long representation through its accessors (`alloc().deallocate(long_data(), long_cap())`) adds the missing inline cost with identical code, brought `CPpuEnv` `destroy`, `~CPpuEnv` and the main-unit map `destroy` to 100% and regressed nothing (+5 matched functions). Check a report diff over the whole build when changing a shared inline. Adding a statement to `~shared_ptr` instead regresses every integer-keyed map `destroy` (CLayoutManager, CModel, CLayout).
- **Virtual `AddRef`/`Release` hide in unnamed vtable slots.** `fn_` placeholders referenced only from a vtable and extab entries are virtual overrides and base-subobject destructors. `CPpuCode` and `CPpuEnv` keep a `SelfReference<T>` (`int` count plus a `shared_ptr<T>` self pointer): `AddRef` stores `shared_from_this()` on the first reference, `Release` moves the pointer into a local, resets the member, then resets the local. Declaring a user `~SelfReference() {}` makes the compiler emit the out-of-line flagged destructor that the extab names.
- **Check the unwritten arguments of a call that should be constant.** `LoadScriptCode` passed `false` as the `background` argument of `LoadLocalizedFile`; the target forwards the `wait` parameter (`mr r6, r26`). A parameter that is saved into a non-volatile register but looks unused is used later.
- **Unreferenced inline layers are fine.** The unit emits all layers of the map destructor chain (map, map_base, tree, red_black_tree); the original object keeps only the outermost one. The linker strips the rest, so only the outermost layer needs the placeholder name.

### Open (not solved)

- `ScriptEffect` (5 functions at 99.8%): in the inline `GetWazaRangeEffectParam` the target holds the returned bool in r0 and the `ceil` conversion temporary in r3; ours swaps them. Tried: `bool`/`int`/`u8`/`BOOL` returns, a result local (before, after and at the head of the branch), reading `rParam.mEnable` back, statement permutations, a void helper tested through `param.mEnable`. None moves the result to r0.
- `ScriptCameraMotion` and `ScriptLightMotion` `getMotionLength`, camera `play`/`request`/`requestNext`: the target emits `addi r4,r1,8` after the stack-pop arithmetic and `lwzux`; ours hoists the address into the prologue and uses `add`/`lwzx`. The late form appears after any earlier call (sandbox with a prior `Foo()` call whose result is live), at the head of a function it did not appear for any source form tried (reference vs pointer engine, `PopString()` member, nested block, return-by-value `Pop`, compiler versions 1.0-1.5).
- `ScriptLayout` `NewLayout`: `LoadFileList(&e, p, 0, false, nullptr)` still compiles `mr r7,r6` where the target has `li r7,0` (F-161-3 constant merge). `#pragma peephole off` around the function removes it but changes the rest of the function (85%).
- `ScriptGame` near misses (`MakeMapID`, `ShowDlgPiiList`, `MapStatus_*`, `IncMapClearedNum`, `GetStageClearedNum`, `OpenScriptChoices`, `AddMessageBalloon`) are non-volatile register or stack-slot permutations. All 24 orders of the `MakeMapID` field assignments, declaration orders and local copies of the popped values were tried.
- `CPpuEnv`: `LoadScriptCode` 91%, `CollectGarbage` 96.3% (the end-node address `this+0x30` is hoisted into a register by us and recomputed at the store in the target), 27/33 functions at 100%. The 670-entry binding table and binding-name strings are not reconstructed.

## Session 3: CPpuEnv binding table

- The 670-entry binding table is now source in `src/game/CPpuEnv.cpp` (`gScriptNativeBindings`, entries `{function, name, argCount}` with a null first entry, count 0x29E in `.sdata`) with declarations in `include/game/ScriptBindings.h`. The built `.rodata` is byte-identical (8040 bytes) and the `.data` table strings are in the same order.
- CPpuEnv derives from `enable_shared_from_this<CPpuEnv>` (target RTTI string), not `<NPpu::CEnvironment>`. The two placeholder names at `0x801035D0` and `0x801055A0` were renamed accordingly.
- Linking the unit as `Matching` fails on `lbl_804B72F0`/`lbl_804B72F8` (NPpu::CEnvironment and NPpu::CCode RTTI referenced from NPpuVM); not renamed yet.

### Traps

- `git checkout <file>` to drop an experiment also drops all uncommitted work in that file. Commit before experimenting.
- `std::string("script.arc/") + pName` as a temporary (no named `directory`) raised `LoadScriptCode` from 91.0% to 92.2%.
- `script.arc/` is the last string in the target `.data`, after all RTTI name strings; ours places it before them. Moving `LoadScriptCode` later in the source did not change this.
- The target `weak_ptr<CPpuCode>` destructor (`0x80105F80`) calls `operator delete` where ours calls `release_weak`; unresolved.
- `CollectGarbage` (96.3%): tried a local root copy, direct `root_.second().node_` store, store reordering and a `reset_empty()` helper in `tree_imp::clear`; none stops the hoisting of `this+0x30`.
