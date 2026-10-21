# CStrapReminder and CTextWorldObjStat (#211)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #211 lists 24 functions in `.text` `0x80118180`-`0x80119018`. The block holds two units; `0x80118140` (a 64-byte deleting destructor of an unrelated empty interface) belongs to #210.

## F-211-1: unit boundaries

| Unit | `extab` | `extabindex` | `.text` | `.data` | `.sdata` | `.sdata2` |
| --- | --- | --- | --- | --- | --- | --- |
| `game/CStrapReminder.cpp` | `0x80029AA4`-`0x80029D7C` | `0x8004BFE4`-`0x8004C08C` | `0x80118180`-`0x80118C00` | `0x803AE0D8`-`0x803AE3A8` | `0x804B73A0`-`0x804B73D8` | none |
| `game/CTextWorldObjStat.cpp` | `0x80029D7C`-`0x80029E78` | `0x8004C08C`-`0x8004C0BC` | `0x80118C00`-`0x80119020` | `0x803AE3A8`-`0x803AE428` | `0x804B73D8`-`0x804B73F0` | `0x804BA128`-`0x804BA138` |

`0x80119020` starts `CTitleSession` (#212); its `.data` begins with `"BUTTON_01"` at `0x803AE428`. File names are descriptive: the class names come from the RTTI strings `CStrapReminder`, `IStrapReminder` and `CTextWorldObjStat`.

## F-211-2: CStrapReminder

`CStrapReminder` derives from `IStrapReminder` (an `ICalcDraw` with one more pure virtual), as the RTTI base list `ICalc`, `ICalcDraw`, `IStrapReminder` shows. It is 0x468 bytes: vptr, an inline `CLayout` at 0x04 (0xE8 bytes), a `CFiber` at 0xF0 (0x370 bytes, vptr at 0x458) and a flag byte at 0x460. The factory (`0x80118180`) constructs it inline, loads `title2d.arc` and the layout `wii_remote`, and starts the fiber with `bind(&CStrapReminder::Run, this)` and a 0x8000-byte stack. `Run` (`0x801184D0`) plays `begin`, waits 60 frames, then waits up to 1200 frames for any Wii Remote button except Home (mask `0x1F10`) and queues `end`, `loading_in`, `loading_loop`. `Finish` (slot 7, `0x80118710`) waits the same way if the sequence has not started, then queues `end`, otherwise plays `loading_out`.

Descriptive names: `Finish`, `Run`, `Create`, `Initialize` (inlined into `Create`), `mCloseQueued`. The default `CFiber()` constructor must be `throw()` to reproduce the `__unexpected` landing pad around the inlined `Initialize` call.

All 16 retained functions are 100% in objdiff except relocation names of string literals (`@STRING@...` against `lbl_`). The unit stays `NonMatching` because of the weak-template order below.

## F-211-3: CTextWorldObjStat

`CTextWorldObjStat` implements `IWorldObjStat`: flags at 0x04 (bit 0 has a position, bit 2 shown), type at 0x08 (6 plus the kind), a position `VEC3` at 0x0C, a `CLayout` at 0x18 and the `show` motion at 0x100. Kinds select the layouts `wos_money`, `wos_spstat`, `wos_chit` and `wos_time` (the original spelling); `Load` fills the `TEXT` pane. `CalcView` projects the position through view and projection, and if it is inside the clip range sets the root pane translation to half the view rectangle scaled by the clip coordinates. The accessors `CLayout::GetLayout`, `CLayout::GetViewRect`, `nw4r::lyt::Layout::GetRootPane` and `nw4r::lyt::Pane::SetTranslate` were added to the headers.

Seven of eight functions are 100%. `CalcView` is 98.7%: the clip constant and the first position component use `f2` and `f3` in the original and `f3` and `f2` in every source form tried (about 2,000 compiled variants of the condition, the locals and the arithmetic).

## F-211-4: weak template order blocks the link

The linked `CStrapReminder.o` has the right size once the original copy of `shared_ptr<list<shared_ptr<CFileData>>>::~shared_ptr` (`0x80076CB0`) is named, but the order of its weak template functions differs: the original emits the three `function_imp` virtuals (`clone`, `clone`, `operator()`) before `get_deleter`, `dispose` and the shared-pointer destructors, and its `.data` holds the deleter vtable before the `function_imp` vtable. In every mini translation unit tried (creating the function first, in another inline function, in the class body, in a separate static function) the `shared_ptr_deleter` virtuals come first. The text and data order of the other functions, and the literal order (reversed within each deferred inline function), are reproduced by defining `Draw`, `Calc`, `Finish`, `Run` and `Initialize` as out-of-class `inline` members after a normal `Create`, in that order.

## Traps

- **Literal and function order follow definition style.** String literals in deferred inline functions are emitted in reverse order of use and after the RTTI names, and inline functions are emitted in reverse declaration order. A class-body inline and an out-of-class `inline` member behave differently. Brute-force the definition order (compile to a scratch object and compare `.data` and `.sdata` order) before fighting registers.
- **Header literals change the float pool.** The inline `CLayout::CreateMotion(const char*)` family pools `0.0f` and `1.0f` when `CLayout.h` is parsed, so a later `-1.0f` follows `1.0f` in `.sdata2`; the original has `-1.0f` before `1.0f` in `CTextWorldObjStat`.
- **A throw() constructor is needed for inlined fibers.** `CFiber() throw()` reproduces the unexpected-handler region of an inlined default construction.
- **Search tooling belongs in scratch.** Direct compiles with the project flags and a word-level comparison against the original bytes (masking relocations) run in under a second; use them for register-allocation searches.
