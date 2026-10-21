# Issue 201: CSaveManager.cpp, third block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #201 lists 27 functions in `.text` `0x801090F0`-`0x80109754`, the third block of the file started in [F-199-1](199-csavemanager.md) and continued in [F-200-1](200-csavemanager-2.md). No decompiler output was used.

## F-201-1: the 27 functions of #201 reconstructed

All 27 functions pair with the original at 100% in `unitdiff.py`, with one exception that is not a code difference: the `function_imp` constructor (`0x801094B0`) is 99.52% because its two vtable relocations name `.data` `0x803AD380`, the `function_imp` vtable that #202 owns. `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged; no other unit's fuzzy score changed. The unit stays `NonMatching` because it spans #199 to #206.

| Address | Function |
| --- | --- |
| `0x801090F0`-`0x801091C0` | `shared_ptr_deleter<CFiber>` constructor, `move_ptr::release`, `CompressedPairStorage::first`, `enable_from_this` and `swap`, emitted by `reset(new CFiber)` and counted here |
| `0x801091F0` | `bind(R (C::*)(A), T, A)` for `CSaveManager::Work` |
| `0x80109270`-`0x80109360` | `mem_fn(R (C::*)(A))`, `mem_fn_1` constructor and copy constructor, `tuple<T, A>` constructor and copy constructor, `bound_func` constructor |
| `0x80109380`, `0x80109460`, `0x801094B0` | `function<void()>` converting constructor from the bound function, `bound_func` copy constructor, `function_imp` constructor |
| `0x80109510`-`0x801096C0` | `LoadSync`, `RequestWork`, the `queue::push` chain, `RequestSave`, `RequestSaveWithoutBackup`, `RequestSaveCommitOnly`, `IsIdle`, `RequestDelete`, `Calc` |

`LoadSync` is `WaitIdle(); StartWork(WORK_LOAD); WaitIdle();`. `RequestWork(WorkType)` (descriptive name, medium confidence) queues the work when the fiber is still running and starts it otherwise; the four request slots call it with save (2), save without backup (3), save commit only (4) and delete (5). `IsIdle` is `!mFiber || mFiber->IsFinished()`. `Calc` resumes the fiber and then calls `Calc` on the layout and the message window when each exists. The vtable order, `RequestDelete` before `IsIdle`, already matched `ISaveManager.h`.

## F-201-2: construction contracts found by the match

- **`tuple<T0, T1>` takes its elements by const reference.** The original loads both values through pointers in `r4` and `r5`, so `bind` spills `value` and `argument` to the stack and passes their addresses. By-value constructor parameters changed the frame from `0x40` to `0x30` bytes. `include/tuple` now declares `tuple(const T0&, const T1&)`.
- **`bind` builds the member function through a free `mem_fn`.** A `mem_fn_1` temporary constructed inside `bind` reuses the constructor's returned `this` (`mr r4, r3`); the original reloads the temporary's address. The original has a separate non-constructor function at `0x80109270` that copies its by-value pointer-to-member argument to the stack and constructs the `mem_fn_1` in the return slot. `include/functional` adds `std::tr1::mem_fn(R (C::*)(A))` and `bind` calls it. The namespace in the mangled name `mem_fn<...>__Q23std3tr1F...` is a guess; the instruction stream does not depend on it.
- **`queue::push` is a four-function chain.** `queue::push`, `deque::push_back` and `__deque_imp::push_back(const T&)` are one-instruction forwarders. The fourth (`0x801095F0`, here `append`) creates the `int2type<false>` tag on the stack and calls the out-of-line `push_back(const T&, int2type<false>)` at `0x8010A260`, which #202 owns and `include/deque` only declares. The name `append` is descriptive; only the frame shape is evidenced.
- **Request slots tail-call the shared helper.** `RequestSave` and its siblings compile to `li r4, n; b RequestWork`, which requires the helper to be a non-virtual member taking `WorkType` by value.

## F-201-3: force_active

The virtual slots are unreferenced until the constructor and vtable (#202) exist, and the anonymous-namespace class would be dropped entirely, so `#pragma force_active` now wraps `LoadSync`, `RequestWork`, the request slots, `IsIdle` and `Calc`. `WaitIdle` and `StartWork` lose it because those slots reference them. Remove the pragma when the constructor and destructor are written.

## Traps

- **A call that does not reuse the callee's returned `r3` is not a constructor.** The original reloads the `mem_fn_1` temporary's address into `r4` after a call, where a constructor call let the compiler pass the returned `this`. A function that builds an object into its return slot (`mem_fn`) produces the reload.
- **A by-value class parameter is copied by the caller, once per call.** In `0x80109270` the pointer-to-member argument is copied to a new stack slot before it is passed on to the `mem_fn_1` constructor, so a function that only forwards a by-value argument gains a copy and a larger frame.
- **Rename outside callees to the mangled name of your declaration.** `fn_8009D1C0` became `function_base<void (*)()>::function_base()` so the `function_imp` constructor's call pairs.
