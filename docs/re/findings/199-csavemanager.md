# Issue 199: CSaveManager.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the dtk split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #199 lists 27 functions in `.text` `0x80108290`-`0x80108B84`, the first block of an eight-issue file (#199 to #206). No decompiler output was used.

## F-199-1: unit boundaries

`0x80108290`-`0x8010C488` is one translation unit, registered as `game/CSaveManager.cpp`. The file name is original: the class is `@unnamed@CSaveManager_cpp@::CSaveManager` in the RTTI name at `.data` `0x803AD6D0`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800287B0`-`0x80028E54` | 145 records, one per function with a stack frame |
| `extabindex` | `0x8004B5D0`-`0x8004BC9C` | First record names `0x80108290`, 145 entries |
| `.text` | `0x80108290`-`0x8010C488` | Starts after the previous file's static initializer, ends after the last `vector<void*>` helper |
| `.data` | `0x803AD140`-`0x803ADA30` | Wide message pool, vtables, deleter and `function_imp` type names, throw-type names |
| `.sdata` | `0x804B7310`-`0x804B7358` | `"saving"`, `"begin"`, `"loop"`, `"end"` and five type-info records |
| `.sbss` | `0x804B9070`-`0x804B9080` | Singleton pointer (`0x804B9070`) and a two-word handle |

- **Start.** `0x80108240` is a static initializer listed in `.ctors` `0x803529E0` that registers a global destructor, so it ends the preceding file. `0x80108290` is the first function that touches this file's `.data` (`0x803AD140`) and `.sdata` (`0x804B7310`).
- **End.** The next file is entered at `0x8010C490`: it is the first function to read `.sdata2` `0x804BA0D8`, and the `.data` at `0x803ADA30` (`mat '%s' has more than ...`) is not referenced by anything before it. The `vector<void*>` helpers of `0x8010C050`-`0x8010C488` are weak and are emitted here because this file instantiates them. The survey estimated the end at `0x8010BD28` (#206) and assigned `0x8010BD30`-`0x8010C488` to #207; they belong to this unit.
- **No `.ctors`, `.rodata`, `.sdata2` or `.bss` entry.** The `.sbss` word `0x804B9098` (`gSharedLayoutArchives`, [F-56](56-ccombodisplay.md)) is shared with other files.

The split alone links and verifies.

## F-199-2: class and function identities

`CSaveManager` derives from `ISaveManager`, an `ICalcDraw` with six more pure slots (vtable `0x803AD780`; `CSaveManager` vtable `0x803AD69C`). Members: `shared_ptr<CFiber>` at 4, `shared_ptr<CLayout>` at 0xC (the "saving" layout), `std::queue<WorkType>` at 0x14 (a `deque`, 0x18 bytes), `shared_ptr<IDlgMsgWin>` at 0x2C. The constructor stores `this` in `.sbss` `0x804B9070`.

| Address | Function | Match |
| --- | --- | --- |
| `0x80108290` | `CSaveManager::Work(WorkType)`: the fiber body | 99.95% |
| `0x80108600`, `0x80108670` | `IDlgMsgWin::AskYesNo(const wchar_t*)`, `IDlgMsgWin::ShowMessage(const wchar_t*)`, non-virtual inline members | 100% |
| `0x801086D0`-`0x80108970` | `shared_ptr<CLayout>` `reset(CLayout*)`, its constructor, `move_ptr`, `compressed_pair`, `default_delete`, `shared_ptr_deleter`, `swap` and `operator->` | 100% |
| `0x80108980`-`0x80108B50` | `vector<IController*>` default constructor and `push_back` chain over `__vec_constructor<void*>`, `cap`, `back` | 100% |
| `0x80108B80` | `queue<WorkType>::empty` | 100% |

`Work` runs the work item and then drains the queue. `WorkType` 1 (load) checks the NAND (`0x80167E90`), loads `savedata.bin`, and on failure loads the backup: a good backup is rewritten with `Save(2)` and a "restored from backup" message is shown; a missing main file (result `0x65`) is silent; otherwise the player is asked whether to delete the broken files (`0x80165B80`, three `WaitNandDelete` calls) and refusal shows the fatal system message. Types 2, 3 and 4 call `Save(0)`, `Save(3)` and `Save(4)`; type 5 deletes the files. Types other than 1, 3 and 4 create a `CLayout` with the layout name `"saving"`, wait for its `"begin"` motion, queue `"loop"`, and after the work queue `"end"`, wait for it and drop the layout. The bitmask test `(1 << (type - 1)) & 0xD` is produced by `type != 1 && type != 3 && type != 4`; the `Save` cases are a nested switch inside one grouped label, which gives the original range test `type - 2 <= 2`.

The three Japanese messages (`0x1001C`, `0x1001B`, `0x10021`) are passed as fallbacks to `fn_80086620`, which ignores them and returns `GetMessageText(id, gEmptyText)`; this is `GetMessageString` in `include/game/Message.h` (added by another contributor while this branch was open; the source here uses it). The string pool is one 0x16C-byte `.data` object (offsets 0, 0x80, 0x118), reproduced exactly from three adjacent wide literals.

`IDlgMsgWin` slots 7 to 15 (vtable `0x8039DBC0`, `CDlgMsgWin` bodies): `SetControllerSlot` (stores 0x11C), `OpenMessage`, `OpenImageMessage`, `IsMessageClosed`, `IsAnimationDone`, `SetDefaultYes`, `OpenYesNo`, `IsYesNoClosed`, `IsYesSelected`. Names are descriptive, behaviour medium. The factory `0x80084D40` is `IDlgMsgWin::Create(bool registerForUpdates, bool background)`; the second argument is passed on to `CLayout::Load`.

`0x8006EC10` assigns a `vector<IController*>` to the 0x40-byte input object `0x803E9088` at offset 0x34 (`CControllerComposition::SetControllers`, declared in `include/game/CControllerComposition.h`); `GetControllerTable()` `0x80124A50` returns that object. `GetUIInput()` at `0x80124A40` returns the same address typed as `IUIInput`.

## F-199-3: what is left

- **The unit stays `NonMatching`.** It covers #199 to #206 (243 functions, 15,256 code bytes). This issue's 27 functions are reconstructed (26 at 100%, `Work` at 99.95%); the other 216 are not. #199 is closed on the same terms as #142, whose `NonMatching` unit also spans neighbouring issues.
- **`Work` 99.95%.** Two instructions load the second and third message from the pool with `addi rX, r28, 0x80` and `0x118`. The original object carries a dtk-synthesised `R_PPC_NONE` relocation with that addend on both; the compiled object cannot have one, so objdiff reports an argument mismatch. The opcodes and operands are identical.
- **`Work` is kept alive with `#pragma force_active`.** It is referenced only by the bound `mem_fn` that `StartWork` (`0x80108DC0`, #200) builds; until that and the vtable owners exist, nothing in the source references it and the compiler would drop it. Remove the pragma when `StartWork` is written.
- **`vector` push_back chain.** The out-of-line `__vec_constructor<void*>::push_back` (`0x80108AB0`) is the plain `size < cap` test; the `grow_by(1, pod_type())` branch seen in `BWorld.cpp`'s inlined copy belongs one level down, in `push_back_realloc(x, int2type<true>)` (`0x8010A8A0`, 89% here), which falls back to `append_realloc(1, x)` (`0x8010BF40`, the split-buffer body moved there). `include/vector` is restructured that way; `BWorld`, `CEffect` and `CDlgPiiSelector` scores are unchanged or better.
- **Scaffolding.** `include/queue` and the `empty`, `front`, `pop_front` additions to `include/deque` give the `queue<WorkType>` chain of `0x80108B80`-`0x80108CF0`. Only `empty` is credited; the original `front` and `pop` chains have one more wrapper level and a different `pop_front` (`0x80108CF0` calls an erase helper `0x80108D00` with a `pod_type` tag), which #200 should rework.

## F-199-4: map of the other 216 functions

Recorded for the owners of #200 to #206; identifications are from call graphs and vtables, names are descriptive.

| Address | What it is |
| --- | --- |
| `0x80108BB0`-`0x80108CF0` | `queue<WorkType>::front` and `pop` chains over `deque`; `0x80108BE0` is the body (`buf()[start_ / 256][start_ % 256]` through `__cdeque_buf::operator[]` at `0x80108C70`). `pop_front` is `0x80108CF0` (`li r4, 1`) calling `0x80108D00`, which tags `int2type<false>` and calls the erase helper `0x8010A5D0` |
| `0x80108D30` | `WaitIdle`: `while (mFiber && !mFiber->IsFinished()) YieldFiber();` through `shared_ptr` bool (`0x80108D90`) and `operator->` (`0x80108DB0`) |
| `0x80108DC0` | `StartWork(WorkType)`: copies the 12-byte member-pointer constant `.data` `0x803AD2AC` (`{0, -1, Work}`), builds `bound_func<void, mem_fn_1<void, CSaveManager, WorkType>, tuple<CSaveManager*, WorkType, nat x8>>` into a `function<void()>`, then `mFiber.reset(new CFiber(f, 0x8000))` (`0x370` bytes; the `CFiber(const function<void()>&, u32) throw()` constructor `0x80108EA0` runs `Initialize` and `Start`; its vptr is at 0x368) |
| `0x80109510`, `0x80109620`-`0x80109650`, `0x801096B0` | `ISaveManager` slots: `LoadSync` is `WaitIdle(); StartWork(1); WaitIdle();`; `RequestSave`, `RequestSaveWithoutBackup`, `RequestSaveCommitOnly` and `RequestDelete` call `0x80109550` with 2, 3, 4 and 5, which pushes onto the queue when the fiber is running and otherwise calls `StartWork`; `IsIdle` (`0x80109650`) is `!mFiber || mFiber->IsFinished()` |
| `0x801096C0`, `0x801097A0` | `Calc` (resume the fiber, then slot 1 of the layout and of the message window) and `Draw` (slot 4 of both) |
| `0x80109820`, `0x80109880` | Factory (`new CSaveManager`, 0x34 bytes, into a `shared_ptr<ISaveManager>`) and the constructor, which stores `this` in `.sbss` `0x804B9070`; `0x8010A0A0` returns that pointer; `0x8010A0B0` is the deleting destructor |
| `0x80109920`-`0x80109E80`, `0x8010A150` | `ISaveManager` deleting destructor, `deque` default constructor chain (`0x801099A0`-`0x80109B50`), `deque` deleting destructors (`0x80109B80`-`0x80109E20`) and `~deque` (`0x8010A150`, frees each 0x400-byte block) |
| `0x80109E80`-`0x8010AA90` | `shared_ptr<CSaveManager>` constructor and `shared_ptr_deleter` instances (vtable `.data` `0x803AD2C8`), `function_imp` for the bound member (vtable `0x803AD380`, `clone` `0x8010A920`, `clone(buf)` `0x8010A970`, `operator()` `0x8010A9E0`) |
| `0x8010A260`, `0x8010A330`, `0x8010A5D0` | `deque::push_back`, `deque` grow at end (0x294, throws `"deque length error"`), and the erase-at-front helper |
| `0x8010AC50`-`0x8010B800` | `vector<void*>::insert(pos, first, last)` chain over `__deque_iterator` ranges (five forwarding layers, `0x8010AE20` is 0x358 bytes) and its iterator helpers |
| `0x8010B840`-`0x8010C488` | `function_imp` destructor and clone, `shared_ptr_deleter<CLayout>` and `<char[]>` support, and the remaining `vector<void*>` helpers (`reallocate`, `swap`, `allocate`, `max_size`) |

Matching this needs the Metrowerks `deque` and range-insert code, which is not in the repository; `include/deque` has only the pieces `Work` reaches.

## Traps

- **An unreferenced class in an anonymous namespace is dropped entirely.** The first compile of `CSaveManager::Work` produced an empty object. `#pragma force_active on` around the definition keeps it without changing code.
- **A bool expression, not a switch, gives the bitmask test.** `switch (type) { case 1: case 3: case 4: ... }` and `||` chains compile to compares; `bool b = type != 1 && type != 3 && type != 4;` compiles to `cmplwi`, `slw`, `andi. 0xD`.
- **A nested switch gives a range split.** Five distinct case labels always compile to a linear compare chain. The original `cmplwi type-2, 2` split appears when cases 2 to 4 share one label that holds an inner switch.
- **`operator->` mangles as `__rf__` and `operator*` as `__ml__`.** An `mMsgWin->Method()` call is one `__rf__` call; `*mMsgWin` would be `__ml__`. This identified `AskYesNo` and `ShowMessage` as members of `IDlgMsgWin` rather than free functions.
- **A weak callee in an earlier unit still needs its name.** objdiff pairs by symbol name; rename the `fn_` placeholder at the callee's address to the mangled name of your declaration (done here for about 40 callees) or every `bl` scores as a mismatch.
- **A fake `R_PPC_NONE` on pooled-literal offsets cannot be matched.** See F-199-3.
