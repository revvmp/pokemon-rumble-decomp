# Issue 158: CMsgBalloon tail and the CMultiRanking.cpp split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the dtk 1.8.4 split assembly under `build/wpse-v1/asm/`. No decompiler output was used.

## F-158-1: `CMultiRanking.cpp` boundaries

#158's block (`0x800CD180`–`0x800CD7F8`) holds the last 29 functions of `CMsgBalloon.cpp` (`0x800CD180`–`0x800CD720`) and the first function of `CMultiRanking.cpp` (`0x800CD730`), as F-159-1 found. `CMsgBalloon.cpp` starts in #142, which owns its split, so its functions are left in the automatic split.

`CMultiRanking.cpp` runs up to `CNoticeDisplay.cpp` (F-159-1):

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8001E964`–`0x8001F948` | Record of `0x800CD730` at `0x8001E964`; `CNoticeDisplay.cpp` starts at `0x8001F948` |
| `extabindex` | `0x80049728`–`0x800497E8` | Entry of `0x800CD730` at `0x80049728` |
| `.text` | `0x800CD730`–`0x800CF8A0` | `0x800CD730` reads the `.sdata2` `0.0f` at `0x804B9F98` with `CMultiRanking` code (F-159-1); `0x800CF860` is slot 0 of the `shared_ptr_deleter<CMultiRanking>` vtable `0x803A55B8` |
| `.data` | `0x803A5550`–`0x803A5840` | Model names `ranking_p1` to `ranking_back` and their table `0x803A5590`, the `ROOT_p1` to `ROOT_p4` table `0x803A55A8`, the deleter, `CMultiRanking` and `IMultiRanking` type data, and format strings used by the factory |
| `.sdata` | `0x804B6C70`–`0x804B6CD8` | `ROOT_p1` to `ROOT_p4`, read only through `0x803A55A8`; the previous object `end` at `0x804B6C68` is read by `CMsgBalloon`'s `0x800C7410` |
| `.sdata2` | `0x804B9F98`–`0x804B9FB0` | Read only by `0x800CD730`, `0x800CD800`, `0x800CEB20` and `0x800CF1D0`; `0x804B9F90` belongs to `CMsgBalloon`'s `0x800C8030` |

The file has no `.bss`, `.sbss` or `.ctors` entry. With the split and a `NonMatching` object line, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-158-2: class shape and the helper at `0x800CD730`

RTTI at `0x804B6CA0` names `CMultiRanking`, with bases `IMultiRanking`, `IGameMap`, `ICalcDraw` and `ICalc` at 0 and `std::tr1::enable_shared_from_this<IGameMap>` at 4. Its vtable `0x803A563C` has the 43 `IGameMap` slots of F-116 and adds none. The factory `0x800CD800` allocates 0x144 bytes and fills five arrays of four shared pointers at 0x40, 0x60, 0x80, 0xA0 and 0xD0. It loads a `BModelObj` (`new BModelObj(0x20)`) for each of the five names in `0x803A5590`, adds each to the world, and keeps the first four in the array at 0x40.

`0x800CD730`, `GetNodePosition(shared_ptr<BModelObj>, const char*)` (descriptive), returns the world translation of a node of the object's model: if the object, its `CModel` (`BModelObj` + 0x74) and the `ScnMdlSimple` (`CModel` + 8) exist, it looks up the node by name and returns column 3 of `GetScnMtxPos(MTX_WORLD, id)`; otherwise it returns zero. Its only caller is `CMultiRanking::CalcMtx` (`0x800CF070`, slot 2), which passes `mModels[i]` by value and `ROOT_p1` to `ROOT_p4`. It compares 100% in objdiff.

The helper is written with external linkage. Its only caller belongs to #159, and an internal-linkage definition would not be emitted until that caller exists (F-93 trap), so the linkage is unproven.

## Traps

None new. The F-159-1 trap (a survey class start can be one function late) applied here.
