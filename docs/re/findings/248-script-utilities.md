# F-248-1: Complete script utility producer

Original #248 contains 28 functions, 5020 instruction bytes, from 0x80145AA0 through 0x80146EE0. `src/game/ScriptUtilities.cpp` reconstructs all 23 script bindings and naturally emits the five owned CTextWorldObjStat ownership helpers. The filename is descriptive; no original source filename was recovered. The 23 binding spellings are original names from `docs/re/script-bindings.tsv`.

The preceding binding finishes at 0x80145A94. This producer starts at the next 16-byte entry, 0x80145AA0. Its four-float constant cluster starts at 0x804BA240. Its CTextWorldObjStat control-block vtable, names, base descriptor and RTTI form a complete adjacent cluster at 0x803B6DF0–0x803B6EAC and 0x804B7A30–0x804B7A40. The five natural ownership helpers finish at 0x80146EE0, where the next model-motion binding begins. No following #249 function is selected or reconstructed here. These boundaries are native metadata/code boundaries, independent of the survey's estimated bind-file names.

## Native inventory

| Address | Bytes | Source function |
| --- | --- | --- |
| 0x80145aa0 | 144 | `NormalizeDeg__FPQ24NPpu7CEngine` |
| 0x80145b30 | 144 | `NormalizeDegPositive__FPQ24NPpu7CEngine` |
| 0x80145bc0 | 192 | `RandInt__FPQ24NPpu7CEngine` |
| 0x80145c80 | 208 | `RandReal__FPQ24NPpu7CEngine` |
| 0x80145d50 | 128 | `RandB__FPQ24NPpu7CEngine` |
| 0x80145dd0 | 148 | `Floor__FPQ24NPpu7CEngine` |
| 0x80145e70 | 148 | `Ceil__FPQ24NPpu7CEngine` |
| 0x80145f10 | 144 | `Round__FPQ24NPpu7CEngine` |
| 0x80145fa0 | 152 | `SinDeg__FPQ24NPpu7CEngine` |
| 0x80146040 | 152 | `CosDeg__FPQ24NPpu7CEngine` |
| 0x801460e0 | 156 | `TanDeg__FPQ24NPpu7CEngine` |
| 0x80146180 | 300 | `Atan2Deg__FPQ24NPpu7CEngine` |
| 0x801462b0 | 212 | `Sqrt__FPQ24NPpu7CEngine` |
| 0x80146390 | 148 | `Abs__FPQ24NPpu7CEngine` |
| 0x80146430 | 220 | `Fmod__FPQ24NPpu7CEngine` |
| 0x80146510 | 220 | `Pow__FPQ24NPpu7CEngine` |
| 0x801465f0 | 144 | `Pow2__FPQ24NPpu7CEngine` |
| 0x80146680 | 280 | `Div__FPQ24NPpu7CEngine` |
| 0x801467a0 | 140 | `Int__FPQ24NPpu7CEngine` |
| 0x80146830 | 580 | `method_showTextWorldObjStat__FPQ24NPpu7CEngine` |
| 0x80146a80 | 224 | `method_enableBoundingVolume__FPQ24NPpu7CEngine` |
| 0x80146b60 | 264 | `method_setMtx34__FPQ24NPpu7CEngine` |
| 0x80146c70 | 148 | `method_unsetMtx34__FPQ24NPpu7CEngine` |
| 0x80146d10 | 76 | `get_deleter__Q43std3tr16detail90shared_ptr_deleter<17CTextWorldObjStat,Q33std6detail35default_delete<17CTextWorldObjStat>>CFRCQ23std9type_info` |
| 0x80146d60 | 36 | `dispose__Q43std3tr16detail90shared_ptr_deleter<17CTextWorldObjStat,Q33std6detail35default_delete<17CTextWorldObjStat>>Fv` |
| 0x80146d90 | 132 | `__dt__Q33std3tr131shared_ptr<17CTextWorldObjStat>Fv` |
| 0x80146e20 | 116 | `__dt__Q210Metrowerks80move_ptr<17CTextWorldObjStat,Q33std6detail35default_delete<17CTextWorldObjStat>>Fv` |
| 0x80146ea0 | 64 | `__dt__Q43std3tr16detail90shared_ptr_deleter<17CTextWorldObjStat,Q33std6detail35default_delete<17CTextWorldObjStat>>Fv` |

## Contracts and source fidelity

The numeric functions use the existing typed NPpu stack API, native MathUtil/Random functions, normal mathematical calls and NW4R angle functions. `Abs` uses the compiler's existing `__fabs` intrinsic, also used by canonical NW4R headers. `TanDeg` uses the native single degree-to-radian conversion. Negative square-root operands return zero; zero itself follows the mathematical path. `Div` writes quotient and remainder through the popped NPpu reference and returns the script success value.

The object functions use complete canonical CPpuEngine, BModelObj, CTextWorldObjStat and IGameMap interfaces. Matrix values populate the public MTX34 `a` array. The text-kind local is `s32`, the project's signed-long SDK type; this preserves the native load scheduling while its existing Load formal remains `int`. No class layout, flags, register bindings or artificial operations were added.

Two necessary additive interfaces accompany the source. `CEngine::PopWString` scopes a real CValue temporary around Pop/ToWString, preserving the native temporary's destructor boundary before object lookup. Its spelling is descriptive. `IWorldObjStatDisplay::Add(BWorldObj*)` is the pure virtual slot at byte 0x1C proved independently by the CWorldObjStatDisplay producer (#219) and this actual caller. Historical #219 owner coordinated this declaration; no #219 bodies or completion are claimed.

The converted `shared_ptr<IWorldObjStat>` argument cleans up through the earlier native destructor at 0x800E0070. The distinct local derived owner cleans up at 0x80146D90. Both identities and all cleanup sites are checked; identical instruction streams are not treated as interchangeable targets. Only the earlier destructor's symbol name changes outside this scope.

## Acceptance

Wii/1.1 and unchanged cflags_game compile the actual registered source. The initial NonMatching split build passed the frozen DOL check, followed by the Matching build. `unitdiff.py game/ScriptUtilities.cpp` reports all 28/28 functions and 5020/5020 code bytes at 100%. The strict actual-object audit applies every relocation operand and compares all 5020 instruction bytes, 400 owned EH bytes and every cleanup target to frozen native bytes. The 27 exception-index records occupy 324 bytes.

A freshly generated MAP and Ninja linker-input query select only `build/wpse-v1/src/game/ScriptUtilities.o`, excluding the original object. All 28 final linked function addresses, sizes and bytes match. All six retained allocated ranges, including natural text/data padding, match the frozen executable. Shared weak helpers and common RTTI emitted by the compiler coalesce to their existing earlier definitions; the source emits no replacement bytes or explicit instantiations.

Same-base additive-header controls at 9895f7df44ca20075f2346de7a374cb344f10a47 rebuilt all 760 existing actual source objects. 739 object files were whole-identical; 21 differed only in compiler numeric symbol metadata. Allocated sections, symbol attributes and raw relocations were unchanged after numeric-label normalization, with no semantic alias normalization. Whole ELF allocated bytes and the frozen DOL were identical. The complete accepted source adds one actual source object.

Reproducible outside receipts are `248-reference/native-proof.json`, `248-accepted/proof.json`, `248-controls/<base>/header-report.json`; scripts `248-native.py`, `248-audit-source.py`, `248-header-control.py`. Final official land-loop receipts additionally pin the exact rebased head and repeat the full native/source-map and header-consumer gates. These scratch receipts are supplementary; all source, split ranges and semantic evidence are retained in the repository.

## Traps

- A raw Pop/ToWString local kept its CValue alive too long and changed the show-text binding. A typed PopWString operation with a scoped temporary restores the native cleanup boundary. A const int or a separate typed object-pop helper did not repair the remaining scheduling; the existing SDK s32 local did. These negative controls remain outside in 248-reference.
- The final default-delete RTTI name has 47 bytes, not the 44-byte common-base type name elsewhere. End the owned data split at 0x803B6EAC after its one ordinary alignment byte. Ending at 0x803B6EAB creates an invalid unaligned automatic split; extending to the next unit would incorrectly absorb its independent four-byte alignment.
- Whole section sizes include discarded shared weak emissions. The complete native relocation/EH audit plus source-only MAP and frozen link establish acceptance; section-only objdiff percentages cannot substitute for those checks.
