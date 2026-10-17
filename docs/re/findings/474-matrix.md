# Issue 474: matrix-vector and projection units

## F-474-1: unit boundaries and source provenance

For target `wpse-v1`, the matrix-vector unit owns `.text` `0x802C9590`-`0x802C96E0`. Its three functions are `PSMTXMultVec` (84 bytes), `PSMTXMultVecArray` (140 bytes), and `PSMTXMultVecSR` (84 bytes). It has no data or external references. The 28 remaining bytes are zero alignment padding. The next unit starts with `C_MTXFrustum` at `0x802C96E0` and ends after `C_MTXOrtho` at `0x802C9908`, with padding to `0x802C9910`; the following function is a vector operation, outside issue 474. The projection unit also owns 24 bytes of `.sdata2` at `0x804BB2B0`-`0x804BB2C8`: 1, 2, 0, -1, 0.5, and the degrees-to-radians multiplier, all single precision.

Source provenance is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, whose repository LICENSE is CC0 1.0. `mtxvec.c` follows its paired-single routines; the array routine includes translation, unlike the reference's retained `PSMTXMultVecArraySR`. The translation terms are merged from each row's fourth component, and the last vector component is loaded with paired-single W=1. `mtx44.c` follows the reference perspective and orthographic C functions; the frustum C function is reconstructed from the target's projection arithmetic. The unused reference constant-order helper is unnecessary because the frustum naturally establishes the observed order. The imported Petari matrix header provides the shared ABI; this issue adds only the missing C_MTXFrustum and PSMTXMultVecArray declarations.

Wii/1.0 with existing `cflags_sdk` produces 100% objdiff for all six functions and all projection data. `mtxvec.c` is source-linked and full frozen-manifest verification passes. Its three routines are inline assembly and must be reported separately from reconstructed C coverage; no raw machine-code blobs or original object fallback remain for that unit. PR 556 initially retained `mtx44.c` as `NonMatching` because the external `tan` name was outside the then-permitted scope. Under the updated outside-symbol rule, F-474-4 names that referenced callee and validates linking all 544 C instruction bytes from source. Both matrix units now link with exact full-DOL verification.

The other 31 functions in issue 474 remain original fallbacks, covering vi3in1 and the earlier matrix unit. This partial implementation does not close the issue. Neighboring issue 475 owns the vector functions beginning at `0x802C9910`; issue 473 covers the preceding VI I2C source, with PR 549 documenting those boundaries.

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
