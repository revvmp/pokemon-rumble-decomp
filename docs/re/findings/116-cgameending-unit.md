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
