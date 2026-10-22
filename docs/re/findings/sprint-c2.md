# Sprint c2: closer attempts (no unit flipped)

## Recipes

- ModelAim `__sinit`: `Complement(0.9f)` folds to 0.1f at compile time, but the target computes `1.0f - 0.9f` at run time (one `fsubs` shared by both follow rates). Routing the constant through a local, `f32 keepRate = 0.9f; Complement(keepRate)` twice, restores the two `.sdata2` constants (section 100%) and lifts `__sinit` from 91.5% to 98.85%. Only an f7/f8 register swap remains.
- NPpuLibrary `Format`: the target writes the terminator through the `%` spec end pointer (`pEnd[1]`) and keeps `pIn`, `pSpec` and `pEnd` in three registers. Rewriting the wide overload with `pEnd[1]` and a late `pIn = pEnd + 1` lowered it from 95.93% to 94.2%; the original form stays.

## Traps

- RenderPrimitive `SetupScreenProjection` (96.33%): pre-converted `width`/`height` locals in either order, inline field arguments, and a named zero local all change the f3/f4/f5 allocation but none reach the target (best 99.38% with width declared first, which swaps the two `lhz` loads).
- RenderUtilities `NormalizeSignedAngle` (99.56%): `adjusted -= 360.0f`, reusing `value`, and clamp-through-assignment forms were tried; all equal or worse.
- ScriptPiiPropData `method_pprGetWazaKouka` (94.53%): target keeps the engine in r31 and the popped type in r30; casting the engine up front, `const int`, split ternaries and an intermediate result local did not swap them.
- ScriptPiiManagement `NewPPD` (97.56%): only the hoisting of `addi r3, r31, 8` over the second callee-saved store differs; reordering the pop, a `const int sex`, or a cast local did not change it.
- CBattleRoyalBox `GetData` (89.33%): the target computes `(this + area * 8)` first and adds `mode * 0x28` as the index register; `mData[mode][area]` yields the reverse. A row pointer (`pRow[area]`) reaches 90%; flat and pointer-arithmetic forms are worse.
- CUIManager `DispatchFocusedInput` (98.61%): the target reloads the shared_ptr pointer from the stack for `->mFlags`; `(*p).mFlags` and `p.get()->mFlags` still reuse the register.
- ModelAim `Update`: the target emits `fabs f0, f0; frsp f3, f0`; `nw4r::math::FAbs`, `fabs`, ternary and double-typed variants do not produce it (FAbs is worse at 98.04%).
- TextLayout `WrapText`: declaration reordering of the state locals gains at most 0.3%; the target duplicates the tail block (`wordWidth = characterWidth; wordStart = i; state = Word`) into both overflow paths where the source merges it.
- PhotoSave `SaveScreenshotToRemoteMemory`: stack slot layout differs by 0x18 and 0x0c from the first local onward (the target has the exception-region records in a different order); this needs a restructure, not a tweak.
- CUIPiiSpec constructor: the only differences are register numbers (this in r28 versus r29, array ctor/dtor pointers swapped); the unit also has `.data` and `.sdata` unmatched.
