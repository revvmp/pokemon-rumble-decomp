# Script camera emission order (#236)

Target `wpse-v1`. Issue #236 lists 27 functions in `.text` `0x80134BD0`–`0x801352EC`, the end of `game/ScriptCamera.cpp` ([F-231-1](231-script-camera-unit.md#f-231-1-unit-boundaries)). PR #1008 (#235) wrote all of them, and objdiff shows each at 100% ([F-235-4](235-script-camera-adjust.md#f-235-4-what-remains)). The unit stays `NonMatching`: linked as `Matching`, `construct_at_end` and its tagged overload come out eleven positions late. This finding explains how the compiler orders the end of a unit, corrects two library headers with that evidence, and narrows what is left to one function pair.

## F-236-1: how Wii/1.1 orders deferred functions under `-ipa file`

Measured with scratch translation units and `game/ScriptCamera.cpp` itself, compiled with `cflags_game` and compared by `nm -n`:

1. **Main phase.** A function defined in the file is emitted, followed depth first by the inline and template functions it calls whose bodies are available.
2. **Pass over templates (A).** At the end of the unit the compiler walks the class and function templates in reverse order of their first declaration; a forward declaration fixes the position. Within a template it walks instances newest first, and members in declaration order. It emits every member already marked as used. A function emitted in this pass only marks its callees; a callee whose template the pass has already passed waits for the next pass.
3. **Deferred inline functions (B).** After the pass, in-class (inline) members that were marked during the pass are emitted. They come in groups, newest marking function first, each group in the order its function names them; their own inline callees follow depth first. Out-of-line members marked here wait for the next template pass.
4. **Vtable destructors (C).** Implicit virtual destructors come after B.
5. The cycle repeats until nothing is marked.

A callee whose body was already instantiated before its caller is emitted (an *instantiated* callee) is emitted directly after that caller, in any phase. Constructors and destructors referenced from main-phase code are instantiated at once with their in-class callees: the out-of-line `~__vec_deleter` is followed by `clear` and `erase_at_end`. Other out-of-line members, such as `reallocate`, are instantiated only when the template pass reaches them. An unused non-template inline function instantiates the in-class template functions it calls.

Exception ranges show a second effect. A call is left out of a cleanup range only when the compiler can prove that the callee does not throw. It proves this by reading the callee's body, recursively. An `extern "C"` function counts as throwing unless it is declared `__attribute__((nothrow))`; `throw()` is ignored.

## F-236-2: header corrections

**`<vector>`.** The original tail after `reallocate` is: `construct_at_end` (both), the five `CStageCamera` deleter functions that the pass emits, `__vec_deleter(allocator)`, `allocate(n)`, `~shared_ptr_deleter<CStageCamera>`, `allocate(n, tag)`. Under F-236-1 this needs `__vec_deleter` to be passed before `__vec_constructor`, the allocator constructor and `allocate(n)` to be inline members marked by `reallocate` (phase B), and `allocate(n, tag)` to stay out of line (the next pass, after the destructors). `include/vector` therefore forward-declares `__vec_constructor` before `__vec_deleter` and defines the allocator constructor and `allocate(n)` inside the class. With this, the allocator constructor, `allocate(n)`, the deleter destructor and `allocate(n, tag)` land in the original relative order. The function bodies are unchanged.

**`<string.h>`.** The original `extab` entry of `reallocate` (`0x8002E1E8`) has one call site in its cleanup range, the return from `allocate(n)` at offset 0x40 (`00000040 0000 0010`). Ours also covered `end`, `begin` and `construct_at_end` (`00000040 0009 0010`), because `construct_at_end` calls `memcpy` and `memset`. Declaring the C string functions nothrow in C++, as MSL's `_MSL_CANT_THROW` does, reproduces the original entry. `include/string.h` now defines `_MSL_CANT_THROW` as `__attribute__((nothrow))` under `__cplusplus` and applies it to every declaration. No other unit's objdiff measures change, and the full DOL verifies.

## F-236-3: what remains

With both corrections, the only difference in the linked unit is that `construct_at_end(void**, void**)` and its tagged overload follow the deleter group instead of directly following `reallocate`. In the original they are emitted directly after `reallocate`, which by F-236-1 means their bodies were already instantiated when `reallocate` was emitted, while the allocator constructor and `allocate(n)` were not.

Proof: an experiment that adds only an unused inline function `void f(std::__vec_deleter<void*, std::allocator<void*> >* p) { p->construct_at_end(0, 0); }` before `#pragma dont_inline on` links `ScriptCamera.cpp` as `Matching`, and `configure.py`, `ninja` and `tools/verify/verify.py` exit 0. That function is a probe, not source: nothing in the executable shows which original code instantiated `construct_at_end`. `0x80134E80` has one caller, `reallocate`, so no unit emits another copy of a pointer-vector range construction. The instantiating code must be inline code that the game never emits, such as an unused inline function in a game header that copies or assigns a pointer vector. `CDividedGround`, `BWorld` and `CPii`, all included here, hold `vector<const char*>` members (F-235-2). Until that code is identified, the unit stays `NonMatching`.

`AdjustPii` needs no change. Its 99.96% is the pooled `-1.0f` placement before linking (F-235-4), and the linked `.sdata2` matches.

Ruled out, each compiled and compared:

| Change | Result |
| --- | --- |
| Allocator constructor, `allocate(n)` and `construct_at_end` in or out of the class, without the forward declaration | Same order as before; all three follow `reallocate` |
| Forward declaration with out-of-line allocator constructor and `allocate(n)` | Both move after the destructors, together with `allocate(n, tag)` |
| `construct_at_end` (one or both overloads) out of line, with the forward declaration | Moves after `allocate(n, tag)` |
| `construct_at_end` as a member template over the iterator type | Same position, different symbol |
| `#include <vector>` first in the file | No change; `<memory>` declaration order does not move the deleter group ahead of `reallocate` |
| `#pragma dont_inline on` before the includes | No change |
| Wii/1.0 and Wii/1.3 with the corrected headers | Same order as Wii/1.1 |
| A base class holding `data_`, `size_` and `construct_at_end`, declared before the forward declaration | Rejected by evidence: the constructors at `0x80108A10` and `0x80135150` store `data_` and `size_` in their initializer list before the `compressed_pair` constructor, so they are direct members |

The real push-back path differs from `include/vector`: `push_back` (`0x80108A80`) reaches `0x8010A8A0`, which tries an in-place expansion (`0x8010B8A0`, always false) and otherwise reallocates through a split buffer (`0x8010BF40`). It never calls `reallocate`. Rewriting it does not change this unit's retained functions, because none of its functions reference `__vec_deleter<void*, allocator<void*>>::construct_at_end`.

## Method

Scratch translation units compiled with the pinned Wii/1.1, 1.0 and 1.3 compilers and `cflags_game`; a script that compiles `src/game/ScriptCamera.cpp` against an overriding include directory and compares the retained function order with the split object; GNU objdump, nm and readelf on the compiled and original objects and `build/wpse-v1/main.elf`; `configure.py`, `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **A forward declaration moves a template in the deferred pass.** The pass position comes from a template's first declaration, so a forward declaration of a derived template before its base reverses their order.
- **In-class and out-of-line members wait in different places.** A member marked after its template has been passed is emitted after the pass if it is inline, but only in the next pass, after the vtable destructors, if it is out of line.
- **A function is emitted early when its body was instantiated early.** Destructors and constructors used by main-phase code, and template functions called from unused inline functions, are emitted right after their first emitted caller. A function marked only from the end-of-unit pass is not.
- **`extern "C"` callees break nothrow proofs.** Without `__attribute__((nothrow))` on `memcpy` and `memset`, every template that copies memory widens the cleanup ranges of its callers. Check `extab` call-site ranges when a unit's code matches but its exception data does not.
- **`python -` hangs in Git Bash.** Write scratch scripts to files.
