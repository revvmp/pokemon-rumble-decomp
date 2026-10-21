# CGameEnding.cpp unit and its first block (#116)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #116 lists 15 functions in `.text` `0x800A4EF0`–`0x800A5BE4`.

## F-116-1: unit boundaries of `game/CGameEnding.cpp`

The file is one translation unit with no static initializer. Its split is in `config/wpse-v1/splits.txt`, with the object registered `NonMatching` in `configure.py`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original object linked.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80015CA0`–`0x80016838` | Records named by the first and last index entries of the unit |
| `extabindex` | `0x80047BEC`–`0x80047D3C` | 28 entries, `0x800A4EF0` to `0x800A7610`; the neighbours index `0x800A4D10` (`CFacility`) and `0x800A7760` (`CGameMap.cpp`) |
| `.text` | `0x800A4EF0`–`0x800A7650` | First function is the factory that builds `CGameEnding`; last are the three `shared_ptr_deleter` destructors at `0x800A7590`, `0x800A75D0`, `0x800A7610` |
| `.data` | `0x803A1878`–`0x803A2150` | From the `event/ending.arc/event2d.arc` path to the `exception` string; `0x803A2150` starts the `CGameMap` stage paths |
| `.sdata` | `0x804B66D8`–`0x804B6790` | From `LINES` to the `default_delete<CShadowCamera>` type info; `0x804B6790` (`%02d`) is used only by `CGameMap.cpp` |
| `.sbss` | `0x804B8FF8`–`0x804B9000` | An 8-byte object read only by `0x800A6490` and `0x800A6D20` |
| `.sdata2` | `0x804B9E68`–`0x804B9E70` | Two floats read only by `0x800A5EB0` |

- **Start.** `CFacility`'s vtable, type name and base lists end at `0x803A1874`. `0x800A4EF0` is the first function to reference this unit's data (`0x803A1878`, the `CGameEnding` vtable `0x803A1AC8` and its three deleter vtables).
- **End.** `0x800A7650` has no exception entry and stores the `CGameMap` vtable `0x803A23F8`, so it is the `CGameMap` constructor and the first function of the next file. The deleter vtables `0x803A1908` (`CGameEnding`), `0x803A19BC` (`BWorld`) and `0x803A1A30` (`CShadowCamera`) name `0x800A7610`, `0x800A75D0` and `0x800A7590`.
- **No `.ctors` entry.** The nearest game initializer is `0x800A3AE0`, in an earlier file.
- The survey title's `CGameMap` and `CGameResult` are later files; only `CGameEnding` belongs to this unit.

## F-116-2: class hierarchy

RTTI type names and base lists (`.sdata` `0x804B6758`–`0x804B6778`):

| Class | Type info | Bases |
| --- | --- | --- |
| `@unnamed@CGameEnding_cpp@::CGameEnding` | `0x804B6758` | `IGameEnding`, `IGameMap`, `ICalcDraw`, `ICalc` at 0; `std::tr1::enable_shared_from_this<IGameMap>` at 4 |
| `IGameEnding` | `0x804B6760` | `IGameMap` at 0 |
| `IGameMap` | `0x804B6768` | `ICalcDraw`, `ICalc` at 0; `enable_shared_from_this<IGameMap>` at 4 |

Vtables:

| Address | Class | Contents |
| --- | --- | --- |
| `0x803A1D00` | `IGameMap` | Destructor `0x800A5980`; the `ICalcDraw` slots (pure `Calc`, `0x80063DC0`, `0x80063DD0`, pure `Draw`); three pure slots; 35 implemented inline slots |
| `0x803A1BF8` | `IGameEnding` | Destructor `0x800A59F0`; the `IGameMap` slots; nine pure slots |
| `0x803A1AC8` | `CGameEnding` | Destructor `0x800A7310`, overrides of `Calc`, `CalcMtx`, `CalcView` and `Draw` (`0x800A5EB0`, `0x800A5E60`, `0x800A5E10`, `0x800A5D90`), the three pure `IGameMap` slots (`0x800A5CF0`, `0x800A6460`, `0x800A6470`), the inherited inline slots with one override (`0x800A6450` for `IGameMap`'s `0x800A6DF0`), and nine `IGameEnding` implementations `0x800A6D30`–`0x800A6480` |

Of the 35 `IGameMap` inline slots, 28 are the weak copies emitted in this file (`0x800A5B40`–`0x800A5CE0`, and `0x800A6DF0` later). Seven keep copies from earlier units (`0x8006BAB0`, `0x8006E020`, `0x80078480`, `0x80078F80`, `0x80078F90`, `0x8008D2B0`, `0x80093630`), so `IGameMap.h` is included before this file. No slot has a recovered name.

## F-116-3: the 15 functions of #116

| Address | Size | Identity | Evidence |
| --- | --- | --- | --- |
| `0x800A4EF0` | `0xA88` | Factory returning `shared_ptr<IGameEnding>` | Called from `0x800BCC50`; see below |
| `0x800A5980` | `0x6C` | `IGameMap::~IGameMap()`, now `__dt__8IGameMapFv` | Slot 0 of `0x803A1D00`; releases the weak count of `enable_shared_from_this` at 4, deletes when the flag is positive |
| `0x800A59F0` | `0x70` | `IGameEnding::~IGameEnding()`, now `__dt__11IGameEndingFv` | Slot 0 of `0x803A1BF8`; the same code with one more inlined `this` check; also a cleanup action of the factory |
| `0x800A5A60` | `0xD4` | `list<wstring>` destructor | Size at 0, sentinel at 4; each node's 12-byte string at +8 frees its buffer when bit 31 of its first word is set; called from the factory's cleanup and `0x800A7310` |
| `0x800A5B40`–`0x800A5BE0` | `0x4`/`0x8` | Eleven `IGameMap` inline virtuals, empty or `return 0` | Listed in reverse slot order in `0x803A1D00`, which is the `-ipa file` emission order of F-735-1 |

The factory takes the result pointer, a record pointer stored at `+0xC` and load flags. It allocates the 0x310-byte object and constructs it inline (vptr, the weak pointer at 4, two `shared_ptr`s at `0x10` and `0x18`, layouts at `0x20`, `0x114` and `0x220` built by `0x801CB1C0`, words at `0x1FC`–`0x204`, `list<wstring>`s at `0x208` and `0x214`, a `shared_ptr` at `0x308`). It then wraps it in `shared_ptr<CGameEnding>`; fills a `BWorld::Desc` (draw type 1, 0x400 objects) and stores `new BWorld` at `0x10`; makes a 0x100-byte `CShadowCamera` and adds it with `BWorld::AddCamera`; loads `event/ending.arc/event2d.arc` and builds the `congratulations` and `staff_roll` layouts; counts the lines of the `LINES` text box (`L'\n'` plus one) into `0x204` and pushes `L""` into both lists for each line; loads `event/epilogue.arc/event2d.arc`, builds the `next` layout, sets the `TextBox_C_01`, `TextBox` and `TextBox2` texts, and shows `HARD`, `VERYHARD` or `THANKYOU` for the signed top byte of the record's first word being 0, 1 or 2.

## F-116-4: what blocks the source

Every function of the block is the factory or a weak copy emitted because the factory references it: the destructors through its cleanup actions, the inline virtuals through the vtables of its inlined constructor. The factory constructs, copies and destroys `std::wstring` inline (`wcslen`, `0x80082020`, `0x80082260`, the 12-byte representation) and inlines `list<wstring>::push_back`. That needs the MSL `basic_string`, which is not in the repository yet (#327). No source is committed and the original object stays linked. A source file also needs the declarations of the 35 `IGameMap` and nine `IGameEnding` virtuals, the layout class at `0x801CB1C0` and the loader `0x80127CF0`.

## Method

Python reading `orig/wpse-v1/main.dol` for vtables, RTTI records, `extabindex` and `.ctors`, and the generated assembly under `build/wpse-v1/asm/` for `@sda21`, `@ha` and call references. No decompiler output was used.

## Traps

- **A survey title can name later files.** #116's title lists `CGameEnding`, `CGameMap` and `CGameResult`, but the block is the first 15 functions of `CGameEnding.cpp`, which runs to `0x800A7650`, across #117 and later blocks.

## F-116-5: the factory is reconstructed and the unit now emits

`IGameEnding::Create(const MapDesc*, bool)` at `0x800A4EF0` is reconstructed in `src/game/CGameEnding.cpp`: `new CGameEnding(pDesc)` held in a `shared_ptr<CGameEnding>`, then the inline `Init`, then a conversion to `shared_ptr<IGameEnding>`. `tools/re/unitdiff.py` reports the factory at 99.88% with the same 2,696 bytes; the only differing rows are relocation names (`lbl_` data and vtable symbols, which the object-level comparison cannot resolve without the full `.data` split). With the constructor used, the compiler emits the `CGameEnding` vtable, so the 35 functions of #116 and #117 that F-117-4 measured now appear in the committed object (the two interface destructors, the sixteen `IGameMap` defaults, `Draw`, `CalcView`, `CalcMtx`, the three getters at 100%, `Calc` at 98.29%). The unit stays `NonMatching` because #118 and later blocks of the file are still absent.

Evidence for the shape, all from the factory body:

- `Init` first assigns the pause menu from `IPauseMenu::Create(mpMapDesc, true, 1)` (`0x800D83B0`, the `CPauseMenu` factory of F-167), which fixes the member at 0x18 as `shared_ptr<IPauseMenu>` (F-117-2).
- `BWorld::Desc` takes draw type 1, 0x400 objects, the outline capacity equal to the object capacity, models plus 0x80, scene objects as outline plus model plus 0x200, effects equal to the object capacity, capture and sound listener enabled. The world is installed with `reset(new BWorld(desc))`; assigning a temporary `shared_ptr` instead copies and releases and gave 85%.
- A default `shared_ptr<CShadowCamera>` is filled with `reset(new CShadowCamera(4))`, set up by `0x8010C5A0` (`CShadowCamera::Setup(width, height, textureLevels, useTextureAlpha)` with 0x200, 0x200, 1, false) and added to the world with `AddCamera(camera, 1)`. It is not stored in the object.
- `event/ending.arc/event2d.arc` and `event/epilogue.arc/event2d.arc` load through `LoadFileList` with separate archive-list locals; `congratulations` and `staff_roll` are hidden after loading. The line count starts at 1 and counts `L'
'` in the `LINES` text box; one empty `wstring` is pushed per line into each list.
- `next` takes message 0x60115 for `TextBox_C_01` and 0x60114 for `TextBox` and `TextBox2`, and shows `HARD`, `VERYHARD` or `THANKYOU` for `mMode` 0, 1 or 2.
- `std::basic_string` gained the MSL copy constructor: a short string copies its three words; a long one is initialised empty, reserved for its size and replaced from its own buffer. Without it the compiler used a bitwise copy and the list nodes of the factory differed.
- The `IPauseMenu::Create` third parameter is `int`, not `bool`: with `bool` the compiler loaded one constant into both arguments (`mr r6, r5`), whereas the target loads two. Its meaning is unknown.

## Traps

- **A repeated constant argument exposes a parameter type.** `Create(p, true, true)` with two `bool` parameters shared one register; the target's two `li` instructions required a different type on one of them.
- **`reset(new T)` and `x = shared_ptr<T>(new T)` compile differently.** The first builds in registers and releases the old pointer; the second goes through a memory temporary and a copy.
- **A missing `basic_string` copy constructor is silent.** The implicit one copies the 12-byte representation, hiding the long-string branch until a list of strings is compared.

## F-116-6: RunAction, Calc and the Matching trial

- **RunAction (`0x800A5CF0`).** Reconstructed as `IGameSession::GetInstance()->SetFade(0, -1)`, `YieldFiberStarted()`, then `RunScript("CGameEnding.pkc", 1, GetInstance()->GetScriptEnvironment())`. `unitdiff` reports 99.75% (only the string relocation name). `0x80102220` is a free function that allocates a `CPpuEngine` (`0xC4` bytes), loads the script with the given mode and environment and yields the fiber until it finishes; it is named `RunScript` (medium) in `symbols.txt` and declared in `CPpuEngine.h`. `0x800B7AA0` calls it six times.
- **IGameSession slot 26** is now `GetEnvironment()` (returns a `shared_ptr<NPpu::CEnvironment>` by value copied from `+0x84` of `CGameSession`, vtable `0x803A395C`, entry k at offset `8 + 4k`; high). Main already declares the neighbouring slots as `UnanalyzedSlot` placeholders and slot 32 as `SetFade(int type, int frames)`, which `RunAction` calls with `(0, -1)`; slot 30 is `GetPlayTimeSeconds`, slot 31 `GetMapChangeType`. Pull request #1121 renames the same slot; the two edits are textually identical.
- **Calc 98.18% to 99.18%.** Declaring the two iterators before the strings, `titles` before `lines`, and `int i` last (an exhaustive search of the 120 declaration orders) reproduced the first-constructed string slots and most register assignments. Still different: the target gives `i`, line and title the registers r30, r29 and r28, mine r28, r30 and r29; the target's string temporaries sit mirrored on the stack (first constructed at `0x40`, mine at `0x4C`), and the two `__unexpected` landing pads are swapped.
- **ParseStaffRollText stays 94.33%.** In the target the address of each `wstring` temporary is held in r25 (the `L""` length in r24) across `reserve` and `replace`; mine recomputes it from r31. Explicit `std::wstring(L"")`, named temporaries per branch (88.7%, worse) and the implicit `push_back(L"")` all failed.
- **Matching trial (not accepted).** With `Object(Matching, ...)` the link failed on `lbl_804B6760`, `lbl_804B6768`, `lbl_804B6770`, `lbl_803A19BC` and `lbl_803A1A30`, which other split objects reference. They are now named `__RTTI__11IGameEnding`, `__RTTI__8IGameMap`, `__RTTI__Q33std3tr134enable_shared_from_this<8IGameMap>` and the two `shared_ptr_deleter` vtables. The link then succeeded and produced a DOL of the same length class but not identical: `.text` is `0x80` bytes larger, `extab` `0x40`, `extabindex` `0x20`, and `.data` `0x220` smaller. Causes seen in the object diff: weak instances emitted here (`~weak_ptr` of `IGameMap`, `BWorld` and `BWorldObj`, six `shared_ptr` destructors, `CLayout::CreateMotion`/`AppendMotion` one-argument wrappers, `IsPausing`, `SetPausing`, `RegisterDialog`, `UnregisterDialog`, `GetTimerFrame`, `GetNoticeDisplay`, `GetDlgPiiSelector`) that earlier units own in the target; and in the target `.data` `0x803A1DDC`-`0x803A2150` a 688-byte block of `std::exception`, `logic_error`, `out_of_range` and `length_error` RTTI, exception name strings and `basic_string` error messages that the source does not yet produce. The unit stays `NonMatching`; the original object remains linked and the full build verifies.
- **Remaining #116 functions.** `0x800A5A60` (`list<wstring>` destructor) and `0x800A5B40`-`0x800A5BE0` (eleven `IGameMap` inline virtuals) are emitted and measure 100% in the object comparison.

### Traps

- **A trial `Matching` link can succeed and still differ.** `ninja` and the linker accept the object; only `verify.py` showed the size drift. Compare the DOL header section sizes first (a 20-line script over the first `0xD8` bytes) to see which section drifted.
- **`sed -i` through Git Bash strips CR.** A blanket `sed` on `CGameEnding.cpp` rewrote the whole file with LF; restore CRLF with a binary-mode Python rewrite before committing.
- **Inline-only edits to `.data` need the whole `std` exception set.** The out-of-line `basic_string` members throw `length_error` and `out_of_range`; their RTTI and name strings land in the first unit that instantiates the inline paths.
