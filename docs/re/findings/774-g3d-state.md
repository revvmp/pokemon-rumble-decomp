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

Baseline configure, Ninja and explicit frozen verification passed, with 408 initial baseline source-object hashes recorded. The shared-header prerequisite passed its own fresh full build. After the DCC prerequisite merged, the eight-section NonMatching split and the actual registered source compilation passed configure, full Ninja and explicit actual-directory frozen verification in `build/774-source-eval/wpse-v1`.

After selecting the source object, full Ninja and the explicit actual-directory frozen verifier both exited 0. All eight objdiff sections score 100%. The actual source object reproduces all 42 native functions and 451 relocation targets; the final ELF independently confirms every native function address/extent and all 32 named data definitions. The actual Ninja link input includes the compiled state object and excludes its original object. All 438 preexisting source objects from the verified `a63b4b94` baseline remain byte-identical. The 4,094,880-byte DOL matches the frozen executable with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

```text
python configure.py --build-dir build/774-source-eval
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/774-source-eval/wpse-v1
```

The source is Matching. PR #816 supplies the two canonical MSL headers; final integration must rebase onto that prerequisite and the latest main, repeat the committed-head gate and recheck the complete original #774 inventory before merging.

## State storage and public layouts

| Native address | Size | Original state object |
| --- | --- | --- |
| `0x8042F9C0` | `0xC` | sGenMode2 |
| `0x8042F9CC` | `0x6C` | sTexCoordScale |
| `0x8042FA40` | `0x104` | sTex |
| `0x8042FB60` | `0x64` | sTlut |
| `0x8042FBC4` | `0xC` | sShp |
| `0x8042FBD0` | `0x20` | sPreTexMtxState |
| `0x8042FBF0` | `0xC` | sPosNrmArrayState |
| `0x8042FC00` | `0x800` | sScnDependentTexMtxFuncTable |
| `0x80430400` | `0x608` | sFogState |
| `0x80430A08` | `0x2A2C` | sLightState |
| `0x80433434` | `0x1A34` | sCameraMtxState |
| `0x80434E68` | `0x3C` | sRenderMode |
| `0x80434EA8` | `0x98` | IndMtxOpDefault |
| `0x80434F40` | `0x24` | GetViewNrmMtxPtr local matrix |

IndTexMtxInfo is `0x94`: a u32 flag at zero and three `0x30`-byte matrices at four. IndMtxOpStd is `0x98`: vptr at zero, three validity bytes at four, the constructor-cleared reserved byte at seven and three matrices at eight. Its native vtable order is operator(), destructor, Reset, SetNrmMapMtx. LightState contains LightSetting at zero, current set index at `0x10`, four masks at `0x14`–`0x20`, eight loaded-light indices at `0x24`, 128 LightObj entries at `0x2C`, 128 ambient entries at `0x222C` and 128 twelve-byte light-set records at `0x242C`.

## Traps

- The exact original MSL find loop is necessary for the native LoadLightSet instruction stream; see F-774-1.
- This unit owns both normal and small BSS state. Ending the normal BSS split at the next object's aligned start incorrectly includes padding; use the final matrix's actual end.
- The default Ninja target also compiles registered NonMatching source objects. A missing prerequisite header therefore fails the build even though the final link would select the original object. Wait for the canonical prerequisite rather than substituting a local declaration or claiming a stale DOL verifies the new source.
- Imported reference names can choose a folded destructor alias. The native constructor-array element size, constructor pointer and destructor pointer prove the consumed class identity; preserve the original declarations and record both aliases rather than reshaping source to fit a survey name.

After expanding the shared prerequisite for #778, the complete state source was restacked onto header head `24518d4e`, based on main `48556fb1`. Shared split/config conflicts were resolved by preserving all main registrations and placing state after the CPU unit and before draw1mat1shp. A fresh `build/774-expanded-source/wpse-v1` build passed Ninja and explicit actual-output frozen verification. All eight sections remained at 100%, all 451 native relocation sites and all 32 data allocations remained exact, and all 454 baseline source objects were byte-identical. The original forty-two-row issue inventory and input-preservation audit passed; every main object/eleven flag profiles was preserved, with only the evidenced private reserved-byte name changed in an existing header. The source PR remains draft until the actual shared prerequisite merges and its final latest-main gate is repeated.
