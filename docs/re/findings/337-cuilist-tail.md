# F-337-6: CUIList local destructor reconstruction

2026-11-04, baseline `d14a20ca1`, target `wpse-v1`, Wii/1.1 with configured game flags.

The remaining function at `0x801DC380`, size `0xC4`, was previously inferred as a local copy of `std::vector<std::tr1::shared_ptr<CLayout>>::~vector`. That inferred name is not original-symbol evidence. The header's vector destructor emits two extra redundant null branches and scores 95.918365%. Explicit specialization, explicit instantiation, and per-instantiation optimizer pragmas did not remove those branches.

Explicit instantiation of the flatter `std::__vec_deleter<std::tr1::shared_ptr<CLayout>, std::allocator<std::tr1::shared_ptr<CLayout>>>::~__vec_deleter` reproduces every instruction and relocation. Its global counterpart already exists at `0x80084C70`. This supports the flat storage destructor behavior but does not prove the original source type or why this local copy was retained.

## Documented fake

`CLayoutVectorStorage`, declared in `include/game/CUIListLayoutStorage.h`, reproduces the flat destructor with named storage fields, the same allocator, reverse element destruction, value-initialized non-POD dispatch tag, and deallocation. The destructor is explicitly retained with `#pragma force_active on`. It is a D-0016 surrogate for the unused local copy, not a claim that the developers wrote this class.

| Offset | Field | Size | Evidence |
| --- | --- | --- | --- |
| `0x0` | `mpData`, shared-pointer element array | `4` | First pointer load and final deallocation |
| `0x4` | `mSize`, unsigned long | `4` | Loop count, subtraction and store |
| `0x8` | `mCapacity`, compressed capacity and empty allocator | `4` | Standard allocator storage layout; capacity is ignored by deallocation |

Class size is `0xC`. Each element is the existing eight-byte `shared_ptr<CLayout>`. The destructor first checks the object pointer, checks its data pointer, subtracts the old count from the size, destroys elements from last to first, releases the allocation, and honors CodeWarrior's deleting-destructor flag. No original code bytes or assembly are embedded.

`powerpc-eabi-objdump -r build/wpse-v1/obj/game/CUIList.o` shows no code or exception-action relocation to the local suffixed destructor name. Its sole local-name reference is its own `extabindex` entry at offset `0xB4`. The unit's exception-action entries at `extab` offsets `0x200` and `0x358` reference the earlier global vector destructor instead. The surrogate preserves those existing calls and only supplies the separate local code extent.

## Validation

A diagnostic copy of the split object had only its local destructor symbol renamed with `powerpc-eabi-objcopy --redefine-sym`; original objects and frozen references were not changed. `objdiff-cli diff -1 <diagnostic-target> -2 <compiled-source>` gives **31/31 functions at 100%**, including the 196-byte surrogate, after a direct compile of the final main-checkout source. Scratch inputs and outputs are under ignored `tools/local/session/337-candidates/`.

Strong method definitions now follow target order: both `Load` overloads precede `GetLayoutCount`, and `OnInput` precedes `Calc` and `Draw`. Their instructions remain unchanged. Full link acceptance and data/emission-order verification are handled by the integrating agent; the direct object result alone does not authorize a Matching claim.

## Link investigation

The first trial source link succeeded but failed the full SHA comparison. It retained an extra allocator-reference layout deleter and placed the weak callback functions after `weak_ptr<IUIListItem>` instead of before them. Including `<memory>` first reproduces the target weak-function ordering without changing any function instructions.

The original `CUIList::Load` exception actions reference the split-buffer destructor at `0x80083DC0` and its allocator-reference base destructor at `0x80084BA0`. Those addresses had been inferred as CDlgInfo `Page` helpers. Their pairing is also visible in CDlgInfo's own exception actions. The other pair is `0x80083AE0` with `0x80084C70`. The integrating agent approved a diagnostic name swap to associate the CUIList-used pair with `shared_ptr<CLayout>` and the other pair with `shared_ptr<Page>`, including the missing allocator-reference qualification. This classification remains subject to whole-build validation and CDlgInfo source reconstruction.

The target also contains the `shared_ptr_deleter<function<void(BUI*)>>` and matching `default_delete` RTTI strings that the baseline source does not emit. Providing the existing inline `CCallbackListOwner::Add` definition at the end, as done in the already-matching CUIMenuSoundCallback unit, restores those instantiations while the original call stays out of line. The scratch candidate retains 31/31 function matches and produces `0x7FE` bytes of raw `.data` against the target's aligned `0x800` extent. Full link validation remains required.

The next link placed every CUIList function at its original address and eliminated the extra allocator-reference destructor. Two further data-order differences were resolved in source: `OnFocus` must be a non-inline declaration, placing `normal_hover` and `hover_normal` in the early literal pool; and `<stdexcept>` must precede the implicit `<functional>` inclusion, placing `bad_function_call` RTTI before the three logic-error classes. With those changes, every printable `.data` string has the exact target content, order and offset, and all 31 functions remain exact in the direct object comparison.

## Accepted full build

The integrating agent then ran the real build with `game/CUIList.cpp` marked `Matching`: `ninja` exited 0 and its SHA check passed. `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` returned PASS for the complete `main.dol`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The Matching selection is retained. This accepts the four EH symbol corrections and the complete data layout, beyond the earlier object-only diagnostics.

The unit contributes 31 matched functions and its `0x2600`-byte `.text` extent is now linked from source. The separate release package and the other unfinished units are outside this acceptance result. The owner requested direct work on main; no issue branch or PR was used for the accepted changes.

## Traps

A local duplicate's inferred template name can imply the wrong destructor depth. Here the flat base destructor matches exactly while the outer vector destructor does not. Check the actual null-branch depth and the local relocation users before changing shared vector headers. `#pragma internal` around an explicit template destructor instantiation did not change its weak symbol binding, so it did not provide a distinct retained local copy.
