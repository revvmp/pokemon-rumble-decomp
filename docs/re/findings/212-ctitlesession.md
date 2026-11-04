# CTitleSession (#212)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #212 lists 17 functions in `.text` `0x80119020`-`0x8011AB90`; the block is exactly one source file.

## F-212-1: unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x80119020`-`0x8011AB90` | `0x80119020` follows `CTextWorldObjStat`; `0x8011AB90` is the factory of the next unit (`CTutorialMsg`), which allocates a 316-byte object and calls the constructor at `0x8011B1A0` |
| `extab` | `0x80029E78`-`0x8002A3C8` | Record of the first function to the record of `0x8011AB90` |
| `extabindex` | `0x8004C0BC`-`0x8004C170` | Entries of the 17 functions; `0x8004C170` names `0x8011AB90` |
| `.rodata` | `0x80354E38`-`0x803550B8` | Three `const` tables referenced only by this unit: message ids by mode (5 ints), message ids by level and mode (3 by 5 ints) and 35 title text records of 16 bytes |
| `.data` | `0x803AE428`-`0x803AEDD0` | Includes the generated `"exception"` string at `0x803AEDC4`; the former boundary inference is superseded by F-212-5 |
| `.sdata` | `0x804B73F0`-`0x804B74B8` | Short literals, `TEXT_1D`-`TEXT_3D`, five RTTI objects and the `default_delete` type-name pointer; `0x804B74B8` is `"text0"` of the next unit |
| `.sdata2` | `0x804BA138`-`0x804BA140` | `0.0f` and `1.0f` |

The `.ctors` table has no entry between `0x80108240` and `0x80124A60`, so the unit has no static initializer. `lbl_803AED30` (a `0xA0` blob) was split at `0x803AEDC4` in `symbols.txt`; dtk refuses a split that ends inside a symbol.

## F-212-2: CTitleSession

`CTitleSession` lives in an anonymous namespace (RTTI name `@unnamed@CTitleSession_cpp@::CTitleSession`) and derives from `ITitleSession`, an empty `ISession` (`ICalcDraw` plus `enable_shared_from_this<ISession>`) whose only addition is the factory `ITitleSession::Create` at `0x80119020`. The vtable at `0x803AE848` is `~CTitleSession`, `Calc`, `CalcMtx`, `CalcView`, `Draw`, `Run`. The object is 0xAC bytes:

| Offset | Member | Notes |
| --- | --- | --- |
| 0x04 | `enable_shared_from_this` weak pointer | |
| 0x0C | `shared_ptr<CUIManager>` | `Calc`, `CalcMtx`, `CalcView` and `Draw` forward to it and then to `mMsgWin` |
| 0x14 | `shared_ptr<BUILayout> mLayouts[7]` | One layout per screen: `title_main`, `title_tuduki` (twice), `title_difficulty`, `option_main`, `option_vibration`, `option_delete` |
| 0x4C | `shared_ptr<IDlgMsgWin>` | `IDlgMsgWin::Create(false, true)` |
| 0x54 | `shared_ptr<ISaveManager>` | created lazily by `SaveAndWait` |
| 0x5C | `wstring mSlotTexts[3]` | |
| 0x80 | `bool mButtonEnabled[7][4]` | per screen: three slot or option buttons and `BUTTON_BACK` |
| 0x9C | `int mSlotsInUse` | |
| 0xA0 | `int mLastPlayedSlot` | slot with the largest save timestamp |
| 0xA4 | `int mFirstEmptySlot` | |
| 0xA8 | `int mFirstUsedSlot` | |

`Run` (`0x80119460`) loads `title2d.arc`, builds the seven layouts, fills 35 text panes from the title text table, registers the Wii Remote controller and then runs the menu state machine on `screen`, `shownScreen`, `selection` and `shownSelection`. Screen 0 is the title (new game, continue, options), 1 and 2 pick a save slot, 3 picks the difficulty (`SlotData` byte 23, unlocked count in byte 21), 4 is the option menu, 5 toggles vibration and 6 deletes a save through two `AskYesNo` confirmations. UI input bits are `0x1` up, `0x2` right, `0x4` down, `0x8` left, `0x10` cancel and `0x20` decide. Choosing a slot on screen 1, or a difficulty on screen 3, ends `Run` after `SetCurrentSaveSlot` and `WaitForMacAddress`.

`RefreshSlots` (`0x80119F00`) resets the button table, then for each slot formats the label (header message `0x600D2`, the timestamp from `OSTicksToCalendarTime`, the mode or level message and the lead Pokemon name `0x30000 + monsno - 1`) and fills the three `TEXT_nD` panes of the new game, continue and delete screens. `SlotData::Data` bytes 21 to 23 are signed (the code sign-extends with `extsb`), so `mFlags` became `s8`, and bytes 4866 and 4867 are the `mLastArea[2]` fields between `mLeadPiiMonsno` and `mLeadPiiIdLow`. The record stays the same size.

External functions used by the unit: `FormatDate` and `FormatTime` (Message.cpp), `SetCurrentSlot`, `GetOption`, `SetOption` and `ResetSlot` (SaveData.cpp, named by their owners; the original callers treat their results and arguments as `int`, so with the `u8` signatures in `SaveData.h` `Run` shows extra `clrlwi` and drops from 100% to 98.5%), `ChangeBGM` (`0x80125840`, medium: plays a BGM id if it differs from the current one), `CLayout::AppendMotion(const char*, const char*, f32, f32)` (`0x801D12F0`, high, the third overload after `CreateMotion` and `QueueMotion`), the `basic_string<wchar_t>` default constructor (`0x80087CF0`) and `operator+(const wstring&, const wchar_t*)` (`0x80088DB0`). `mLastArea[2]` (bytes 4866 and 4867, named by the `CGameMap` owner) is the last chosen level per difficulty.

## F-212-3: inlining and emission order

- `Run` is compiled with inlining disabled: every shared_ptr accessor, `GetLayout`, the vector and `GetMessageString` stay calls. `#pragma dont_inline on` around `Run` reproduces this exactly, including its 0xA9C bytes and register count; the other functions inline normally. A first guess without the pragma inlined the helpers into `Run` and produced a 0x370-byte frame.
- The pragma state is applied at code generation, so it must be reset before the end of the file; otherwise the deferred in-class functions and template instances (`~CUIManager` holders, `Calc`) lose their inlining.
- The kept functions appear in the original in this order: `Create`, `~ITitleSession`, `Draw`, `CalcView`, `CalcMtx`, `Calc`, `Run`, `RefreshSlots`, `WaitForMacAddress`, `SaveAndWait`, `IsFinished`, the `shared_ptr_deleter` virtuals, `~shared_ptr<ISaveManager>`, `~move_ptr<CUIManager>`, `~CTitleSession`, `~shared_ptr_deleter`. The source reproduces it with `Create` defined right after the class, the four virtuals defined as ordinary out-of-class functions in that order, `IsFinished` last and an implicit destructor.
- `ITitleSession::Create` must build `shared_ptr<ITitleSession>(new CTitleSession)` in the return slot; returning a `shared_ptr<CTitleSession>` adds a temporary and a 0x60 frame.
- `std::wstring::assign(const wstring&)` is the form the original uses for every append (`text.assign(text + L" " + message)`); `operator=` compiles to a different copy sequence.
- The case 2 to case 3 transition of the menu (a slot with no unlocked difficulties starts the game with difficulty 0) jumps over the head of case 3 in the original; only a `goto` reproduces it.

## F-212-4: remaining differences

All 17 functions are 100% in objdiff apart from relocation names of string literals and from `RefreshSlots`, which is at the score recorded in the ledger. Its instruction sequence and its `extab` action table are identical to the original; only register numbers differ. The original keeps 18 callee-saved registers live (`_savegpr_14`), uses `r31` for `this`, hoists the addresses of the locals `date` and `time` and of two string data pointers out of the loop and rematerializes the constants `0x80000000` and `0x60000`; the source reaches 17 registers, puts `this` in the lowest register and hoists the two constants instead.

## F-212-5: complete source-link tail

2026-11-04. The coordinating agent verified the original layout literal as `title_tuduki`, correcting the former inferred spelling `title_tududki`. Declaring `Run`, `RefreshSlots`, `WaitForMacAddress`, `SaveAndWait`, and `IsFinished` inline defers their literal pools until after the CUIManager and CTitleSession shared-pointer deleter data. All seventeen owned functions remain in exactly the original order. `Run` retains its existing inlining-control pragma; its actual machine instructions match, while nine stale `R_PPC_NONE` string groupings still affect diagnostic scoring.

The first source link had exact text and exception-table extents but extra data. Moving the existing `<string>` include before `ITitleSession.h` places the ITitleSession, ISession, and ICalcDraw RTTI chains before the standard exception RTTI, matching the target order. The source also emits the twelve-byte compiler-generated `exception` string at `0x803AEDC4`; the previous split excluded it and retained a duplicate fallback object. Extending this unit's data ownership to `0x803AEDD0`, exactly the next unit's start, corrects that boundary inference without copying any original data into source.

Two earlier RTTI objects had still been labelled as anonymous data. Reading the frozen executable proves:

| Original address | Name pointer | Proven RTTI name |
| --- | --- | --- |
| `0x804B68E0` | `0x803A3CD0` | `ISession` |
| `0x804B68E8` | `0x803A3CF8` | `std::tr1::enable_shared_from_this<ISession>` |

The second record has a null base-chain pointer. Naming these existing objects `__RTTI__8ISession` and `__RTTI__Q33std3tr134enable_shared_from_this<8ISession>` allows the linker to reuse them rather than retain duplicate weak RTTI from CTitleSession. The first retry then differed in only sixteen nonzero bytes within the thirty-two-byte region `0x803AE918`?`0x803AE938`: the original retained a zeroed duplicate vtable slot. The earlier real ISession vtable is at `0x803A3CB0`?`0x803A3CD0`, inside the former `lbl_803A3C88` object. Its words are `804B68E0, 0, 800B2390, 0, 80063DC0, 80063DD0, 0, 0`, exactly the newly emitted CTitleSession copy. Splitting the enclosing object and naming the earlier winner `__vt__8ISession` lets the linker resolve the duplicate correctly. An experimental `novtable` annotation suppressed the source vtable but was not applied: the original already proves an earlier retained definition.

The integrating agent applied the shared symbol and split changes and split the nine proven literal boundaries formerly grouped into `lbl_803AE70C`. All **17/17 functions now score 100%**. The complete source build then succeeded (`ninja` exit 0), and verification against the frozen `config/targets/wpse-v1.json` passed. `main.dol` SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. CTitleSession remains `Matching`, with its code and linked data reconstructed from source.

## Traps

- **An inlined member can hide a function.** Without `#pragma dont_inline on` around `Run`, `WaitForMacAddress`, `SaveAndWait` and `IsFinished` are inlined and disappear from the object, so objdiff reports them as "not in source". Check the function list before judging a score.
- **Register pressure flips the register of `this`.** CodeWarrior gives `this` the lowest callee-saved register until all 18 registers are in use, then `r31`. A function that is one register short in `_savegpr_N` will disagree on almost every register.
- **Stack object addresses are hoisted only for objects declared outside the loop.** Local `std::wstring` objects declared inside a loop body are never kept in a register; objects declared before the loop are.
- **Data order follows emission order.** A mismatch in a relocation addend such as `r28 + 0x2F0` reflects the position of literals in `.data`, which follows the order in which functions and instantiated vtables are generated, not their position in the file.
