# F-394-1: NW4R g3d scene animation units and `g3d_transform.cpp`

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #394 owns `.text` `0x80253A90–0x80255504`. Its first two functions, `ChrAnmResult::GetMtx` and `SetRotTrans`, end `g3d_resanmchr.cpp` and were matched with #393 ([F-393-1](393-g3d-resnode-resanm.md)). The rest is seven units and the start of `g3d_transform.cpp`, which ends with the first two functions of #395.

## Units and boundaries

| Unit | Section | Range | Functions |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_reslightset.cpp` | `.text` | `0x80253C80–0x80253DEC` | `ResLightSet::Bind` |
| `nw4r/g3d/res/g3d_resanmamblight.cpp` | `.text` | `0x80253DF0–0x80253EC0` | `ResAnmAmbLight::GetAnmResult` |
| | `.sdata2` | `0x804BAB40–0x804BAB50` | |
| `nw4r/g3d/res/g3d_resanmlight.cpp` | `.text` | `0x80253EC0–0x802542A8` | `ResAnmLight::GetAnmResult` |
| | `.sdata2` | `0x804BAB50–0x804BAB60` | |
| `nw4r/g3d/res/g3d_resanmfog.cpp` | `.text` | `0x802542B0–0x80254410` | `ResAnmFog::GetAnmResult` |
| | `.sdata2` | `0x804BAB60–0x804BAB70` | |
| `nw4r/g3d/res/g3d_resanmcamera.cpp` | `.text` | `0x80254410–0x80254720` | `ResAnmCamera::GetAnmResult` |
| `nw4r/g3d/res/g3d_resanmscn.cpp` | `.text` | `0x80254720–0x802552AC` | 22 `ResAnmScn` functions |
| | `.data` | `0x803C5940–0x803C59E0` | five `ResNameData` dictionary names |
| `nw4r/g3d/res/g3d_resanmshp.cpp` | `.text` | `0x802552B0–0x80255418` | `ResAnmShp::GetAnmResult` |
| | `.sdata2` | `0x804BAB70–0x804BAB78` | |
| `nw4r/g3d/g3d_transform.cpp` | `.text` | `0x80255420–0x80255724` | `detail::CalcViewNrmMtx`, `CalcViewTexMtx`, `CalcInvWorldMtx` |
| | `.sdata2` | `0x804BAB78–0x804BAB7C` | `sEpsilon` |

`CalcViewTexMtx` (`0x80255510`) and `CalcInvWorldMtx` (`0x80255620`) are the first two functions of #395, which continues at `0x80255730`. The `.sdata2` symbol at `0x804BAB78` was resized from 8 to 4 bytes so the transform split ends after its constant. objdiff reports 100% for all 31 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

All eight files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` sources, stripped of comments. Seven compile to the target bytes unchanged. `ResAnmFog::GetAnmResult` is linked here although ss's linker stripped it.

`g3d_resanmscn.cpp` differs. This revision has five top-level dictionary groups, light sets, ambient lights, lights, fogs and cameras, and each has a `NumEntries` accessor. The linked functions are, in address order: `HasResLightSet`, `HasResAnmAmbLight`, `HasResAnmLight` and `HasResAnmCamera`; `GetResLightSetNumEntries`; `GetResAnmAmbLight(ResName)`, `GetResAnmAmbLight(u32)` and `GetResAnmAmbLightNumEntries`; the same three for lights; `GetResAnmFog(u32)` and `GetResAnmFogNumEntries`; `GetResAnmCamera(int)`, `GetResAnmCamera(u32)` and `GetResAnmCameraNumEntries`; the five `ByRefNumber` accessors; and `Bind`. The source defines every `Has`, name, index and `NumEntries` accessor of the five groups in that order, and the linker strips the unreferenced ones. Each data reference identifies its group: light sets use the name at `0x803C5940`, ambient lights `0x803C5960`, lights `0x803C5980`, fogs `0x803C59A0` and cameras `0x803C59C0`. The `int` and `u32` index accessors compile to identical code. Where only one is linked, it is named `u32`, following the ogws (Wii Sports) symbol map, which keeps the `u32` versions.

The `g3d_transform.cpp` assembly labels are renamed `_invertible` and `_singular_done`. ss's epsilon constant `dat_epsilon` is named `sEpsilon`; no original name is known.

## Traps

None new. The g3d trap from F-386-1 applies again: `g3d_transform.cpp` starts in #394 and ends inside #395.
