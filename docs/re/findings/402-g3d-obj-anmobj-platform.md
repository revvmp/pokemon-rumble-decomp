# F-402-1: NW4R g3d object, animation-object and platform units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #402 owns `.text` `0x8025A250–0x8025B694`. Its first 19 functions, through `0x8025ACFC`, end `g3d_anmscn.cpp` and were matched with #401 ([F-401-1](401-g3d-anmscn.md)). The rest is four complete units that end exactly at the block's end; #403 begins at `0x8025B6A0` with `ScnDependentMtxFunc::EnvironmentMapping`.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_obj.cpp` | `.text` | `0x8025AD00–0x8025AE6C` | `G3dObj` destructor, `Destroy`, `DetachFromParent`, `GetTypeName`, `GetTypeObj` |
| | `.rodata` | `0x8036C838–0x8036C848` | `TYPE_NAME` of `G3dObj` |
| | `.data` | `0x803C5E88–0x803C5EA4` | `G3dObj` vtable (0x1C) |
| `nw4r/g3d/g3d_anmobj.cpp` | `.text` | `0x8025AE70–0x8025AF38` | `PlayPolicy_Onetime`, `PlayPolicy_Loop`, `AnmObj::SetAnmFlag`, `AnmObj::TestAnmFlag` |
| | `.rodata` | `0x8036C848–0x8036C858` | `TYPE_NAME` of `AnmObj` |
| | `.sdata` | `0x804B8960–0x804B8964` | `FrameCtrl::smBaseUpdateRate` |
| | `.sdata2` | `0x804BAC10–0x804BAC14` | `0.0f` |
| `nw4r/g3d/platform/g3d_gpu.cpp` | `.text` | `0x8025AF40–0x8025B460` | the seven `fifo` display-list functions from `GDSetGenMode2` to `GDLoadTexMtxImm3x3` |
| | `.sdata2` | `0x804BAC18–0x804BAC30` | the local `cm2hw` cull-mode table and five float constants |
| `nw4r/g3d/platform/g3d_cpu.cpp` | `.text` | `0x8025B460–0x8025B694` | `detail::Copy32ByteBlocks`, `ZeroMemory32ByteBlocks`, `ZeroMemory16ByteBlocks` |
| | `.sdata2` | `0x804BAC30–0x804BAC34` | `0.0f` |

Each data split ends at its last object and leaves alignment padding to the next unit. ss links `platform/g3d_tmem.cpp` between `g3d_gpu.cpp` and `g3d_cpu.cpp`; this executable has no code or data from it, so the linker removed the whole unit and it is not built. objdiff reports 100% for all 19 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

All four files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` (`src/nw4r/g3d/g3d_obj.cpp`, `g3d_anmobj.cpp`, `platform/g3d_gpu.cpp`, `platform/g3d_cpu.cpp`), stripped of comments, with their includes replaced. `include/nw4r/g3d/platform/g3d_gpu.h` is ss's header from the same commit, stripped of comments, including `math_types.h`, `<revolution/GX.h>` and `g3d_gxhardware.h`. `g3d_gpu.cpp` also includes `<revolution/gx/GXTransform.h>` for `GXLoadTexMtxImm`. `ZeroMemory16ByteBlocks`, missing from ss's `g3d_cpu.cpp` and already declared in this repository's `g3d_cpu.h`, follows CC0 Etrainquandt17/Whippy `src/nw4r/g3d/g3d_cpu.cpp` (`main`, fetched 2026-10-17): the 32-byte routine with a 16-byte stride and two paired stores.

## Outside symbols

The `G3dObj` vtable's `IsDerivedFrom` slot (`.data` `0x803C5E90`) points to `0x80053820`, inside game code. That function is byte-identical to the object's weak `G3dObj::IsDerivedFrom`, including its relocation to `TYPE_NAME` of `G3dObj`, and nothing else references it, so an earlier weak copy won the link, as with `AnmScnRes::GetLightSetMaxRefNumber` (F-401-2). `symbols.txt` names it `IsDerivedFrom__Q34nw4r3g3d6G3dObjCFQ44nw4r3g3d6G3dObj7TypeObj` with weak scope.

## Traps

None new. F-401-2's trap applies again: a weak copy of an inline `G3dObj` member in game code can stand in for the unit's copy.
