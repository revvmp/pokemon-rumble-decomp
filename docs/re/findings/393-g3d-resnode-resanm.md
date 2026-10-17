# F-393-1: NW4R g3d `ResNode` and resource animation units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #393 owns `.text` `0x80251120–0x80253A84`. Its first function, `ResTex::Init`, ends `g3d_restex.cpp` and was matched with #392 ([F-392-1](392-g3d-resvtx-restex.md)). The rest is seven units. The last, `g3d_resanmchr.cpp`, ends with the first two functions of #394.

## Units and boundaries

| Unit | Section | Range | Functions |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_resnode.cpp` | `.text` | `0x80251130–0x80251460` | `ResNode::PatchChrAnmResult`, `CalcChrAnmResult`, `GetResUserData` |
| | `.sdata2` | `0x804BAAD0–0x804BAAE0` | |
| `nw4r/g3d/res/g3d_resanm.cpp` | `.text` | `0x80251460–0x802516A4` | `detail::GetResKeyFrameAnmResult`, `detail::GetResColorAnmResult` |
| | `.sdata2` | `0x804BAAE0–0x804BAAF8` | |
| `nw4r/g3d/res/g3d_resanmvis.cpp` | `.text` | `0x802516B0–0x802517D0` | `ResAnmVis::GetAnmResult` |
| | `.sdata2` | `0x804BAAF8–0x804BAB08` | |
| `nw4r/g3d/res/g3d_resanmclr.cpp` | `.text` | `0x802517D0–0x8025193C` | `ResAnmClr::GetAnmResult` |
| | `.sdata2` | `0x804BAB08–0x804BAB18` | |
| `nw4r/g3d/res/g3d_resanmtexpat.cpp` | `.text` | `0x80251940–0x80251D00` | anonymous `SearchFrame`, `ResAnmTexPat::GetAnmResult`, `Bind` |
| `nw4r/g3d/res/g3d_resanmtexsrt.cpp` | `.text` | `0x80251D00–0x80252114` | `ResAnmTexSrt::GetAnmResult` |
| | `.sdata2` | `0x804BAB18–0x804BAB20` | |
| `nw4r/g3d/res/g3d_resanmchr.cpp` | `.text` | `0x80252120–0x80253C7C` | 15 anonymous helpers (`GetAnmScale`, three `CalcAnimationFVS` instances, `GetAnmRotation`, `GetAnmTranslation`, eight `GetAnmResult_*`), `ResAnmChr::GetAnmResult`, `ChrAnmResult::GetRotTrans`, `GetMtx`, `SetRotTrans` |
| | `.rodata` | `0x8036C6F0–0x8036C710` | `gGetAnmResultTable` |
| | `.sdata2` | `0x804BAB20–0x804BAB40` | |

`ChrAnmResult::GetMtx` (`0x80253A90`) and `SetRotTrans` (`0x80253AF0`) are the first two functions of #394; the next unit begins at `0x80253C80`. objdiff reports 100% for all 30 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

All seven files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` sources, stripped of comments, and compile to the target bytes unchanged with one addition. The linker stripped `ResAnmTexPat::GetParent` and `Release` and seven `ChrAnmResult` accessors (`GetScale`, `GetRotateDeg`, `GetTranslate`, `SetScale`, `SetRotateDeg`, `SetTranslate`, `SetMtx`); the source keeps them.

- `ResNode::GetResUserData` (`0x80251440`, 0x20 bytes) is out of line after `CalcChrAnmResult`. It reads offset 0x6C, `toResUserData`, and matches the out-of-line `ResMat::GetResUserData` of F-390-1. ss has no such definition.
- The function sizes listed in ss's own symbol map differ from the target for `GetResKeyFrameAnmResult`, `GetResColorAnmResult`, `SearchFrame`, the `CalcAnimationFVS` instances and `GetAnmRotation`, but the same ss source compiled with Wii/1.1 produces the target sizes and bytes. ss uses a later compiler.
- `include/nw4r/g3d/platform/g3d_cpu.h` gains ss's `fastcast` conversions used by `g3d_resanmchr.cpp`. `include/revolution/os/OSFastCast.h` gains `OSu8tof32`, in the style of the existing `OSu16tof32`. `MTXIdentity` is written as `PSMTXIdentity`, the release-build SDK name.

## Traps

- **ss's symbol sizes are not this game's sizes.** Five functions here differ in size between ss's map and the target, yet ss's source reproduces the target with Wii/1.1. Compile the reference before concluding that a size difference means a different source revision.
- **The build rewrites unrelated `symbols.txt` entries.** On this base `ninja` rewrote DVD symbols far outside this issue's ranges. Keep only rewrites in your own ranges before committing.
