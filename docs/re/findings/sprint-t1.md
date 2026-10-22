# Sprint t1: CodeWarrior stack-frame slot order

Compiler: Wii/1.1 `mwcceppc -O4,p -inline auto -ipa file` with the game unit flags. The rules were measured in sandbox translation units (`scratchpad/t1/a*.cpp`, `s*.cpp`, `p*.cpp`, compiled with `sb.sh` and summarised with `off.py`) and then used to predict and fix real functions. Every fix was gated by the 100%-function count over the whole build (`scratchpad/e1/cnt100.py`) and landed through the full verify.

## The slot model

Every object that needs memory (an address-taken local, a class temporary, a by-value argument copy, an inlined function's parameter or local, an exception-specification frame from an inlined `throw()` function) is a frame object. CodeWarrior lays them out like this:

1. **Size buckets, smallest at the bottom.** The bucket is the object's size rounded up to a power of two: 1, 2, 4, 8, 16, 32 and so on. Buckets are placed in ascending order upwards from the bottom of the local area, so every `bool` and empty tag sits below every `int`, which sits below every 8-byte object, and a 12-byte `VEC3` shares the 16-byte bucket with a 16-byte struct. Alignment padding goes between buckets. Type, alignment and declaration order do not change the bucket.
2. **Within a bucket, the first-created object is highest.** In one function body, creation order is:
   - named locals, in declaration order, wherever they are declared (nested blocks and loop bodies included; scopes are not reused);
   - then expression temporaries, in source order; for a call, the by-value parameter copy is created before the temporary that initialises it;
   - then each inline expansion, in call order.
3. **Every inline level reverses its callee's objects.** When a callee is inlined, its objects (its own creation order, recursively including its own inlinees, followed by its by-value parameters from right to left) are added to the caller in reverse. So at depth 1 the parameters sit above the callee's locals with the first parameter highest, and at depth 2 that is mirrored. At odd depth below the function being compiled, a callee's objects appear upside down; at even depth they are in their original order. Auto-inlined non-`inline` functions, return-forwarding wrappers (`return f();`) and explicit `inline` functions all count as one level each. This is the mechanism behind r1 rule 16.
4. **Consequence: only parity matters, and only relative to the closest common inline ancestor.** Two objects from different expansions keep the order of those expansions in their common caller, flipped once per level between that caller and the function being compiled. Adding a level deeper inside one expansion (for example around a destructor) never moves it relative to its siblings.
5. **Named versus unnamed is itself a parity flip.** A named tag (`random_access_iterator_tag tag; f(tag);`) creates `[tag, copy]`; an unnamed one (`f(random_access_iterator_tag())`) creates `[copy, temporary]`. At the same depth they are mirror images, so "named at depth d" lays out like "unnamed at depth d plus or minus 1".

6. **Named locals versus temporaries.** A named local initialised from a call (`MapIndex index = MakeMapIndex(...);`) is created at its declaration, ahead of every temporary, so it sits at the top of its bucket. The same value used directly as an argument (`= MakeMapIndex(...)`) is a temporary and sits below the named locals. A target slot at the bottom of the bucket for a value that our source names usually means the original used it inline.
7. **A call's by-value argument copy comes before the expression that produces it.** For `GXSetTevColor(reg, color)` with a named `color`, the copy is below `color`; with the colour produced by an inline function or a temporary, the copy is created first and sits above it.

## Recipes

- **Find the pair and the direction.** Pure slot swaps show as rows that differ only in `0x..(r1)` or `0x..(r31)` offsets (`scratchpad/t1/cls.py <unit>` counts them per function). Read which object is which from the code that uses the slot: `lbz` from the uninitialised tag and `stb` to the argument copy for tag passing, the `addi r3, r31, X; bl __unexpected` handlers for exception-specification frames (one per inlined `~shared_ptr() throw()`, in code order).
- **Fix by changing one level between the objects and their common ancestor**, choosing the form that is more plausible source: an operator instead of a member call, a shared inline helper for repeated code, or a different wrapper in a library header. Then gate the header change across the build, because every caller at another depth flips too.
- **A parity mismatch can identify the original API.** If one header form fixes some callers and breaks others, the callers are at depths that differ by one in our source but not in the original. The string `assign` case below showed that the original sources used `operator=`, not `assign()`, and that the out-of-line function at `0x80072280` is `operator=` with `assign` inlined.
- **Repeated blocks that come out upside down point to an inline helper per block.** In CEffect `CreateEmitter` the target reverses the objects of each `switch` case separately while keeping the cases in order, which is exactly one inline level per case. The same reasoning found the shared `StartEndMotion` helper in errmsg and the helper boundary in `GetStrongestBP`.
- **Initialiser constants reveal declaration order.** Aggregate initialisers (`UIMenuItem item = {nullptr, MENU_RESUME};`) are pooled in `.sdata2` in declaration order, so the constant order in the target tells which local was declared first even when the slots do not.
- **A sandbox reproduces the real unit.** Compiling the real source file with the unit's flags (`scratchpad/t1/ub.py <unit source> <copy> <function>`) and grepping the `r1`/`r31` offsets confirms a prediction in about a second, without `ninja`.

## Fixes landed

| Change | Functions fixed |
| --- | --- |
| `map_compact_tree::insert_node_at` throws `length_error` directly, as `tree_imp` does (the extra `__throw_tree_length_error` level reversed the two `__error_string` flags) | 5: CLayout (2), CGameResult, CStatistic, CFileData `insert_node_at` |
| `string::operator+=(const charT*)` calls `replace_tagged` directly instead of through `append` | 4: CLayout `get_what` (2), CFileData, CUIList |
| Sources use `str = other` instead of `str.assign(other)` (CDividedGround, CPauseMenu, Message, NPpuVM); `0x80072280` renamed from `assign` to `__as__` (`string::operator=`) | 5: `__as__`, IPauseMenu `Create`, Message `FormatDate`, CEngine `AllocString`, `AllocWString` |
| `operator==`/`operator!=(const string&, const charT*)`, each calling `compare` directly; CPath `Parse` compares with them | 1: CPath `Parse` |
| CGameResult `KeepStronger` takes the candidate pointer; the callers pass `FindStrongestPii(...)` | 1: `GetStrongestBP` |
| CErrMsgFatalLoop: inline `StartEndMotion` shared by both pad loops | 1: `Prepare` |
| CGameResult `SetMaterialTexture` compares with `name != pTextureName` | 1 |
| homebutton: inline `GetNoticeColor(alpha)` produces the `GXSetTevColor` argument | 1: `DrawHomeButtonNotice` |
| `AppendLoadedFile` calls `insert(end(), file)` (one level shallower than `push_back`) | 1 |
| `__vec_constructor::assign(first, last, int2type<false>)` goes through `assign_dispatch` with a named tag | 1 (`vector<map>::operator=` in CFileData); about 15 other `vector::operator=` copies in SaveData, CStatistic, CUIMenu, PiiPropData and CPiiCollectionBox rose by 1 to 1.5 points |
| CGameSession `LoadFromSlot` assigns the pending map from the `MakeMapIndex` temporary | 1 |
| CEffect `CreateEmitter`: one inline member per effect type (`SetupLifeEmitter`, `SetupLifeFollowEmitter`) | slots fixed, 99.17% to 99.27%; two `fmuls` operand orders remain |
| CPath `ToString` reserves the device string before assigning it; the constructors move above `GetDeviceNameFor` (target text order). CPath is Matching | 1 and a unit flip |
| CUIManager `SetFocusedUI` calls an inline `NotifyFocusChanged(pUI, pPrevious)` for the old and new UI (rule 8: the inline's parameters rank above the caller's locals); `GetFocusedUI` and the four forwarding `Calc`/`Draw` overrides follow the target text order; the CUIManager `.data` split starts at its own `bad_function_call` string (0x803C0280, not BUI's). CUIManager is Matching | 1 and a unit flip |
| CUIPiiSpec `SetPii` reads the player through a named `CPlayer*` local (the inline `GetController` result otherwise takes r27) and tests `GetCappedLevel() < GetLevel()` for `CAP` | 1 |
| CPostEffect: the implicit destructor replaces the user-declared one, and `Draw`, `Calc`, `Hide`, `Show` and `BindCaptureTexture` are `inline` vtable-only overrides (declared `Show, Hide, Calc, Draw`; `BindCaptureTexture` defined before `Show`). This gives the target `.text` order (e1 rules 6 and 7) and the `@STRING@` literals that the target re-addresses with `lis` and two `addi`. CPostEffect is Matching | 2 and a unit flip |

## Open cases

- **Address-taken values kept in memory.** CLayout `StopMotion`, `FindPaneMP`, `FindAnimation` and `AdvancePaneMotions` have larger target frames (0x20 to 0x40 bytes more): the target keeps the map key (`pPane`) and the `it`/`end()` iterators in stack slots and reloads the key inside the inlined search loop, while ours promotes them to registers. Sandbox finding: any inlined `throw()` function in a function makes iterator temporaries passed by `const&` keep dead stores to their slots; it does not keep the key in memory. What makes the target reload the key was not found.
- **CPath `GetDeviceNameFor`.** `std::string(&letter, &letter + 1)` has the tag pair the wrong way round, while the same constructor in `Parse` matches. The target probably used a different constructor (for example pointer and count) that is one level shallower.
- **Message `InitMsgResList`.** The two `pod_type()` temporaries in `__vec_constructor(size_type)` are reversed. An inline `init(n)` level fixes the order in principle but changes inlining elsewhere in the function (99.99% to 99.72%).
- **ScriptGame `AddMessageBalloon`.** The three `CValue` slots of `PopMessageText`, `PopFloat` and `PopFloat` are reversed relative to `RegisterMessageBalloon`, which matches with the same calls. The target therefore has the three pops one level deeper (or reversed), and nested blocks do not change the order. No plausible helper was tried.
- **Structural slot differences.** `vector<UIMenuItem>::push_back`, CLayout `Attach`/`AttachLayout` and `__ct__7CLayout` have extra or missing frame objects (zero-stored tags, a `bool` temporary next to `insert`'s result), not swapped ones.

- **CPauseMenu `ShowMenu` (inlined into `Run`).** The `.sdata2` initialiser order shows that the original declares `resume`, `screenshot`, `remoteMemory`, `controllers`. With that order the four slots still come out rotated (screenshot highest, resume lowest) instead of the target's declaration order, so the block-local `item` copies also take part. Not solved.
- **CLayout constructor.** `mPixelScale = GetDefaultPixelScale()` gets an extra return object in our build (a `VEC2(x, y)` built at the bottom of the frame and copied), while the target builds the value straight into the assignment temporary with float loads and stores. `return static_cast<const VEC2&>(sDefaultPixelScale)` fixes the slots but copies with integer loads; adding NW4R's `VEC2(const _VEC2&)` brings back the extra object.
- **CDlgPiiSelector `vector::assign<list::const_iterator>`.** The target has the tag copy above the tag with no zero store. A named tag one inline level down would do it, but every such helper drags `assign_dispatch` into the inline budget and nothing is inlined.

- **CNoticeDisplay `FormatAreaName`.** Two tag pairs inside the inlined `basic_formatter` appends are reversed. `operator+=(const basic_string&)` must stay `append(str)` (CMsgBalloon has the out-of-line `__apl__` calling `append`), and `operator=(const charT*)` must stay `assign(s)` (CDlgMsgWin has both out of line), so the difference is in how the formatter is used, not in `<string>`.
- **Missing exception-specification frames.** PhotoSave `SaveScreenshotToRemoteMemory` and CPauseMenu `TakeScreenshot` have one more `__unexpected` handler in the target than in ours: an extra `shared_ptr` temporary is destroyed there. That is a source difference, not a slot order.

- **CUIPiiSpec `.data` order.** Every function now matches (only string labels differ), and the CUIPiiListItem split must end at 0x803AF3F8/0x804B75B8 so that its type_info stays with it. The flip still fails: the target emits `ICalcDraw`'s type_info right after the `enable_shared_from_this<BUI>` name and then the `shared_ptr_deleter_common` vtable; ours emits the vtable and its name first. This comes from `<memory>` or BUI emission order, not from CUIPiiSpec.

- **CShadowCamera `SetupShadowModel`.** The four 4-byte temporaries are in plain source order in the target (the `DynamicCast` type object highest, then the `ResDic`, then the `color` and `mat` argument copies, right to left). Ours puts the two argument copies above the inline objects. Accessor calls in the argument list and an `inline` helper (which CodeWarrior then inlines) did not help.
- **resetmgr `Update`.** In `NotifyAndBlankScreen` the target keeps the loop iterator in r30 and the decremented copy in r29; ours swaps them. A `while` loop, `(--current)->Call`, a temporary copy, a nested inline helper and `std::reverse_iterator` did not reproduce it.
- **CUIMenu `OnInput`.** The target computes `mCursorRow = selected % mRowCount` from the saved register (r25) and loads `mRowCount` into r3; ours reuses the call result in r3. Using `candidate` instead and an inline `SelectIfSelectable` helper did not help (the helper drops to 93%).
- **CPiiPersonalData `Import`.** The target loads the low creation-time word first and stores `mRandomSeed` between the flags and the two time halves. Reordering the assignments and writing the time expression inline or with swapped operands did not reproduce the registers.

## Traps

- **unitdiff's left column is the target.** It is easy to read the direction backwards; the left column carries the original `lbl_` names.
- **Not every `r31` offset is a frame slot.** In functions without `mr r31, r1`, `r31` is often the base of the string pool, so `addi rX, r31, N` differences are string layout (e1), not frame order. `scratchpad/t1/cls.py` only counts `r31` as the frame when the function sets it from `r1`.
- **Out-of-line library copies pin the header.** Before moving a level in `<string>` or `<vector>`, check whether the target has the wrapper out of line (`__as__`, `__apl__`, `assign`, `append`). Those copies show which function calls which, and a header change that inlines them away drops whole functions from the build (the link then fails).
- **A slot swap is not cosmetic.** One swapped pair makes a function non-matching, and the same header form is shared by many units. Measure a library-header change with the full 100%-function count, not only the unit at hand.
- **A literal re-addressed with `lis` once and `addi` twice is an object of its own.** Normal pooled literals keep the full address in one register. When the target keeps only the high half and adds the low half at each use, the literal is a separate object: an `@STRING@` literal of an inline function (CPostEffect `Show`) or a static array such as `__FUNCTION__` (CLayout `get_what`). Look for an `inline` member before inventing an array.
- **Check the `.data` split start when a flip only shifts data.** CUIManager's split started at the second of its own strings, and CUIPiiListItem's ended before its own type_info, so the following unit was misaligned. Dump the target bytes around the boundary before changing source.
