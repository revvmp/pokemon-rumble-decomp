# F-228-1: main callback contracts

Issue #228 originally lists 24 functions and 2,304 instruction bytes from `8012F900` through `80130264`. It continues the first-owned `main.cpp` unit in #225. The neighboring owners retain their original inventories; no main source unit is registered or accepted by this prerequisite.

Native RTTI and vtables establish four retained callable implementations. The `CPath` error callback is `void(CPath::Drive, const char*, long, unsigned long)` with a plain function-pointer target and both implementation flags false. The Home Button callback is `bool(HBMSelectBtnNum)`, also a plain pointer with both flags false. Existing member binding supplies the zero-argument `CMain` callback, and the plain zero-argument callback retains its existing implementation.

The bounded functional extension adds the complete four-argument pointer wrapper and a plain-pointer constructor overload and implementation for the existing one-argument wrapper. Existing generic one-argument functors retain their accepted false/true implementation. The pointer overload deduces its own return type so null-pointer constants continue to select the existing empty-wrapper constructor rather than create an ambiguous pair of non-template pointer overloads. A target result is converted with `R(f_(arguments))`; the native boolean implementation normalizes the returned value, whereas direct return emits a different twenty-byte function.

Both wrappers have the existing twelve-byte ABI: callable-base pointer at zero and an eight-byte, word-aligned inline buffer at four. Four-argument pointer targets occupy eight bytes including their virtual pointer. Copying, destruction, empty-call handling, swapping and boolean conversion follow the existing wrapper contracts. No CPath class, enum, runtime object, production source selector or unrelated callable implementation is published here.

## Native producer evidence

Outside `228-reference/main.cpp` instantiates the actual native target types using the first owner's complete outside CMain and CPath declarations. This is an instantiation driver, not reconstructed startup source. `228-audit-callables.py` independently applies every code relocation to the frozen native address and compares all twelve original callable bodies: 760 instruction bytes and 21 relocation sites. It also compares ten complete exception records totaling eighty bytes. All pass. The functions are the three retained operations in each group: CPath at `8012F900/8012F950/8012F990`, bound CMain at `8012F9B0/8012FA20/8012FA80`, plain zero-argument at `8012FAB0/8012FB00/8012FB40`, and Home Button at `8012FB50/8012FBA0/8012FBE0`.

The #227 owner independently pairs the actual constructor, base/destructor and exception-callback contracts: eight retained functions, 588 instruction bytes and eleven resolved relocation sites, with no unresolved site. Its receipt is `227-reference/227-callback-resolved-proof.json`. Recovered main startup callers must still be checked in the final combined translation unit; these isolated proofs do not establish retained section order or full main acceptance.

At baseline `ae91fbb8785d8f350473a0c02f4224690f588033`, actual Ninja `all_source` contains 756 source objects. Dependency records select 34 consumers of functional. An initial outside overlay preserves every allocated byte, section attribute, symbol attribute and semantic relocation in all 34. The official landing verification repeats the same-base full source inventory, allocated sections, relocations, symbol attributes, linked ELF and frozen DOL controls after every rebase, then recompiles the native callable producer against the actual tracked header. Its exact-head receipts are outside in `228-reference/contract-control-*`.

## Remaining original scope

The font and guard deleter half remains part of #228. Current canonical ownership probes reproduce the small guard/resource-font/paired-font deleter extents. The FontMap disposal is 1,852 versus 956 bytes in isolation: fourteen pair allocator destructions inline in the candidate where the native calls the retained 136-byte helper. Moving that allocator member to an ordinary out-of-class definition, with or without inline, does not repair this context. The actual combined main producer and remaining twelve original functions still require proof. No original issue completion is claimed.

## Traps

- A callback wrapper's indirect boolean result can require an explicit generic result conversion. Native RTTI and the full sixty-byte invocation distinguish it from the twenty-byte direct-return implementation; do not infer the implementation flags from another arity.
- An instantiation driver proves the retained template body and ABI, but does not reconstruct its startup caller or prove whole-unit physical placement. Keep those acceptance gates separate.
