# Issue 200: CSaveManager.cpp, second block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #200 lists 26 functions in `.text` `0x80108B90`-`0x801090E4`, the second block of the eight-issue file started in [F-199-1](199-csavemanager.md). No decompiler output was used.

## F-200-1: the 26 functions of #200 reconstructed

All 26 functions pair with the original at 100% in `unitdiff.py` except `StartWork` (99.64%), whose only differences are the two relocation names of its pooled constants (the 12-byte member-pointer constant `0x803AD2AC` and the fiber name string `0x803AD2B8`, which the unit does not yet own). The unit stays `NonMatching` because it spans #199 to #206. `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged; no other unit's fuzzy score changed.

| Address | Function |
| --- | --- |
| `0x80108B90`-`0x80108BD0` | `deque::empty`, `__deque_imp::empty` and the `queue`, `deque`, `__deque_imp` `front` forwarders |
| `0x80108BE0`-`0x80108C70` | `__deque_imp::first` (`buf()[start_ / block_size][start_ % block_size]`), `buf`, and the `operator[]` chain `__cdeque<T*>` to `__cdeque_pointer` to `__cdeque_buf<void*>` |
| `0x80108CD0`-`0x80108D00` | `queue::pop`, `deque::pop_front`, `__deque_imp::pop_front()` and `pop_front(size_type)` |
| `0x80108D30` | `CSaveManager::WaitIdle` |
| `0x80108D90`, `0x80108DB0` | `shared_ptr<CFiber>` bool conversion and `operator->` |
| `0x80108DC0` | `CSaveManager::StartWork`, the only referrer of `Work` |
| `0x80108EA0` | `CFiber::CFiber(const char*, const function<void()>&, u32) throw()` |
| `0x80108F50`-`0x801090E0` | `shared_ptr<CFiber>` `reset(CFiber*)`, its pointer constructor, `move_ptr`, `compressed_pair`, `CompressedPairStorage` and `default_delete` constructors |

`StartWork` is `mFiber.reset(new CFiber("CSaveManager", std::tr1::bind(&CSaveManager::Work, this, type), 0x8000))`. Binding the function as a temporary of the full expression, not as a named local, reproduces the stack layout (function at 8, member pointer at 0x14).

## F-200-2: construction contracts found by the match

- **The CFiber constructor takes a name.** The original passes `.data` `0x803AD2B8` (`"CSaveManager"`) in `r4` and the body and stack size in `r5`, `r6`. The name is not stored. `include/game/CFiber.h` now declares `CFiber(const char* pName, const function<void()>& rBody, u32 stackSize) throw()`. The earlier two-argument form had no caller.
- **`function<void()>` constructors take a trailing defaulted argument.** The original constructs `mOnSuspend` and `mOnResume` with `li r4, 0`, and the converting constructor from `bound_func` with `li r5, 0`. `include/functional` gives both an unused `detail::unused_argument*` parameter defaulting to `nullptr`; the pointee type is a guess, only the argument position and null value are evidenced. No other Matching unit constructs a `function<void()>` through these constructors.
- **Fully bound member functions.** `bind(R (C::*)(A), T, A)` returns `bound_func<R, mem_fn_1<R, C, A>, tuple<T, A>>` and a new `bound_func<R, F, tuple<T0, T1>>` calls the member with both stored values. `tuple<T, nat>` needed its own specialisation, otherwise the one-element `bind` of `CNoticeDisplay.cpp` selected the two-element class.
- **`deque` front and pop.** `__deque_imp::front()` forwards to `first()`; `pop_front()` calls `pop_front(1)`, which dispatches on `int2type<is_POD<T>::value>`. `WorkType` is not POD so it calls the `int2type<false>` helper at `0x8010A5D0`, which is declared in `include/deque` and owned by #202 (it is not defined, so no unit emits it).

## F-200-3: force_active moved from Work to its callers

`StartWork` references `Work`, so the pragma around `Work` is removed. `WaitIdle` and `StartWork` are still unreferenced because the `ISaveManager` slots belong to later issues, and the unreferenced anonymous-namespace class would be dropped entirely, so `#pragma force_active` now wraps those two. Superseded by [F-201-3](201-csavemanager-3.md#f-201-3-force_active).

## Traps

- **Identical weak instances in two units collide at link time.** `__vc__Q23std38__cdeque_buf<Pv,Q23std13allocator<Pv>>FUl` exists at `0x80108C70` and `0x80134BD0` (ScriptCamera). The first rename produced `multiply-defined` from `mwldeppc`. Mark the later copy `scope:local`, the convention used for the other duplicated names; marking both `scope:weak` made the linker fold them and shortened `main.dol` by 64 bytes (first difference at header byte `0x1F`).
- **A reference-tier name can sit on the wrong copy.** `cap__Q23std38__cdeque_buf<Pv,Q23std13allocator<Pv>>Fv` was on `0x800FC080`, but the `operator[]` of this unit calls `0x800FC0C0`, whose neighbour is `CompressedPairStorage<Ul,allocator<Pv>,2>::first`. The name moved to `0x800FC0C0` and `0x800FC080` is `fn_800FC080` again; four such 8-byte `cap`-like functions exist at `0x800FC060`, `0x800FC080`, `0x800FC0A0`, `0x800FC0C0` and only evidence from callers distinguishes them.
- **A hidden constructor argument shows up as an unexplained register.** A vtable `lis` into `r4` before a constructor means `r4` is a real, unused parameter. Look at the caller's `r4` before assuming the constructor takes `(this, a, b)`.
- **Default arguments are materialised by the caller.** Two `li r4, 0` before library constructors meant a defaulted pointer parameter, not an explicit `(0)` initialiser.
- **A new `tuple<T0, T1>` specialisation also matches `tuple<T>`.** `tuple<T>` is `tuple<T, nat, ...>`, so a two-element partial specialisation captured the one-element `bound_func` of `CNoticeDisplay.cpp` and broke its build. Add an explicit `tuple<T, nat>` specialisation.
