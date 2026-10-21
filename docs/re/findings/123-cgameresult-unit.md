# Issue 123: CGameResult.cpp unit and its first block (0x800AC4B0 to 0x800AF494)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #123 lists 26 functions in `.text` `0x800AC4B0`-`0x800AF494`. The first of them is the factory of the anonymous-namespace class `@unnamed@CGameResult_cpp@::CGameResult`, so this issue owns the `game/CGameResult.cpp` split, its `configure.py` object line and the `IGameResult` header. The neighbouring blocks are #122 (before, `CGameMap`) and #124 and #125 (after, the rest of `CGameResult`).

## F-123-1: unit boundaries of `game/CGameResult.cpp`

The file is one translation unit with no static initializer. Its split is in `config/wpse-v1/splits.txt`, with the object registered `NonMatching` in `configure.py`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original object linked. `unitdiff.py` reports 70 target functions and 23,428 bytes of `.text`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800186BC`-`0x80019A1C` | Record of the factory `0x800AC4B0` through the record of `0x800B1F50`; the next record belongs to `0x800B2040` |
| `extabindex` | `0x80048084`-`0x800482A0` | 45 entries, `0x800AC4B0` to `0x800B1F50`; the preceding entry is `0x800AC140` (`CGameMap`) and the following one is `0x800B2040` |
| `.text` | `0x800AC4B0`-`0x800B2034` | First function is the factory; last is the 0xE4-byte destructor `0x800B1F50`. `0x800B2040` stores the vtable `0x803A395C` and starts the next file |
| `.rodata` | `0x80352B90`-`0x80352BA8` | A 0x14-byte constant copied into a stack structure at `0x800ACCA8`; `0x80352B90` is the end of the `CEffect.cpp` range. The symbol `lbl_80352B90` is 0x18 bytes, and dtk rejects a split that ends inside it |
| `.data` | `0x803A2938`-`0x803A3678` | From the `gameresult` literal (first read by `0x800AC4B0`) to the `exception` string at `0x803A3668`; `0x803A3678` is the `vector length error` literal of the next file, which also holds the `.pkc` names from `0x803A36C4` |
| `.sdata` | `0x804B67D0`-`0x804B6880` | From the `time` literal to the last `default_delete` type-name record; `0x804B6884` is the constant `0x0098967F` of the next file |
| `.sdata2` | `0x804B9E78`-`0x804B9EC8` | `0.0f` and `1.0f` first read by `0x800AC4B0`; `0x804B9EC8` is `1.0e7f` of the next file |

- **Start.** `0x800AC4B0` is the first function that stores the `CGameResult` vtable (`0x803A2F18`) and the first reader of `0x803A2938`. The issue's start address is therefore correct. `0x800AC140` (size `0x368`) is the last function of `CGameMap`; the eight bytes `0x800AC4A8`-`0x800AC4B0` are alignment padding. `CGameMap`'s data ends with the `exception` string at `0x803A292C` and its `.sdata` with the type-name record at `0x804B67C8`.
- **End.** `.text` ends with `0x800B1F50` (a destructor that destroys a two-element `shared_ptr` array and releases a control block), at `0x800B2034`, not at the padded `0x800B2040` (trap F-495-1).
- **No other sections.** There is no `.ctors` entry (the nearest game initializer is `0x800A3AE0`, the next `0x800BE9B0`), no `.bss` or `.sbss` reference, and no other `.rodata`. `0x804B9098`, `0x804B5C48` (`gEmptyText`) and the large `.rodata` table at `0x803550E0` that `0x800B0110` indexes (the script binding table) are read here but belong to other units.
- **Corrections for neighbours.** #122 owns `CGameMap` up to and including `0x800AC140`, which ends at `0x800AC4A8`. Its last section ranges end at `.data` `0x803A2938`, `.sdata` `0x804B67D0`, `.sdata2` `0x804B9E78`, `extab` `0x800186BC` and `extabindex` `0x80048084`. #125 (`0x800B1580`-`0x800B2034`) contains the end of this file; `0x800B2040` is outside it.

## F-123-2: class hierarchy and layout

`IGameResult` adds no virtual function to `IGameMap`: its vtable (`0x803A3020`) holds the destructor `0x800AD0F0`, the `ICalcDraw` slots, three pure slots and the 35 `IGameMap` defaults, exactly like `IGameMap`. `include/game/IGameResult.h` declares it. `CGameResult` (vtable `0x803A2F18`, type info `0x804B6858`, bases `IGameResult`, `IGameMap`, `ICalcDraw`, `ICalc`) overrides slots 0 to 7 as in F-117-1: destructor `0x800B1B00`, `Calc` `0x800B0D00`, `CalcMtx` `0x800B0C70`, `CalcView` `0x800B0BE0`, `Draw` `0x800B0B40`, `RunAction` `0x800B0110`, `GetMapDesc` `0x800B0F90`, `GetWorld` `0x800B0FA0`.

The object is 0x98 bytes (`operator new(0x98)` in the factory). The offsets are fixed by the inlined constructor in `0x800AC4B0` and by the accesses of this block:

| Offset | Member | Evidence |
| --- | --- | --- |
| 0x00 | vptr | Stored with `0x803A2F18` |
| 0x04 | `enable_shared_from_this<IGameMap>` weak pointer (two words) | Zeroed; released by the destructor `0x800AD0F0` |
| 0x0C | `const MapDesc*` | Second factory argument; returned by `GetMapDesc` |
| 0x10 | `shared_ptr<CUIManager>` | `new(0x54)` built by `0x801C9680` |
| 0x18 | `shared_ptr<BUILayout>` | `new(0x134)` built by `0x801C9EE0`; loaded as `gameresult` (`0x803A2938`) with `gSharedLayoutArchives`, the default font map and the factory's second argument |
| 0x20 | `shared_ptr<CUIPiiSpec>` | `new(0x188)` built by `0x8011DCF0`; loaded as `piispec`, attached under pane `PIISPEC` (`0x804B67E0`) of the first layout |
| 0x28 | `shared_ptr<?>` | Two zeroed words, never read in this block |
| 0x30 | `shared_ptr<IPauseMenu>` | Assigned from the `CPauseMenu` factory `0x800D83B0` (F-167) through `0x800AABF0` |
| 0x38 | `shared_ptr<BWorld>[2]` | Array built with `__construct_array` (constructor `0x800AD160`, destructor `0x800A70A0`); each `new(0x90)` built from a `BWorld::Desc` with draw type 0, 128 effects, 1024 objects, 512 outlines, 512 models, 1024 `ScnObj`s and the sound listener enabled |
| 0x48 | `shared_ptr<BWorldCamera>[2]` | Constructor `0x800AD170`, destructor `0x80065630`; each `new(0x68)` built with `BWorldCamera(2)`, added with `BWorld::AddCamera` and made current |
| 0x58 | `vector<ResultPii>` (data, size, capacity) | Zeroed; see F-123-3 |
| 0x64 | `int` | Set to -1 by the factory; the window centre used by `0x800AEE00` |
| 0x68 | `int` | Stored from the result of `0x800AC140`, the last function of `CGameMap` |
| 0x6C | `CaptureTexture` (0x2C bytes) | Built with the render mode's width and height, format 4, from `GetWorkBuffer(0)` |

The factory then adds two `BModelObj`s (`new(0xF8)`, constructor `0x800528E0` with type 32) per world, one named by the `result2.brres`/`ref_dummy`/`result.brres` strings at `0x803A2968`-`0x803A29B4`, and calls the three helpers `0x800AD280` (build the entries), `0x800AED50` (position the entries for row 1) and `0x800AEE00` (spawn and remove the `CPii`s).

## F-123-3: the 26 functions of #123

`ResultPii` below is a descriptive name for the 0x38-byte vector element (no original name is known).

| Address | Size | Identity | Evidence |
| --- | --- | --- | --- |
| `0x800AC4B0` | `0xC3C` | Factory returning `shared_ptr<IGameResult>`; inlines the `CGameResult` constructor | See F-123-2 |
| `0x800AD0F0` | `0x70` | `IGameResult::~IGameResult()` (deleting), now `__dt__11IGameResultFv` | Slot 0 of the vtable `0x803A3020`; same code as `IGameMap::~IGameMap` at `0x800A5980` |
| `0x800AD160` | `0x10` | `shared_ptr<BWorld>` default constructor | Constructor argument of the array at 0x38 |
| `0x800AD170` | `0x10` | `shared_ptr<BWorldCamera>` default constructor | Constructor argument of the array at 0x48 |
| `0x800AD180` | `0xEC` | `vector<ResultPii>::~vector()` | Destroys elements of size 0x38 (array of two `shared_ptr` at 36, one at 28), then frees the buffer |
| `0x800AD270` | `0x8` | `shared_ptr<BWorldCamera>::operator->()` | Named in the symbol map |
| `0x800AD280` | `0x57C` | Builds the entries: for each of four players, if joined, copies the Pii list of the player's team box, marks each `CPiiPersonalData`, records its player index in a `map<const void*, unsigned long>` and pushes it to a `vector<shared_ptr<CPiiPersonalData>>`; then reserves `ResultPii`s and pushes one per Pii with a scale chosen by species (F-123-4) | Callers: factory; callees `0x800ADAF0`, `0x800AE0D0`, `0x800AE160`, `0x800AE450`, `0x800AE590`, `0x800AD820` |
| `0x800AD800` | `0x18` | `map<const void*, unsigned long>` default constructor | Zeroes two words and points the third at the second |
| `0x800AD820` | `0x2C8` | `map<const void*, unsigned long>::~map()`; erases the tree through `0x800B1270` | Depth-unrolled tree walk, node = (left, right, ...) |
| `0x800ADAF0` | `0x5E0` | `vector<shared_ptr<CPiiPersonalData>>::push_back` | 8-byte elements; length-error path throws `std::length_error("vector length error")` |
| `0x800AE0D0` | `0x90` | `map<const void*, unsigned long>::operator[]` | Looks up with `0x800AFEA0`, inserts with `0x800B10A0` |
| `0x800AE160` | `0x2EC` | `vector<ResultPii>::reserve` | Copy-constructs each element into a new buffer of `n * 0x38`, swaps, destroys the old one |
| `0x800AE450` | `0x58` | `ResultPii::ResultPii()` | Zeroes the `shared_ptr` at 28 and constructs the array at 36 |
| `0x800AE4B0` | `0xA4` | `ResultPii::~ResultPii()` (deleting) | Destroys the array at 36 and the `shared_ptr` at 28 |
| `0x800AE560` | `0x2C` | Predicate over `PiiSpeciesData` (`0x80124F40`), returning `bool` | Used for the default scale |
| `0x800AE590` | `0x7B8` | `vector<ResultPii>::push_back` | Copy-constructs the element at `data + size * 0x38` |
| `0x800AED50` | `0x84` | `CGameResult::SetPiiPositions(int row, int centerIndex)` (descriptive) | For each entry: `mPositions[row] = (index - centerIndex) * (< 0 ? 2.0f : 1.0f), mScale, 0.0f` |
| `0x800AEDE0` | `0x10` | `vector<ResultPii>::operator[]` | `data + index * 0x38` |
| `0x800AEDF0` | `0x8` | `vector<ResultPii>::size()` | Word at 4 |
| `0x800AEE00` | `0x620` | Spawns and removes the `CPii`s of the entries inside a window (descriptive `SyncPiis`) | An entry is shown when `center - 5 <= i < center + 30` or `i >= size - 35`; it builds two `CPii`s (type 64) from the personal data, adds them to the worlds at 0x38 and 0x40, positions them from `mPositions`; outside the window it removes and resets them |
| `0x800AF420` | `0xC` | Iterator copy constructor | One word |
| `0x800AF430`, `0x800AF440` | `0x8` | Iterator `operator*` | Returns the word |
| `0x800AF450` | `0x10` | Iterator `operator++` | Adds 0x38 |
| `0x800AF460` | `0x1C` | Iterator `operator!=` | |
| `0x800AF480` | `0x14` | `vector<ResultPii>::end()` | `data + size * 0x38` |

`ResultPii` layout (0x38 bytes): `unsigned long` player index at 0 (the value found in the map), `VEC3 mPositions[2]` at 4 and 16, `shared_ptr<CPiiPersonalData>` at 28, `shared_ptr<CPii> mPiis[2]` at 36 and 44, `float mScale` at 52.

## F-123-4: scale per species

`0x800AD280` stores 0.01f in the scale and replaces it by species. The table was recovered by running the compare tree of `0x800AD4D4`-`0x800AD6E4` for every value; the order below is the order of the case bodies in the object:

| Scale | Species ids |
| --- | --- |
| 0.04 | 50, 83, 131 |
| 0.05 | 51, 105, 407, 63 |
| 0.2 | 93 |
| 0.35 | 472 |
| 0.25 | 82 |
| 0.03 | 104, 117, 442 |
| 0.1 | 22, 116, 145, 147, 148, 149, 455, 474, 477 |
| 0.07 | 140 |
| 0.08 | 144, 146 |
| 0.01 | 412 |
| 0.02 | 413 |

Body order (each id is a separate body, identical bodies are not merged): 50, 51, 93, 472, 82, 83, 104, 105, 407, 22, 63, 116, 117, 131, 140, 144, 145, 146, 147, 148, 149, 412, 413, 442, 455, 474, 477. Other species use 0.15f when `0x800AE560` is true, or when `0x80124F30` returns 3 or 4; otherwise the 0.01f default stays.

## F-123-5: what blocks the source

Every function of the block is the factory, one of its three drivers (`0x800AD280`, `0x800AED50`, `0x800AEE00`), or a weak copy emitted because a driver uses a vector, map or iterator member. The factory needs, besides the shared_ptr and `BWorld` code already modelled, the class `CUIPiiSpec` (#217), the `IPauseMenu` factory (#167), the `CPiiBox`/`CPlayer` layout (the list at team box + 8 and the player at +20), the `PiiSpeciesData` predicates `0x80124F30` and `0x80124F40`, the `CPii` virtual slots called by `0x800AEE00` (vtable offsets 20, 40, 116 and 128), the helper `0x801B8BB0` that resolves the model names, `0x80066720` and `0x80066C80` on `BWorldCamera`, `fn_800DE750`, and a `gSharedLayoutArchives` read from `.sbss` `0x804B9098`. A source file also depends on `basic_string<char>` `reserve`/`replace<const char*>` (`0x8006D8B0`, `0x80067F70`) built from `strlen`. No source is committed and the original object stays linked.

A stand-in test of `SetPiiPositions` as an out-of-line member measured the same instruction stream as `0x800AED50` apart from register allocation; the real source cannot keep the function out of line without the factory calling it, as the original does. The vector accessors `0x800AEDE0` to `0x800AF480` follow from `std::vector` and appear once a caller uses them.

## Traps

- **A split must cover the whole symbol.** dtk refused `.rodata` `0x80352B90`-`0x80352BA4` because `lbl_80352B90` is 0x18 bytes. Check `symbols.txt` sizes before choosing a section end.
- **Raw `objdump` misdecodes Gekko prologues.** The `psq_st` and `psq_l` saves of float registers print as `xsmaddmdp`, `xxsel` and `lq` when the disassembler is not told the CPU; use objdiff (`unitdiff.py --function`) for the true instruction stream.
- **An unreferenced anonymous-namespace function emits nothing.** A member defined in `@unnamed@` and not called is dropped, and `#pragma dont_inline on` changes how the vector accessors compile (they become calls), so it cannot be used to test a single function. A temporary caller outside the namespace inlines the function instead; see F-117-4 for the stand-in method.
- **A case table's body order is the source order.** Equal bodies are not merged, so the order of the `0x800AD610`-`0x800AD6E0` stores in F-123-4 must be reproduced.

## Method

Disassembly with `powerpc-eabi-objdump -d` over the linked `build/wpse-v1/main.elf` (`--start-address`/`--stop-address`), `extabindex` entries parsed from `objdump -s -j extabindex`, a script that follows `lis`/`addi` pairs and `r13`/`r2` offsets to the data addresses each function reads, and a script that interprets the compare tree of the species switch for every value. `python configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the split.
