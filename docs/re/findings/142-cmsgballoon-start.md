# Issue 142: where CMsgBalloon.cpp starts

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical to `orig/wpse-v1/main.dol`, with GNU objdump 2.42 and Python scans of small-data references. No decompiler output was used.

## F-142-1: `CMsgBalloon.cpp` starts at `0x800C59D0`, inside #141

Question: does the file of #142's functions start at the block start `0x800C6090`, so that #142 owns the split, the `configure.py` object line and the header.

### Result

No. The file starts at `0x800C59D0`, eight functions before #142's block, in #141 (`0x800C4960`–`0x800C6090`). Under [the file ownership rule](../../COLLABORATION.md#how-work-is-divided), #141 owns the `CMsgBalloon.cpp` split, its object line and `include/game/CMsgBalloon.h`. #141 was open and unassigned when this work was done, so #142 adds no split, object line, header or source; this contribution is research only. Confidence high.

### Evidence

- **The previous file ends at `0x800C59D0`.** The `shared_ptr_deleter<@unnamed@CMoneyLifeDisplay_cpp@::CMoneyLifeDisplay, ...>` vtable `0x803A4D28` holds the deleter destructor `0x800C5990` (slot 0), `get_deleter` `0x800C56F0` and `dispose` `0x800C5740`; the `CMoneyLifeDisplay` vtable `0x803A4EF0` holds its destructor `0x800C5880`. As in `CMultiRanking.cpp` (F-159-3), the deleter destructor is the file's last function. `0x800C5990` is 0x40 bytes and ends at `0x800C59D0` without padding.
- **The first `CMsgBalloon.cpp` functions are file-local helpers.** `0x800C59D0` (save-record lookup from a `StageID`, F-195-2) is called only by `0x800C5A60` and `CMsgBalloon` code at `0x800C8940`; `0x800C5A60` only by `0x800C8AFC`; `0x800C5B10` only by `0x800C8064` and `0x800C87B4`. No `CMoneyLifeDisplay` function calls any of them. The by-reference `max` helper `0x800C5C10` is also called from `0x801A0EDC` and `0x801C791C`, so it is a weak copy first emitted by this object.
- **The factory and initialization precede #142.** `0x800C5C30` (called only by `0x800A896C`) allocates 0x88 bytes, calls the constructor `0x800C66F0` (#143) and returns `shared_ptr<IMsgBalloon>` through `0x800C6330`. `0x800C5CC0` initializes the object and reads `msg_balloon` (`0x803A5488`), `msg_balloon_msg` (`0x803A5478`), `msg_balloon_warp` (`0x803A5464`) and `MODEL%02d` (`0x803A53A8`), all `CMsgBalloon.cpp` `.data`.
- **No initializer boundary.** `.ctors` has no entry between `0x800C0C30` and `0x80103540` (F-160-1).

### Section ranges

The end of each range is the `CMultiRanking.cpp` start of F-158-1.

| Section | Range | Start evidence |
| --- | --- | --- |
| `extab` | `0x8001DB14`–`0x8001E964` | Record of `0x800C59D0`; `0x8001DB0C` is the record of `0x800C5990` |
| `extabindex` | `0x80048C90`–`0x80049728` | Entry of `0x800C59D0`; `0x80048C84` is the entry of `0x800C5990` |
| `.text` | `0x800C59D0`–`0x800CD728` | See above. The last function `0x800CD720` (0x8 bytes) ends at `0x800CD728`; F-495-1 leaves the padding to the next object |
| `.data` | `0x803A50E0`–`0x803A5550` | `0x803A50E0` is the `shared_ptr_deleter<CMsgBalloon, default_delete<CMsgBalloon> >` vtable stored by `0x800C6630`; the previous object, `default_delete<BUILayout>` at `0x803A50B8`, ends the `CMoneyLifeDisplay` name cluster |
| `.sdata` | `0x804B6B20`–`0x804B6C70` | `0x804B6B20` is the type info named by the deleter vtable; `0x804B6B18` (`default_delete<CMoneyLifeDisplay>` type info) is read by `CMoneyLifeDisplay`'s `get_deleter` `0x800C56F0` |
| `.sdata2` | `0x804B9F78`–`0x804B9F98` | Lowest constant read by `0x800C59D0`–`0x800CD728` is `0x804B9F78` (`0x800C9C60`, `0x800C9D80`); `0x804B9F70` is read by `CMoneyLifeDisplay`'s `0x800C53B0` |

The file has no `.bss`, `.sbss` or `.ctors` entry. `0x804B9098`, `0x8035B9F8` and MSL's `_current_locale` (`0x803E6228`) are referenced but belong to other objects. The ranges were not linked as a split here, because the split belongs to #141.

## F-142-2: classes

| Class | Type info | Vtable | Base list |
| --- | --- | --- | --- |
| `CMsgBalloon` | `0x804B6B28` | `0x803A5208` | `0x803A5250`: `enable_shared_from_this<CMsgBalloon>` at 4, `ICalc`, `ICalcDraw`, `IDlg`, `IMsgBalloon` at 0 |
| `IMsgBalloon` | `0x804B6B38` | `0x803A52AC` | `0x803A52F4`: `ICalc`, `ICalcDraw`, `IDlg` |

`IDlg` is `include/game/IDlg.h` (type info `0x804B5FF0`). Slots are counted from the first function word.

| Slot | `IMsgBalloon` | `CMsgBalloon` | Name |
| --- | --- | --- | --- |
| 0 | `0x800C67C0` | `0x800CBD00` | Destructor |
| 1 | pure | `0x800C7410` | `Calc` |
| 2 | `0x80063DC0` | `0x800C7370` | `CalcMtx` |
| 3 | `0x80063DD0` | `0x800C7100` | `CalcView` |
| 4 | pure | `0x800C70C0` | `Draw` |
| 5 | `0x80077860` | `0x80077860` | `IDlg::IsClosable` |
| 6 | pure | `0x800C9F10` | `IDlg::Close` |
| 7 | pure | `0x800C6090` | Show, called by `ScriptBinding_ShowMessageBalloon` |
| 8 | pure | `0x800C9C60` | Called by `ScriptBinding_AddMessageBalloon` |
| 9 | pure | `0x800C9D80` | Called by `ScriptBinding_RegisterMessageBalloon` |
| 10 | pure | `0x800C99D0` | Called by `ScriptBinding_RegisterWarpzoneBalloon` |
| 11 | pure | `0x800C9EE0` | Called by `ScriptBinding_IsMessageBalloonShown` |
| 12 | pure | `0x800C98D0` | Called by `ScriptBinding_AdjustMessageBalloonY` |

The bindings reach the balloon through `IGameMap` slot 22 (vtable offset 0x60) of the map from `0x800AA2F0`. Method names other than `Calc` to `Draw` and the `IDlg` slots are descriptive, taken from the bindings (medium).

Members seen in `0x800C5CC0` and `0x800C6090`: `bool` shown at 0xC, `int` at 0x10, `shared_ptr<CUIManager>` at 0x18 (`new(0x54)`, constructor `0x801C9680`), `shared_ptr<BUILayout>` at 0x20, 0x28 and 0x30 (`new(0x134)`, constructor `0x801C9EE0`), `vector<shared_ptr<BUILayout> >` at 0x38 filled with 18 layouts, words at 0x5C (0) and 0x60 (-1), words at 0x64 to 0x70, and a motion holder at 0x80. The object is 0x88 bytes.

## F-142-3: the 24 functions of #142

Names are descriptive; the template shapes follow the `CDlgPiiSelector` reconstruction of F-92-2.

| Address | Function |
| --- | --- |
| `0x800C6090` | `CMsgBalloon` slot 7, `Show(bool)`. Returns if the flag at 0xC already equals the argument, then stores it. Showing clears 0x5C and 0x64 to 0x70, sets 0x60 to -1 and 0x10 to 5 and calls `0x800CAC00` on 0x80. Hiding calls `0x801D04A0(…, 0)` on the layout at 0x20, starts its `end` motion (`0x8006E240`, `.sdata` `0x804B6B68`), stores it at 0x80 (`0x80087320`) and plays sound 0x59 (`0x801E7F30`) |
| `0x800C6160` | Weak `BUILayout` forward: text setter on the object at 0x4C (`0x801D60B0`), called five times by `0x800C5CC0` with a pane name and a message from `0x80086620` |
| `0x800C6170` | `vector<shared_ptr<BUILayout> >::push_back(const value_type&)`, tail call to `0x800C6180` |
| `0x800C6180` | Push-back layer passing a zeroed tag byte to `0x800C61B0` |
| `0x800C61B0` | Push-back: constructs at `end()` when size is below capacity, else calls `0x800C6300` |
| `0x800C6240` | Capacity accessor of the compressed pair at 8 |
| `0x800C6250` | Compressed-pair storage `first()` |
| `0x800C6260` | `allocator<shared_ptr<BUILayout> >::construct`: placement `new` (`0x80087500`) and copy constructor `0x80096190` |
| `0x800C62C0` | Allocator accessor of the compressed pair at 8 |
| `0x800C62D0` | Compressed-pair storage `second()` |
| `0x800C62E0` | `end()`: data plus size times 8 |
| `0x800C6300` | Growth path, passing a zeroed tag byte to `0x800CA5D0` |
| `0x800C6330` | `shared_ptr<IMsgBalloon>(const shared_ptr<CMsgBalloon>&)` converting constructor, the factory's return |
| `0x800C6380` | `shared_ptr<CUIManager>::reset(CUIManager*)`: builds a temporary (`0x8008FB70`), swaps and destroys it (`0x8007C200`) |
| `0x800C63D0` | `shared_ptr<CUIManager>::swap` |
| `0x800C6400` | `shared_ptr<CMsgBalloon>(CMsgBalloon*)`: `move_ptr` guard, `new(0x10)` control block, `enable_from_this` |
| `0x800C64B0` | `shared_ptr<CMsgBalloon>::enable_from_this` |
| `0x800C6510` | `weak_ptr<CMsgBalloon>(CMsgBalloon*, shared_ptr_deleter_common*)`, calls `attach_weak` |
| `0x800C6550` | `weak_ptr<CMsgBalloon>::swap` |
| `0x800C6580` | `Metrowerks::move_ptr<CMsgBalloon>(CMsgBalloon*)` |
| `0x800C65B0` | `compressed_pair<CMsgBalloon*, default_delete<CMsgBalloon> >(CMsgBalloon*)` |
| `0x800C65E0` | Compressed-pair storage constructor |
| `0x800C6620` | `default_delete<CMsgBalloon>()` |
| `0x800C6630` | `shared_ptr_deleter<CMsgBalloon, default_delete<CMsgBalloon> >(CMsgBalloon*)`, stores vtable `0x803A50E0` |

Every helper is called out of line, including one-instruction accessors, so the file is compiled without inlining like `CDlgPiiSelector.cpp` (F-92-1).

### Dependencies

Once #141 splits the file, the template functions (`0x800C6170`–`0x800C6330`, `0x800C63D0`–`0x800C6630`) need byte-exact `shared_ptr`, `move_ptr`, compressed-pair and `vector` headers, which are partly available from the `CDlgPiiSelector` and `CPpuEngine` work. `0x800C6090` needs `BUILayout`, `CLayout` motion and sound interfaces; `0x800C6160` needs `BUILayout`. None of the 24 functions uses `basic_string`; `0x800C5CC0` in #141 does (`0x800721A0`, #327).

## Traps

- **A survey class start can be eight functions late.** The survey put `CMsgBalloon` at its first vtable function, `0x800C6090`, but the factory, its initialization and file-local helpers start at `0x800C59D0`. Find the previous file's deleter destructor before taking a block start as a file start.
