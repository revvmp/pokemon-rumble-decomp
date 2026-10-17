# F-380-2: complete surveyed effect issue inventory

Issue #380 lists 25 functions and 9,500 instruction bytes from `0x80235CB0` through `0x80238264`. Its first eight functions are the utility tail delivered by [PR #885](https://github.com/revvmp/pokemon-rumble-decomp/pull/885), merged as `2409e83dc30d76f6771b280250b8bd12aed05d98`. The remaining 17 functions were previously reconstructed in the eight units documented in [F-380-1](380-ef-handle-emitter-forms.md). All nine owning units are Matching and source-linked.

The utility file starts in #770. [F-770-1](770-effect-utility.md) records its complete split, function/data evidence, chosen constant names, and Whippy/MH Tri CC0 provenance. Its paired-single assembly is explicitly accounted as assembly, not C++ decompilation credit. No new source unit, duplicate implementation, or additional source credit is introduced. The five owned vtable data extents are corrected to exclude alignment padding.

## Acceptance audit

The audit reads the live issue address/size table and the real default-Ninja report. It requires 25 distinct listed functions totaling 9,500 bytes, exact native sizes, 100% per-function comparison, source-linked owning units, and complete section comparison. Configure, default Ninja, and the frozen artifact verifier independently validate the complete executable. Exact committed-head commands and exit codes are recorded in the PR.

| Native address | Size | Current symbol | Owning unit |
| --- | --- | --- | --- |
| `0x80235CB0` | `0x74` | `_PSSinCosRad__Q24nw4r2efFPff` | `main/nw4r/ef/ef_util` |
| `0x80235D30` | `0x78` | `PSSinCosRad__Q24nw4r2efFPfPff` | `main/nw4r/ef/ef_util` |
| `0x80235DB0` | `0x14C` | `Rotation2VecY__Q24nw4r2efFRCQ34nw4r4math4VEC3PQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_util` |
| `0x80235F00` | `0x178` | `PSMTX34RotXYZRad__Q24nw4r2efFPQ34nw4r4math5MTX34fff` | `main/nw4r/ef/ef_util` |
| `0x80236080` | `0x80` | `MTX34ScaleNonZero__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_util` |
| `0x80236100` | `0x88` | `MTX34ScaleNonZero__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3PCQ34nw4r4math5MTX34` | `main/nw4r/ef/ef_util` |
| `0x80236190` | `0xA8` | `MTX34ScaleInv__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_util` |
| `0x80236240` | `0x58` | `MTXColLen__Q24nw4r2efFPCQ34nw4r4math5MTX34i` | `main/nw4r/ef/ef_util` |
| `0x802362A0` | `0x10` | `__ct__Q34nw4r2ef10HandleBaseFv` | `main/nw4r/ef/ef_handle` |
| `0x802362B0` | `0x14` | `__as__Q34nw4r2ef10HandleBaseFRCQ34nw4r2ef10HandleBase` | `main/nw4r/ef/ef_handle` |
| `0x802362D0` | `0x28` | `__as__Q34nw4r2ef10HandleBaseFPQ34nw4r2ef12LinkedObject` | `main/nw4r/ef/ef_handle` |
| `0x80236300` | `0x2C` | `IsValid__Q34nw4r2ef10HandleBaseCFv` | `main/nw4r/ef/ef_handle` |
| `0x80236330` | `0x40` | `GetPtr__Q34nw4r2ef10HandleBaseCFv` | `main/nw4r/ef/ef_handle` |
| `0x80236370` | `0x4C0` | `CalcVelocity__Q34nw4r2ef11EmitterFormCFPQ34nw4r4math4VEC3PQ34nw4r2ef7EmitterRCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3RCQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_emitterform` |
| `0x80236830` | `0xD4` | `CalcLife__Q34nw4r2ef11EmitterFormFUsfPQ34nw4r2ef7Emitter` | `main/nw4r/ef/ef_emitterform` |
| `0x80236910` | `0xC` | `__ct__Q34nw4r2ef13CreationQueueFv` | `main/nw4r/ef/ef_creationqueue` |
| `0x80236920` | `0x17C` | `AddParticleCreation__Q34nw4r2ef13CreationQueueFPCQ34nw4r2ef21EmitterInheritSettingPQ34nw4r2ef8ParticlePQ34nw4r2ef15EmitterResourceUsPCQ34nw4r4math4VEC3PCQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_creationqueue` |
| `0x80236AA0` | `0x180` | `AddEmitterCreation__Q34nw4r2ef13CreationQueueFPCQ34nw4r2ef21EmitterInheritSettingPQ34nw4r2ef8ParticlePQ34nw4r2ef15EmitterResourceUsPCQ34nw4r4math4VEC3PCQ34nw4r4math4VEC3` | `main/nw4r/ef/ef_creationqueue` |
| `0x80236C20` | `0x134` | `Execute__Q34nw4r2ef13CreationQueueFv` | `main/nw4r/ef/ef_creationqueue` |
| `0x80236D60` | `0x7C` | `Create__Q34nw4r2ef15EmitFormBuilderFQ34nw4r2ef12EmitFormType` | `main/nw4r/ef/ef_emform` |
| `0x80236DE0` | `0x58` | `__sinit_\ef_emform_cpp` | `main/nw4r/ef/ef_emform` |
| `0x80236E40` | `0x348` | `Emission__Q34nw4r2ef16EmitterFormPointFPQ34nw4r2ef7EmitterPQ34nw4r2ef15ParticleManageriUlPfUsfPCQ34nw4r4math5MTX34` | `main/nw4r/ef/ef_point` |
| `0x80237190` | `0x338` | `Emission__Q34nw4r2ef15EmitterFormLineFPQ34nw4r2ef7EmitterPQ34nw4r2ef15ParticleManageriUlPfUsfPCQ34nw4r4math5MTX34` | `main/nw4r/ef/ef_line` |
| `0x802374D0` | `0x534` | `Emission__Q34nw4r2ef15EmitterFormDiscFPQ34nw4r2ef7EmitterPQ34nw4r2ef15ParticleManageriUlPfUsfPCQ34nw4r4math5MTX34` | `main/nw4r/ef/ef_disc` |
| `0x80237A10` | `0x854` | `Emission__Q34nw4r2ef17EmitterFormSphereFPQ34nw4r2ef7EmitterPQ34nw4r2ef15ParticleManageriUlPfUsfPCQ34nw4r4math5MTX34` | `main/nw4r/ef/ef_sphere` |

## Traps

The blocked utility tail was part of a source file beginning in the neighboring #770 range. PR #885 reconstructs the full file there and resolves the former missing-reference/assembly gap. Closure here audits every listed function across nine existing source units and preserves the assembly distinction. Five single-method vtable symbols included four following alignment bytes each. Correcting their sizes and data split ends from 16 to 12 bytes makes their data sections 100% without changing the executable. Their two-word prefix plus one virtual pointer establishes the actual 12-byte objects; compiler-emitted unused weak base vtables do not belong in their native extents. No additional C++ or assembly instruction bytes are credited.


PowerShell's native pipeline used `us-ascii` in this session: an en dash inside a piped Python replacement script became a question mark before Python parsed it. `encoding="utf-8"` on file reads/writes does not fix input transcoding. Construct the separator with `chr(0x2013)` or write the script as UTF-8 before running it; the corrected replacement preserved the repository's existing UTF-8 text.
