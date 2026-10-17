# F-774-2: Complete G3D state source reconstruction

## Scope and provenance

Issue #774 owns the complete `g3d_state.cpp` source unit at `0x8025B6A0`–`0x8025ECB0`: 42 retained functions and 13,644 function bytes. It replaces the unmatched remainder of the older survey blocks #403 and #404; this work does not independently close or count those older issues.

Source comes from zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, whose root license is CC0-1.0. The source and reused headers have no separate file-level notices. Comments are removed, C++ null pointers use `nullptr`, platform headers use the existing Revolution interfaces, and the source retains the original public classes and anonymous state classes. The shared light/GPU/MSL prerequisites are documented in [F-774-1](774-g3d-state-headers.md). `g3d_dcc.h` is the canonical prerequisite supplied by #398 / PR #749.

## Compiler and native proof

The existing Wii/1.1 `cflags_nw4r` profile reproduces all 42 native function sizes and every instruction, including 445 decoded text relocation sites. Four vtable pointers, the default indirect-operation pointer and the static-constructor entry bring the paired relocation total to 451. The 48-byte environment matrix, complete 40-byte SDA2 pool, 24-byte retained vtable and eight initialized SDA bytes also match the native executable. No assembly, raw instruction bytes, extra flags or function-alignment override are used.

The source uses the canonical `GX_PTTEXMTX0` spelling for the reference's `GX_DUALMTX0` value. The no-fog call uses the existing canonical GXSetFog parameter order. Native code proves both adaptations. Unused alignment fields in TexState and TlutState are omitted; natural class padding preserves their sizes and all instructions. IndMtxOpStd retains the byte explicitly cleared by its constructor, named `mReserved` instead of the reference's offset-based padding name. Its interpretation as reserved storage is descriptive; no additional behavior is inferred.

## Complete unit extents

| Section | Native start | Native end | Evidence |
| --- | --- | --- | --- |
| `.text` | `0x8025B6A0` | `0x8025ECB0` | 42 functions through the complete static initializer |
| `.ctors` | `0x80352A4C` | `0x80352A50` | Pointer to `0x8025EA40` |
| `.rodata` | `0x8036C858` | `0x8036C888` | EnvironmentMapping local matrix |
| `.data` | `0x803C5EA8` | `0x803C5EC0` | IndMtxOpStd virtual table |
| `.bss` | `0x8042F9C0` | `0x80434F64` | All state objects through the local normal matrix |
| `.sdata` | `0x804B8968` | `0x804B8970` | Sentinel and default-operation pointer |
| `.sbss` | `0x804B92F8` | `0x804B9318` | Small states and local-matrix guard |
| `.sdata2` | `0x804BAC38` | `0x804BAC60` | Complete native constant pool |

The earlier placeholder BSS objects included alignment padding and merged ShpState into the preceding TlutState. Actual object sizes separate those allocations. The last normal matrix is `0x24` bytes; the following padding up to `0x80434F80` belongs to the next unit.

Four referenced Fog callees outside this issue receive names only: constructor `0x80270A90`, Init `0x80270AA0`, CopyTo `0x80270B10`, and SetGP `0x80270B90`. The canonical existing Fog header and pinned source give their signatures, and native state-unit branch relocations independently select these exact addresses. Their splits, sizes and types remain unchanged.

The compiler folds the original empty LightObj and LightSetting destructors at one source offset. The native initializer passes `0x8025C050` as the destructor for 128 LightObj entries of size `0x44`, alongside the LightObj constructor at `0x8025C090`. This directly proves LightObj::~LightObj as a native identity for that function. The imported survey map's LightSetting spelling describes the folded alias; the native primary name is changed to LightObj to reflect the actual array reference. No source declaration, destructor body, code size or instruction is forced to change.

## Validation status

Baseline configure, Ninja and explicit frozen verification passed, with 408 baseline source-object hashes recorded. The shared-header prerequisite passed its own fresh full build. After the DCC prerequisite merged, the eight-section NonMatching split and the actual registered source compilation passed configure, full Ninja and explicit actual-directory frozen verification in `build/774-source-eval/wpse-v1`. The actual source object independently reproduces all 42 native functions and 451 relocation targets. Full source-selected acceptance remains the next gate.

## Traps

- The exact original MSL find loop is necessary for the native LoadLightSet instruction stream; see F-774-1.
- This unit owns both normal and small BSS state. Ending the normal BSS split at the next object's aligned start incorrectly includes padding; use the final matrix's actual end.
- The default Ninja target also compiles registered NonMatching source objects. A missing prerequisite header therefore fails the build even though the final link would select the original object. Wait for the canonical prerequisite rather than substituting a local declaration or claiming a stale DOL verifies the new source.
