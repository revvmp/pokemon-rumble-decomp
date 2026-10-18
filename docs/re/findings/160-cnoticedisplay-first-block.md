# Issue 160: the first CNoticeDisplay.cpp block

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the dtk 1.8.4 split assembly under `build/wpse-v1/asm/` and the linked `build/wpse-v1/main.elf`, which verifies byte-identical to `orig/wpse-v1/main.dol`. No decompiler output was used.

## F-160-1: `CNoticeDisplay.cpp` starts at `0x800CF8A0`, inside #159

Question: does the file of #160's functions start at the block start `0x800D00B0`, so that #160 owns the split, the `configure.py` object line and the header.

### Result

No. The file is `game/CNoticeDisplay.cpp`, named by the RTTI strings `@unnamed@CNoticeDisplay_cpp@::CNoticeDisplay`, `::BNoticeDisplayImpl`, `::BOther`, `::CSimpleText`, `::CYouWin`, `::CFight` and `::CLose`. Its first function is `0x800CF8A0`, the 0x6B0-byte function before #160's range, so the file's first three functions (`0x800CF8A0`, `0x800CFF50`, `0x800D0030`) lie in #159 (`0x800CD800`–`0x800D00B0`). Under [the file ownership rule](../../COLLABORATION.md#how-work-is-divided), #159 owns the split, the object line and the header. #159 was unclaimed when this work was done, so #160 adds no split, object line or source; this contribution is research only. Confidence high.

Start of each section, as far as the evidence below fixes it:

| Section | Start | Evidence |
| --- | --- | --- |
| `.text` | `0x800CF8A0` | `CMultiRanking` ends with `0x800CF860` (0x40 bytes, ending at `0x800CF8A0` without padding), which is slot 2 of the `CMultiRanking` deleter vtable `0x803A55B8` |
| `extab` | `0x8001F948` | Record of `0x800CF8A0`; `0x8001F940` is the record of `0x800CF860` |
| `extabindex` | `0x800497E8` | Entry of `0x800CF8A0`; `0x800497DC` is the entry of `0x800CF860` |
| `.data` | `0x803A5840` | The three `NIntl::basic_formatter<w>` exception type names (`InvalidFormatException`, `TooFewArgsException`, `TooManyArgsException`), read by `0x800CF8A0`, `0x800D0920`, `0x800D1340` and `0x800D1CA0`. `0x803A5834` (`time_c_00`) is the last string of `CMultiRanking`'s `0x800CD800` |
| `.sdata` | `0x804B6CD8` | `L"%d"`, read by `0x800CF8A0`; `0x804B6CD0` (`begin`) is read by `CMultiRanking`'s `0x800CEB20` |

### Evidence

- **Shared local data.** `0x800CF8A0` and three later `CNoticeDisplay.cpp` functions read the same `.data` object `0x803A5840`. Literal data is local to an object file, so they are one unit.
- **Callers.** `0x800CF8A0` is called only by `0x800D0920`, `0x800D1340` and `0x800D1CA0`, the `Init` functions of three notice classes (F-160-2). `0x800CFF50` is called only by `0x800D0030`, and `0x800D0030` only by `0x800D2BE0` (#162). The three are file-local helpers defined before the classes.
- **The previous file.** `0x800CF650` is in the `CMultiRanking` vtable `0x803A563C` and `0x800CF860` in the deleter vtable `0x803A55B8`, so `CMultiRanking`'s functions reach `0x800CF8A0`. `0x800CF8A0` reads the stage table `0x8035B9D0`, which `CMultiRanking` code does not.
- **No initializer boundary.** `.ctors` has no entry between `0x800C0C30` and `0x80103540`.

The file continues through #161 to at least #163: the `CNoticeDisplay` destructor `0x800D4080` and the deleter functions `0x800D3640`, `0x800D3690` and `0x800D4110` are in #162 and #163. Its end was not established here.

## F-160-2: classes and the 20 functions of #160

### Classes

The RTTI base lists and vtables give this hierarchy. `ICalcDraw` slots are 0 `~dtor`, 1 `Calc`, 2 `CalcMtx`, 3 `CalcView`, 4 `Draw` (`include/game/ICalcDraw.h`).

| Class | Vtable | Base list | Size | Notes |
| --- | --- | --- | --- | --- |
| `INoticeDisplay` | `0x803A5E08` | `0x803A5E50`: `ICalc`, `ICalcDraw` | 4 | Public interface, 12 slots; slots 5 to 11 are pure. Destructor `0x800D01D0` |
| `@unnamed@CNoticeDisplay_cpp@::CNoticeDisplay` | `0x803A5B00` | `0x803A5B68`: `ICalc`, `ICalcDraw`, `INoticeDisplay` | 0x14 | Implements `INoticeDisplay` |
| `::BNoticeDisplayImpl` | `0x803A5D9C` | `0x803A5DF4`: `ICalc`, `ICalcDraw` | 0x460 | Abstract, 7 slots; slots 5 and 6 are pure |
| `::BOther` | `0x803A5C68` | `0x803A5CB4`: adds `BNoticeDisplayImpl` | 0x550 | Abstract; slot 6 stays pure |
| `::CSimpleText` | `0x803A5BF4` | `0x803A5C44`: adds `BOther` | 0x560 | |
| `::CYouWin` | `0x803A5B84` | `0x803A5BD0`: adds `BOther` | 0x728 | |
| `::CFight` | `0x803A5CD0` | `0x803A5D1C`: `BNoticeDisplayImpl` | 0x468 | |
| `::CLose` | `0x803A5D38` | `0x803A5D80`: `BNoticeDisplayImpl` | 0x468 | |

`CNoticeDisplay` layout, from the factory and its accessors: vptr at 0, `shared_ptr<BNoticeDisplayImpl>` at 4 (object pointer at 4, count at 8), an `int` at 0xC and a `float` at 0x10 initialised from `.sdata2` `0x804B9FB0`. Both are passed to the `CYouWin` constructor.

`BNoticeDisplayImpl` layout, from the `CSimpleText` constructor `0x800D0760` and the destructors: vptr at 0, `CLayout` by value at 4 (constructor `0x801CB1C0`, destructor `0x801CC070`, F-93-1), and `CFiber` by value at 0xF0 (inline constructor then `0x8019E110`, destructor `0x8019E180`, its vptr at 0x458, F-82). `BOther` adds an `int` notice type at 0x460 and a second `CLayout` at 0x464. `CSimpleText` adds words at 0x550 (0), 0x554 (-1) and 0x558 (0) and a byte at 0x55C (0).

### Vtable slots

| Slot | `INoticeDisplay` / `CNoticeDisplay` | `BNoticeDisplayImpl` |
| --- | --- | --- |
| 0 | `0x800D01D0` / `0x800D4080` (#163) | `0x800D0830` |
| 1 `Calc` | `0x800D02E0` | `0x800D0380` |
| 2 `CalcMtx` | `0x80063DC0` / `0x800D02B0` | `0x80063DC0` |
| 3 `CalcView` | `0x80063DD0` / `0x800D0280` | `0x80063DD0` |
| 4 `Draw` | pure / `0x800D0210` | `0x800D0260` |
| 5 | pure / `0x800D3440` (#162) | pure; returns the notice type |
| 6 | pure / `0x800D0460` | pure |
| 7 | pure / `0x800D0430` | |
| 8 | pure / `0x800D0420` | |
| 9 | pure / `0x800D0410` | |
| 10 | pure / `0x800D0400` | |
| 11 | pure / `0x800D03D0` | |

### Functions

Names are descriptive unless the class name is given by RTTI. Confidence is that of the behaviour; method names are low.

| Address | Function | Behaviour |
| --- | --- | --- |
| `0x800D00B0` | Factory, `shared_ptr<INoticeDisplay>` by hidden result | Allocates and constructs the 0x14-byte `CNoticeDisplay` inline, wraps it in a `shared_ptr` with the deleter vtable `0x803A590C`; cleanup through the holder destructor `0x800D3960` (#163). Its one caller, `0x800A8680`, passes `r4 = 1`, which is not read, and stores the result at its object + 0x98 |
| `0x800D01D0` | `INoticeDisplay::~INoticeDisplay` | Deleting destructor; nothing but `delete` |
| `0x800D0210` | `CNoticeDisplay::Draw` | Forwards to the implementation's `Draw` when the pointer is set |
| `0x800D0240` | `shared_ptr<BNoticeDisplayImpl>` `operator bool` | Out-of-line weak copy, called from `0x800D0460` |
| `0x800D0260` | `BNoticeDisplayImpl::Draw` | Calls `Draw` (slot 4) of the `CLayout` at 4 |
| `0x800D0270` | `shared_ptr<BNoticeDisplayImpl>::operator->` | Out-of-line weak copy |
| `0x800D0280` | `CNoticeDisplay::CalcView` | Forwards to the implementation |
| `0x800D02B0` | `CNoticeDisplay::CalcMtx` | Forwards to the implementation |
| `0x800D02E0` | `CNoticeDisplay::Calc` | Calls the implementation's `Calc`; when bit 1 (finished) of its `CFiber` flags at 0xF0 is set, resets the `shared_ptr` |
| `0x800D0380` | `BNoticeDisplayImpl::Calc` | Resumes the `CFiber` (`0x8019E460`), then calls `Calc` of the `CLayout` |
| `0x800D03D0` | `CNoticeDisplay` slot 11 | Returns the implementation's slot 5 (its notice type), or 10 when none |
| `0x800D0400` | `CNoticeDisplay` slot 10 | Stores its `float` argument at 0x10 |
| `0x800D0410` | `CNoticeDisplay` slot 9 | Stores its `int` argument at 0xC |
| `0x800D0420` | `CNoticeDisplay` slot 8 | Empty |
| `0x800D0430` | `CNoticeDisplay` slot 7 | Forwards to the implementation's slot 6 |
| `0x800D0460` | `CNoticeDisplay` slot 6, show a notice of type `n` | Type 0 is derived from the state returned by slot 6 of the object from `0x800AA2B0`. Type 10 hides the current notice (slot 29 of that object with 0, then the display's slot 7). Otherwise it keeps a current notice of the same type, and else creates `CSimpleText` for types 1 to 3, `CFight` for 4, `CLose` for 5 and 9, and `CYouWin` for 6 to 8 (with the words at 0xC and 0x10), calls the class's `Init` (`0x800D0920`, `0x800D1340`, `0x800D1840`, `0x800D1CA0`) and stores it on success |
| `0x800D0760` | `CSimpleText::CSimpleText(int)` | Builds the `BNoticeDisplayImpl` and `BOther` bases inline and stores the type at 0x460 |
| `0x800D0830` | `BNoticeDisplayImpl::~BNoticeDisplayImpl` | Destroys the `CFiber` and the `CLayout` |
| `0x800D08A0` | `BOther::~BOther` | Destroys the second `CLayout`, then the base members |
| `0x800D0920` | `CSimpleText::Init` | Reads `/arc/blyt/notice.brlyt` and the `notice` name, loads the layout (`0x801CCBD0`), sets the `AREA`, `STAGE`, `ENEMY`, `START`, `CLEAR` and `YOUWIN` panes, formats the area name through `0x800CF8A0` and the `AREA_nw`–`AREA_se` table `0x803A58E8`, and starts the fiber (`0x8019E320`) |

### Dependencies

Reconstruction needs `#159`'s split and object line first, and then: the `CLayout` interface (`0x801CB1C0`, `0x801CCBD0`, `0x801CC070`), `CFiber` (`0x8019E110`, `0x8019E320`, `0x8019E460`), `shared_ptr` with the move-holder destructor used in the factory, the interface of the object from `0x800AA2B0` (slots 6 and 29), and, for `0x800D0920`, the `NIntl::basic_formatter` and MSL `basic_string` (#327). Fifteen of the twenty functions are small accessors or forwards that can be written once the header exists; the factory, `0x800D0460`, the constructor and the destructors need the member classes above.

## Traps

- **A file can start in the previous block through file-local helpers.** The first `CNoticeDisplay` RTTI and vtable references are at `0x800D00B0`, which the survey took as the file start, but three helpers called only from this file precede it. Check callers of the functions before a block start, and shared literal data such as exception type names, before taking the split.
- **Inlining is not uniform in this file.** `CNoticeDisplay::Draw` inlines the `shared_ptr` boolean test, while `0x800D0460` calls the out-of-line `operator bool` and `operator->`. Do not assume the whole file is built with `dont_inline` (F-92) from one function.
