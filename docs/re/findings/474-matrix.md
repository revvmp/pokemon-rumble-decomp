# Issue 474: VI encoder and matrix units

## F-474-1: unit boundaries and source provenance

For target `wpse-v1`, the matrix-vector unit owns `.text` `0x802C9590`-`0x802C96E0`. Its three functions are `PSMTXMultVec` (84 bytes), `PSMTXMultVecArray` (140 bytes), and `PSMTXMultVecSR` (84 bytes). It has no data or external references. The 28 remaining bytes are zero alignment padding. The next unit starts with `C_MTXFrustum` at `0x802C96E0` and ends after `C_MTXOrtho` at `0x802C9908`, with padding to `0x802C9910`; the following function is a vector operation, outside issue 474. The projection unit also owns 24 bytes of `.sdata2` at `0x804BB2B0`-`0x804BB2C8`: 1, 2, 0, -1, 0.5, and the degrees-to-radians multiplier, all single precision.

Source provenance is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, whose repository LICENSE is CC0 1.0. `mtxvec.c` follows its paired-single routines; the array routine includes translation, unlike the reference's retained `PSMTXMultVecArraySR`. The translation terms are merged from each row's fourth component, and the last vector component is loaded with paired-single W=1. `mtx44.c` follows the reference perspective and orthographic C functions; the frustum C function is reconstructed from the target's projection arithmetic. The unused reference constant-order helper is unnecessary because the frustum naturally establishes the observed order. The imported Petari matrix header provides the shared ABI; this issue adds only the missing C_MTXFrustum and PSMTXMultVecArray declarations.

Wii/1.0 with existing `cflags_sdk` produces 100% objdiff for all six functions and all projection data. `mtxvec.c` is source-linked and full frozen-manifest verification passes. Its three routines are inline assembly and must be reported separately from reconstructed C coverage; no raw machine-code blobs or original object fallback remain for that unit. PR 556 initially retained `mtx44.c` as `NonMatching` because the external `tan` name was outside the then-permitted scope. Under the updated outside-symbol rule, F-474-4 names that referenced callee and validates linking all 544 C instruction bytes from source. Both matrix units now link with exact full-DOL verification.

At the initial partial delivery, 31 functions remained original fallbacks. F-474-5 and F-474-6 complete those functions and supersede that coverage status. Neighboring issue 475 owns the vector functions beginning at `0x802C9910`; issue 473 covers the preceding VI I2C source, with PR 549 documenting those boundaries.

## Traps

### F-474-2: selecting the paired-single disassembler

GNU binutils 2.42 `powerpc-eabi-objdump -d` on the linked ELF interpreted several paired-single instructions as newer VSX/AltiVec operations, for example the first `psq_l` in `PSMTXMultVec` as `lq`. Explicit `-m powerpc:750` decodes these instructions correctly. Use that architecture when comparing SDK matrix instructions. The final array mismatch was the unused second output lane source of `ps_sum0`: the target encodes `ps_sum0 f13,f10,f9,f10`; changing only the unused operand still changes bytes and is not an exact match.

### F-474-3: next-unit alignment after source replacement

The first source-linked vector build failed the frozen manifest despite all functions scoring 100%. The compiler emits the final return at `0x802C96D0` and ends the section at `0x802C96D4`. The next original fallback object had only four-byte alignment, so its `C_MTXFrustum` moved to `0x802C96D4` instead of `0x802C96E0`, shifting subsequent code by 12 bytes. The owned projection text split therefore explicitly specifies `align:16`. The failure was diagnosed from `powerpc-eabi-nm -n build/wpse-v1/main.elf`, the full verification failure at DOL offset `0x6F`, and the link's SDK function addresses. No padding code or dummy function was added.

### Imported header weak attribute

The shared imported matrix header used `ATTRIBUTE_WEAK` for `PSMTXRotRad`, but the project had no definition for that macro. The first clean matrix compile rejected that declaration. This issue spells the existing weak attribute as `__attribute__((weak))`, preserving its semantics without adding a new shared macro.

## Validation

The initial split-only build passed full frozen-manifest verification. Source validation uses `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. Objdiff 3.8.2 reports three matching functions per unit. No expected hashes, originals, or outside-claim symbols were changed.

## F-474-4: projection source link and tan dependency

The updated take-issue procedure permits evidenced names for external symbols referenced by the claimed unit, while retaining their original sizes, types and splits. `C_MTXPerspective` at `0x802C9780` calls `0x8034D4B8` at `0x802C97C8`; the matching Petari projection source calls the double-precision `tan` function there. Naming that symbol `tan` supplies the source object's only unresolved call. Its size remains `0x78`; no external unit was split or reconstructed.

The projection `.text` split now ends at the final function end `0x802C9908`, leaving the eight alignment bytes before the next vector function to the automatic object. The earlier split included those bytes through `0x802C9910`, which the compiler does not emit at the end of the translation unit. The unit retains its evidenced 16-byte starting alignment.

With these changes and `Object(Matching, "RVL_SDK/mtx/mtx44.c")`, all three projection functions and all 24 data bytes are 100% in objdiff. Configure, ninja and the full frozen-manifest verifier exit 0. The executable links 544 additional C instruction bytes; the prior matrix-vector unit contributes 308 inline-assembly instruction bytes, separately disclosed. Thirty-one issue functions remain original fallbacks. No new trap beyond the already documented trailing-alignment behavior arose.

## F-474-5: complete affine matrix unit

`mtx.c` owns text `0x802C8A80`-`0x802C9588`, eight bytes of `Unit01` at `0x804B8B18`, and 28 scalar bytes at `0x804BB290`. All seventeen functions compare at 100%, and linking the source passes the frozen full-DOL manifest. The seventeen functions contain 2,716 instruction bytes: eleven paired-single inline-assembly routines account for 1,548 bytes; six C routines account for 1,168 bytes. They are separately classified rather than presenting inline assembly as reconstructed C.

The reference is the same pinned CC0 Petari tree as F-474-1. Its absent `PSMTXConcatArray`, `PSMTXRotRad`, and `C_MTXLightFrustum` implementations are supplied from CC0 zeldaret/tww commit `8b93b372dc8e841f12ddfb7e0215626f62b05417`, `src/dolphin/mtx/mtx.c`, omitting debug assertions. Every retained function is checked against this target. `PSMTXInverse` retains the original paired-single inverse algorithm, with target-observed instruction scheduling and reciprocal refinement ordering. No original object bytes are embedded.

`PSMTXIdentity` declares zero and one registers in that order but initializes one first, reproducing both register allocation and scalar order. The final scalar is a four-byte degrees-to-radians value, followed by four bytes of alignment; the previous inferred eight-byte symbol incorrectly included padding. The external names `sin` at `0x8034D3EC`, `cos` at `0x8034CEE4`, `PSVECNormalize` at `0x802C9940`, and `PSVECCrossProduct` at `0x802C9A00` follow reference call sites and target instruction behavior. Only their names change; their sizes, types and splits remain untouched.

## F-474-6: complete VI encoder unit

`vi3in1.c` owns text `0x802C7350`-`0x802C8A7C`, data `0x803D5890`-`0x803D5EA0`, BSS `0x80453B60`-`0x80453B7C`, small data `0x804B8B08`-`0x804B8B18`, and small BSS `0x804B9680`-`0x804B9698`. All fourteen C functions compare at 100%, accounting for 5,868 instruction bytes and 1,620 reported data bytes including padding. All sections compare at 100%, and full-DOL verification passes with this source linked.

The code and tables come from the pinned CC0 Petari `src/RVL_SDK/vi/vi3in1.c`. Original header types and declarations reuse the imported `revolution/vi.h`, extended by neighboring PR 603. `VIGammaObj` is 34 bytes: six u16 coefficients, seven u8 inputs, one byte of alignment, and seven u16 outputs. `VIMacroVisionObj` is 26 bytes. The 31 gamma entries begin at `0x803D5890`; thirteen macrovision tables follow at offset `0x420`, each with 26 bytes and two bytes of inter-object padding. `VIZeroACPType` is the 26-byte zero BSS object. Initialized control bytes and six small-BSS words are individually named from matching reference accesses.

The target additionally retains a sixteen-byte default-gamma wrapper at `0x802C8450` and clears a word at `0x804B9684` at the end of initialization. The wrapper adopts neighboring PR 603's `__VISetGamma1_0` name. `gEncoderState` is descriptive and is not claimed as a recovered original. Only the zeroing behavior of the latter is established; its broader meaning is unknown. Making that word volatile changes the epilogue instruction order; an ordinary external u32 reproduces the observed store and epilogue. The reference YUV routine must be inline-eligible to reproduce initialization's embedded region-selection switch.

The target data also retains `vi3in1.c` and the complete progressive-CGMS diagnostic string after the tables, although the referring debug routine is absent from the executable. These strings are represented as named, force-active diagnostic arrays in the source, preserving readable evidence rather than a machine-code blob. `VIGetDTVStatus` at `0x802C6670` is named from the reference call and the target's DTV-status operation; no external size, type or split changes.

### Completed issue coverage

All 37 original issue functions are source-linked across four units, with no original object fallback inside the issue: 23 C functions / 7,580 instruction bytes and 14 routines containing SDK inline assembly / 1,856 instruction bytes. Total is 9,436 instruction bytes. Callees in neighboring SDK and math-library units may still use their original fallback objects; this does not claim their reconstruction. Configure, ninja, and frozen-manifest full-DOL verification all exit zero. The source report records 37/37 matching functions and 100% data for every owned unit.

## Traps

### Gamma constants can be mistaken for a pointer

The adjacent gamma outputs `0x8000` and `0xA000` form the word `0x8000A000`. Automatic analysis inferred an executable relocation and zeroed those bytes in the relocatable comparison object. The original DOL and CC0 source both establish two scalar u16 outputs. The owned `gammaSet` symbol uses `noreloc` to prevent that false relocation, retaining the original scalar values and making its data comparison exact. Do not copy the relocation placeholder zeros into source.

### Preserve the VI initialization state store without volatile

An added volatile qualifier moved the return-address reload before the saved-register reloads and failed exact comparison despite equivalent observable behavior. A normal externally visible word preserves the target's final zero store and epilogue; the reconstructed name deliberately makes no claim about an unknown higher-level state meaning.
