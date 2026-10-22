# Sprint tZ findings (long-tail units)

## Fixes

- **Two-placeholder binds take their call arguments through a reference tuple.** CPii's `function_imp<void (*)(CModel*, const shared_ptr<CMotionProgram>&), bound_func<...ph<1>,ph<2>>>::operator()` spills the first argument and stores its address and the second reference into a stack tuple. `bound_func<R,F,tuple<T,ph<1>,ph<2>>>::operator()` now builds `tuple<A&,B&>` and resolves both placeholders from it, matching the existing one-placeholder specializations. The two-element tuple constructor takes `tuple_argument` types so reference elements compile. No other unit changed (checked with the full-build 100% count).
- **A `const` local resource accessor lets loads be cached.** CPii `ApplyTextureVariant` re-read `info.ref().namePltt` after the first test; declaring `info` as `const nw4r::g3d::ResTexPlttInfo` gives the target's single load (99.41% to 100%).
- **Assigning a default in an `if` lays out a conditional value like the target.** CPii `LoadModel` uses `if (!pShadowName) { pShadowName = "..."; }` before the call; a ternary in the argument placed the default block out of line. `sizeScale` is a separate local multiplied by the model scale.
- **A member read in a `const` function can still need reloading.** CPiiTex `GetTexMap` reloads `mTexture` after each store to the returned `TexMap`. Declaring the member `mutable` reproduces this (88.8% to 100%). `const_cast` of `this` does not.
- **`#pragma scheduling off` around CPpuEngine `GetWorld`** keeps the owner load after the prologue store. `mutable` on `mOwnerObj` does not help there.
- **Weak destructor misnamed in symbols.** `0x800BC370` in CGameSession is the `alloc_ptr<tree_imp<const void*>::node>` destructor, not `weak_ptr<ISession>`; renaming it matched the function.

## Traps

- `var.py`-style scripts that read a source in text mode and write it back convert line endings on Windows; harmless with `core.autocrlf`, but compare with `newline=''` when building replacement blocks from CRLF files.

## Unsolved (unit, function, match, cause)

- snd_SeqSound `SeqSound::SeqSound` 94.87%: target stores both volatile flags (0x288, 0x289) before the other members. Volatile combinations, mem-initializer order and body assignment do not reproduce it.
- ut_TextWriterBase `PrintToDisplayListImpl<char/wchar_t>` 97.9%: the target calls `~CharStrmReader` at the end and keeps `GetCharStrmReader` weak in this unit; an out-of-class inline destructor changes nothing.
- CGameSession `vector<SD_PiiPersonalData::Data>` destructor 96.77%, `~SessionSnapshot` 97.73%, CPii `swap<vector<Data>>` 97.5%: the target's vector of this type has one more base-destructor level (five `beq`), like `vector<short>`/`vector<unsigned short>`, while `UIMenuItem`/`MTX34` vectors have the shorter chain. Cause not found.
- CPiiTex `InitTexture` 97.82% and the three missing weak functions: deleting a `COffScreenTexture` calls `CaptureTexture`'s deleting destructor directly (`__dt__14CaptureTexture(p, 1)`); our build inlines the implicit destructor. Variants of the empty derived class (ctor, inline dtor, struct, private base) change nothing.
- CPpuEngine `Load` 97.76%: target passes the `int` mode to `LoadScriptCode(..., bool)` without a bool conversion. `LoadPlacement` 98.87%: target has two extra value-initialised tag bytes in the `reserve` expansion.
- CGameSession `GetSnapshot` 88.24%, `AddMoney` 96.86%, `RunStage` 99.60% (loop counter/pointer register swap), `CanAddPPD...` 99.01%, `Calc` 93.26%: register and layout differences; local-order and ternary variants tried.
- CRemoteMemory (six functions), ef DrawFree/DrawDirectional, ParticleManager `Calc`, animcurve: same partly unoptimised look as recorded in sprint-tC (branchy bool assignment, binary switch trees). `int texCoord = 0; if (...) texCoord = 1;` raised DrawFree `Draw` to 97.50%; pragma sweeps (`peephole`, `optimization_level`, `global_optimizer`, `opt_*`) do not reproduce the rest.
- ut_ArchiveFontBase `ConstructOpAnalyzeGLGR` 99.29%, puh_msc_blk `pread`/`pwrite` 99.4%, pdm_mbr 89%, TMC JPEG setup/convert: register colouring only; declaration permutations and expression shapes tried.
