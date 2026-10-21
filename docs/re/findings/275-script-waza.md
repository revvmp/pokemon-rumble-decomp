# F-275-1: fade and Waza bindings

Original issue #275 contains 18 functions and 3,456 instruction bytes from `0x8015E0E0` through `0x8015EEB0`. The survey's `bind_7` label is a grouping estimate rather than a recovered filename. The preceding #274 owns its separate bindings; no #274 source, split or private state is changed here. The following SaveData unit starts at `0x8015EEB0`, as established in [F-276-1](276-savedata.md#f-276-1-unit-boundaries).

An actual typed producer reconstructs 17 functions and 3,328 instruction bytes with every instruction relocation resolved to its actual native destination. Its 15 owned exception records reproduce all 512 bytes and every cleanup target, corresponding to 180 bytes of exception-index entries. This is an object-level result; no complete source-selected acceptance or original-issue completion is claimed.

The producer uses existing typed engine, session, world, map-table and Waza interfaces. Two additive CWaza declarations are directly constrained by the callers: its type flag is `0x10000`, and `GetWazaID()` reads the existing signed member at offset `0x64`. The original binding name `method_CWaza_getWazaID` supplies that accessor name. No storage, existing operation or native Setup declaration changes.

Same-base controls compile all 765 existing source objects. Of these, 764 objects are byte-identical; CWaza's parsed sections, symbol attributes and raw relocations are unchanged after normalizing numeric compiler-generated identity suffixes. The linked ELF preserves those same semantic contents, and the complete fresh DOL is byte-identical and passes the frozen verifier. The additional enum and unused inline accessor do not create reconstructed coverage by themselves.

## Remaining forwarding function

The missing function at `0x8015E530` is 128 bytes with a 56-byte exception record. It receives a Waza pointer, an integer move ID and a reference to a shared parent. It copies that parent, calls the established Setup body at `0x80121040`, and releases the copy. Both branches in NewWaza call this function. A declaration-only descriptive `SetupScriptWaza` binding names that actual native dependency in the diagnostic producer; it supplies no replacement implementation or coverage for the missing function.

Immediately before the Setup call the original forwarding function loads zero into `r6`. The existing three-argument member-call interface does not explain that value. The Setup callee never reads the register, and there is only one direct native caller, so the value alone cannot establish an original optional parameter's type. A bounded outside diagnostic with an inferred extra integral parameter fails to reproduce the complete forwarding/caller context and is rejected. Moving the same-type shared-pointer copy constructor to its genuine original MSL out-of-class definition also leaves the diagnostic gap unchanged. Neither change is proposed for shared headers.

The complete typed producer, exact instruction/cleanup receipts and rejected diagnostics remain outside the repository under `275-reference`. The native and exception auditors are `275-strict.py` and `275-eh.py`. The header consumer comparison is in `275-reference/275-cwaza-gate/report.json`. These records distinguish the 17 reconstructed bodies from the unresolved native dependency.

## Traps

`SetWazaDesc` retains the derived CPpuEngine view: replacing it with only a base-engine view changes register allocation despite equal nominal field offsets. The actual typed derived view reproduces all 272 bytes.

`SetWazaDescRow` pops the final argument into the existing argument temporary before pushing null. Replacing this with a stack-count decrement removes 24 native bytes of observable temporary stores. Ordinary value flow reproduces the complete 296-byte routine without forcing stores.

An otherwise exact caller can still name the wrong callee. NewWaza's direct call to the existing Setup kernel produces a native-size body but bypasses the original forwarding function. The strict audit requires the intermediate actual destination; it does not alias the two functions.
