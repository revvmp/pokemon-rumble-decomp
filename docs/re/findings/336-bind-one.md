# F-336-6: One-argument member callback prerequisite

Part of #336. This additive prerequisite does not select CUIList source or credit new matching functions.

## Native contract

Frozen RTTI at 804B8490 identifies function_imp<void (*)(BUI *), bound_func<void, mem_fn_1<void,CUIList,BUI *>, tuple<CUIList *,ph<1>,nat x8>>,false,true>. Its complete type name is at 803C12E0, vtable at 803C12C8 and base array at 803C1430. The base function_base<void (*)(BUI *)> has RTTI804B6058 and vtable8039CD88. The second boolean differs from the existing zero-argument specialization; its semantic name remains unproved.

The independent producer compiled against canonical headers matches clone108 at801DC450, buffer clone96 at801DC4C0, invocation48 at801DC520, destructor64 at801DC680, base destructor64 at8007BF00 and is_null84 at80070700: 464 code bytes and eight resolved targets. All six indexed exception tables total48 bytes and match. Both24-byte vtables, both8-byte RTTI records, the12-byte base array and complete names match after relocation resolution.

The implementation adds a two-element tuple, first placeholder, member-one adapter, bound invocation and one-argument function specialization. The borrowed argument tuple is passed by value to placeholder resolution, preserving its referenced argument lifetime and native invocation spill. Helper spelling and source organization are inferred from behavior, not recovered names. Existing zero-argument definitions remain unchanged. The placeholder is declared extern and introduces no emitted storage in these native consumers.

## Current consumer controls

Baseline was independently refreshed on fbab3bbc36 after the initial0b86a452 control. Actual all_source builds before and after the change contain718 objects:714 are whole-object byte-identical; CMsgBalloon, CFiber, pad and sound differ only in numeric local symbol names. All other sections, raw relocations, exception data and normalized symbol attributes remain identical. No fallback-only default build was substituted for this control.

Outside evidence is retained in C:/MassStorage/GitHub/336-reference: 336-binding-clean-probe.cpp/.o, 336-binding-native-proof.json, 336-binding-data-proof.json and 336-bind-candidate-build.log. The immutable before-manifest and after-result are in C:/MassStorage/GitHub/329-reference/336-binding-fbab-sections-before.json and336-binding-fbab-control-result.json. Auditors are C:/MassStorage/GitHub/336-audit-binding.py and336-audit-binding-data.py.

## Traps

A static placeholder definition emitted an unreferenced byte in every consumer. The declaration-only placeholder removes this unintended storage. Passing the borrowed argument tuple by reference removed a native argument spill; the by-value borrowed view reproduces the actual lifetime and six complete native bodies without extra operations or forced code.

Native is_null at8007CC20 is byte-identical to80070700, but the complete CUIList caller destination is not yet proven. The array Load candidate remains rejected at2824 versus2996 native bytes; the840-byte destructor remains rejected with22 differing words. No complete caller acceptance follows from the helper proof.

Official land dry-run rebased onto fbab3bbc36 and verified d420dcd6e7adfc465110a7537dfef7219df6ac8b: configure0, ninja0, frozen verify0, exact main.dol. It then stopped because the draft PR had not yet been opened; no merge was attempted.
