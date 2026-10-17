# F-401-1: NW4R g3d `AnmScn` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #401 owns `.text` `0x80259740–0x8025A244`. Its first two functions, `AnmObjShp::GetTypeName` (`0x80259740`) and `GetTypeObj` (`0x80259770`), end `g3d_anmshp.cpp`, which #400 owns. The rest of #401 begins `g3d_anmscn.cpp`, which runs through `0x8025ACFC`, inside #402.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_anmscn.cpp` | `.text` | `0x80259780–0x8025ACFC` | 42 functions: the anonymous-namespace `MakeDiffuseLightObj`, `AnmScn` (`GetLightSetting`, `Attach`, `Detach`, the destructor, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`) and `AnmScnRes` |
| | `.rodata` | `0x8036C818–0x8036C838` | `TYPE_NAME` of `AnmScn` and `AnmScnRes` (0x10 each with padding) |
| | `.data` | `0x803C5D68–0x803C5E88` | vtables of `AnmScnRes` and `AnmScn` (0x90 each) |
| | `.sdata2` | `0x804BABF0–0x804BAC0C` | float and double constants; the split ends at the last constant, leaving four bytes of padding to the next unit |

`g3d_obj.cpp` follows at `0x8025AD00` with `G3dObj::~G3dObj`. objdiff reports 100% for all 42 functions and all 348 data bytes, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` has only `AnmScn::GetLightSetting` in `g3d_anmscn.cpp`. The full unit comes from CC0 Etrainquandt17/Whippy (Kirby's Return to Dream Land) `src/nw4r/g3d/g3d_anmscn.cpp` at `main` (fetched 2026-10-17), stripped of comments. Changes:

- Whippy declares `AnmScnRes` in the source file because of its header order; here it moves into `include/nw4r/g3d/g3d_anmscn.h`, which now includes `g3d_resanmscn.h`. The header is ss's, stripped of comments, with Whippy's `Detach(int idx)` signature. The mangled parameter list is not observable in the binary; the body at `0x80259D00` matches either signature.
- `ut::RoundUp(x, 4)` becomes `align4(x)` from `g3d_obj.h`, because `<revolution/mem.h>` defines `RoundUp` as a macro.
- `AnmScnRes::GetLightSetMaxRefNumber` is defined inline in the class (see F-401-2).
- `include/nw4r/g3d/g3d_light.h` is ss's header from the same commit, stripped of comments, with `<revolution/GX.h>`.
- `include/algorithm` is new and holds only MSL's `std::min` and `std::max`, as in ss's MSL C++ `algorithm`.

The linker stripped the out-of-line `AnmScn` constructor and the weak `LightSet` and `ut::Color` destructors.

## F-401-2: a game unit supplies `GetLightSetMaxRefNumber`

The `AnmScnRes` vtable slot for `GetLightSetMaxRefNumber` (`.data` `0x803C5DB8`) points to `0x8005ED80`, a 0xC-byte function inside game code, and nothing else references that address. Its body, `lwz r3,0x20(r3); lhz r3,0x3C(r3); blr`, is the accessor for `ResAnmScn`'s light-set limit, one offset below the other limit accessors at `0x80259DB0` (`0x3E`), `0x80259DC0` (`0x40`) and `0x80259DD0` (`0x42`). The `g3d_anmscn.cpp` range has only four limit accessors, so the unit's copy is a weak definition that the linker dropped in favour of an earlier weak copy in a game unit. Defining the accessor inline in the class reproduces this: the object emits it weak at the end of `.text`, and the link keeps the copy at `0x8005ED80`, which `symbols.txt` names `GetLightSetMaxRefNumber__Q34nw4r3g3d9AnmScnResCFv` with weak scope. Confidence: high for the identity (vtable slot order and body); medium that the original header declared only this accessor inline.

## F-401-3: Complete original issue acceptance

The complete original #401 inventory is now supplied by two already-merged deliveries: #818 (`f2698da863dee2512c6e84b94c1e05b1b1a47aa3`) provides its 24 AnmScn functions, and #785 (`3e02cc0e9675d6daf6658b7baa71a92bf276c5b3`) provides the two preceding AnmObjShp RTTI methods. All 26 original functions / 2,660 native bytes are reconstructed, in established Matching units, and selected from actual source objects. This acceptance adds zero reconstructed functions or bytes; both deliveries retain their existing credit.

| Original address | Recovered function | Native size |
| --- | --- | --- |
| `0x80259740` | `AnmObjShp::GetTypeName` | `0x30` |
| `0x80259770` | `AnmObjShp::GetTypeObj` | `0xC` |
| `0x80259780` | `MakeDiffuseLightObj` | `0x238` |
| `0x802599C0` | `AnmScn::GetLightSetting` | `0x328` |
| `0x80259CF0` | `AnmScn::Attach` | `0x8` |
| `0x80259D00` | `AnmScn::Detach` | `0x8` |
| `0x80259D10` | `~AnmScn` | `0x3C` |
| `0x80259D50` | `AnmScnRes::GetNumLightSet` | `0x8` |
| `0x80259D60` | `AnmScnRes::GetNumAmbLight` | `0x8` |
| `0x80259D70` | `AnmScnRes::GetNumDiffuseLight` | `0x8` |
| `0x80259D80` | `AnmScnRes::GetNumSpecularLight` | `0xC` |
| `0x80259D90` | `AnmScnRes::GetNumFog` | `0x8` |
| `0x80259DA0` | `AnmScnRes::GetNumCamera` | `0x8` |
| `0x80259DB0` | `AnmScnRes::GetAmbLightMaxRefNumber` | `0xC` |
| `0x80259DC0` | `AnmScnRes::GetDiffuseLightMaxRefNumber` | `0xC` |
| `0x80259DD0` | `AnmScnRes::GetFogMaxRefNumber` | `0xC` |
| `0x80259DE0` | `AnmScnRes::GetCameraMaxRefNumber` | `0xC` |
| `0x80259DF0` | `AnmScnRes::Construct` | `0x194` |
| `0x80259F90` | `AnmScnRes` | `0xFC` |
| `0x8025A090` | `~AnmScnRes` | `0x40` |
| `0x8025A0D0` | `AnmScnRes::SetFrame` | `0x58` |
| `0x8025A130` | `AnmScnRes::GetFrame` | `0x8` |
| `0x8025A140` | `AnmScnRes::SetUpdateRate` | `0x24` |
| `0x8025A170` | `AnmScnRes::GetUpdateRate` | `0x8` |
| `0x8025A180` | `AnmScnRes::UpdateFrame` | `0x78` |
| `0x8025A200` | `AnmScnRes::G3dProc` | `0x44` |

The only source adjustment normalizes 27 legacy C++ null-pointer tokens to `nullptr`, as required by the project's source standard. No runtime statements, class declarations, unit boundaries, symbol metadata, flags or frozen hashes change. The entire affected AnmScn unit is independently rechecked, including its #402-owned tail: all 42 functions / 5,220 native bytes, 117 instruction relocation targets and 348 initialized data bytes reproduce the frozen original. Every allocated section of both AnmScn and AnmObjShp scores 100% in actual-object objdiff.

The separate original-inventory audit checks every one of the 26 listed body sizes and bytes, resolves all 59 instruction relocations, checks actual linker inputs for both reconstructed units, and verifies native final ELF addresses and extents. No instruction word is excluded from this comparison. The full source-selected build passes `python configure.py`, plain Ninja and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, each with exit zero. Its complete DOL remains byte-identical to the frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Comparing all 464 previously compiled source objects before and after the pointer-spelling cleanup finds identical code, data and relocation targets, including AnmScn. Existing ledger rows and complete headers already cover both units; the AnmScn row links this final acceptance without adding another unit or progress count. No additional binary or toolchain trap emerged during this audit; the existing neighboring-file boundary and vtable lessons above remain applicable.

The latest-main integration gate uses the previously clean output
`build/401-merge1` after rebasing the acceptance onto `c8a34339`, inheriting
main metadata without a three-way metadata merge or shared-header conflict.
Configure, Ninja, the actual-directory frozen verifier, both complete native
audits, all allocated-section comparisons, and the unchanged 464-object
baseline pass with exit zero.

## Traps

- **A vtable slot can point outside its unit.** A function count one short of the reference, with a vtable of the reference's size, meant a weak copy elsewhere had won the link. Dump the target vtable and check each slot's address against the unit range before assuming a different source revision.
- **ss's `g3d_anmscn.cpp` is incomplete.** It holds only `GetLightSetting`; the rest of the unit, including all of `AnmScnRes`, needs another reference.
