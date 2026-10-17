# F-395-1: NW4R g3d `AnmObjVis` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #395 owns `.text` `0x80255510–0x80255E74`. Its first two functions, `detail::CalcViewTexMtx` and `CalcInvWorldMtx`, end `g3d_transform.cpp` and were matched with #394 ([F-394-1](394-g3d-resanmscn-transform.md)). The rest is `g3d_anmvis.cpp`, which ends with the first two functions of #396.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_anmvis.cpp` | `.text` | `0x80255730–0x80255EBC` | 22 functions of `AnmObjVis` and `AnmObjVisRes`, and the two `ApplyVisAnmResult` functions |
| | `.rodata` | `0x8036C710–0x8036C738` | `TYPE_NAME` of `AnmObjVis` (0x10) and `AnmObjVisRes` (0x18 with padding) |
| | `.data` | `0x803C59E0–0x803C5A68` | vtables of `AnmObjVisRes` and `AnmObjVis` (0x44 each) |
| | `.sdata` | `0x804B8958–0x804B8960` | the weak `policyTable` of the inline `GetAnmPlayPolicy` |
| | `.sdata2` | `0x804BAB80–0x804BAB90` | float and double constants |

`AnmObjVis::GetTypeName` (`0x80255E80`) and `GetTypeObj` (`0x80255EB0`) are the first two functions of #396, which continues at `0x80255EC0` with `g3d_anmclr.cpp`. objdiff reports 100% for all 22 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

`src/nw4r/g3d/g3d_anmvis.cpp` is CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/g3d_anmvis.cpp`, stripped of comments, and compiles to the target bytes unchanged. The linker stripped the `AnmObjVisNode` and `AnmObjVisOR` classes and several `AnmObjVis` members; the source keeps them.

The unit needs the g3d object framework headers, imported from the same ss commit and stripped of comments: `include/nw4r/g3d/g3d_rtti.h`, `g3d_obj.h`, `g3d_anmobj.h`, `g3d_anmvis.h` and `platform/g3d_allocator.h`. Changes: ss's `ROUND_UP` macro does not exist here, so `g3d_obj.h` writes the alignments as masks, `(x + 3) & ~3`, `(x + 31) & ~31` and `(N + 4) & ~3`; `g3d_anmobj.h` includes `g3d_resmdl.h` for `ResMdl`; and the allocator header includes `<revolution/mem.h>`.

## Outside symbols

These were renamed in `symbols.txt` so the unit links. Their splits and sizes are unchanged. Functions: `G3dObj::~G3dObj` (`0x8025AD00`), `PlayPolicy_Onetime` (`0x8025AE70`), `PlayPolicy_Loop` (`0x8025AE80`) and `AnmObj::SetAnmFlag` (`0x8025AEF0`). Data: `TYPE_NAME` of `G3dObj` (`.rodata` `0x8036C838`, string `G3dObj`) and `AnmObj` (`0x8036C848`, string `AnmObj`), and `FrameCtrl::smBaseUpdateRate` (`.sdata` `0x804B8960`, 1.0f). The `policyTable` entries point to `PlayPolicy_Onetime` and `PlayPolicy_Loop` in that order.

## Traps

- **`<revolution/mem.h>` defines `RoundUp` and `RoundDown` as macros.** `include/revolution/mem/heapCommon.h` defines them, and they break `nw4r/ut/ut_algorithm.h`'s templates of the same names if it is included later. Include `ut_algorithm.h` before any header that pulls in `mem.h`.
