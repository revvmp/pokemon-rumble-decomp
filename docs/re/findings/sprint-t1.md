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

## Recipes

- **Find the pair and the direction.** Pure slot swaps show as rows that differ only in `0x..(r1)` or `0x..(r31)` offsets (`scratchpad/t1/cls.py <unit>` counts them per function). Read which object is which from the code that uses the slot: `lbz` from the uninitialised tag and `stb` to the argument copy for tag passing, the `addi r3, r31, X; bl __unexpected` handlers for exception-specification frames (one per inlined `~shared_ptr() throw()`, in code order).
- **Fix by changing one level between the objects and their common ancestor**, choosing the form that is more plausible source: an operator instead of a member call, a shared inline helper for repeated code, or a different wrapper in a library header. Then gate the header change across the build, because every caller at another depth flips too.
- **A parity mismatch can identify the original API.** If one header form fixes some callers and breaks others, the callers are at depths that differ by one in our source but not in the original. The string `assign` case below showed that the original sources used `operator=`, not `assign()`, and that the out-of-line function at `0x80072280` is `operator=` with `assign` inlined.
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

## Open cases

- **Address-taken values kept in memory.** CLayout `StopMotion`, `FindPaneMP`, `FindAnimation` and `AdvancePaneMotions` have larger target frames (0x20 to 0x40 bytes more): the target keeps the map key (`pPane`) and the `it`/`end()` iterators in stack slots and reloads the key inside the inlined search loop, while ours promotes them to registers. Sandbox finding: any inlined `throw()` function in a function makes iterator temporaries passed by `const&` keep dead stores to their slots; it does not keep the key in memory. What makes the target reload the key was not found.
- **CPath `GetDeviceNameFor`.** `std::string(&letter, &letter + 1)` has the tag pair the wrong way round, while the same constructor in `Parse` matches. The target probably used a different constructor (for example pointer and count) that is one level shallower.
- **Message `InitMsgResList`.** The two `pod_type()` temporaries in `__vec_constructor(size_type)` are reversed. An inline `init(n)` level fixes the order in principle but changes inlining elsewhere in the function (99.99% to 99.72%).
- **ScriptGame `AddMessageBalloon`.** The three `CValue` slots of `PopMessageText`, `PopFloat` and `PopFloat` are reversed relative to `RegisterMessageBalloon`, which matches with the same calls. The target therefore has the three pops one level deeper (or reversed), and nested blocks do not change the order. No plausible helper was tried.
- **Structural slot differences.** `vector<UIMenuItem>::push_back`, CLayout `Attach`/`AttachLayout` and `__ct__7CLayout` have extra or missing frame objects (zero-stored tags, a `bool` temporary next to `insert`'s result), not swapped ones.

## Traps

- **unitdiff's left column is the target.** It is easy to read the direction backwards; the left column carries the original `lbl_` names.
- **A slot swap is not cosmetic.** One swapped pair makes a function non-matching, and the same header form is shared by many units. Measure a library-header change with the full 100%-function count, not only the unit at hand.
