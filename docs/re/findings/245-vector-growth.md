# Non-POD vector growth (#245)

## F-245-6: dispatch through the existing append operation

The non-POD `push_back_realloc` overload must use the result of `grow_by(1)`. When growth succeeds it constructs the element at the current end and increments the size; otherwise it calls the existing `append_realloc(1, x)` overload. The previous implementation ignored the growth result and expanded the replacement-buffer operation directly. The recovered branch and call structure is independently visible in the complete native `shared_ptr<BUILayout>` producer at `0x800CA5D0`, including its call to the append operation at `0x800CC130`.

This change affects one existing method. It preserves the current POD implementation, traits, public interfaces, unsigned-long specialization, allocator, numeric limits and split-buffer ownership operations. The existing append helper retains the native moved-from size reset before transferring the replacement allocation. Names of internal vector helpers remain inferred where no original mangling is available.

## Native producer controls

The current source independently reproduces five complete native producers, all instruction words and all 261 resolved targets: four `CGameSession` push-back instances at `0x800B3A60`, `0x800B4310`, `0x800B4B90` and `0x800B5120`, and the `CMsgBalloon` reallocation wrapper at `0x800CA5D0`. Together these contain 5652 instruction bytes. Their complete five exception records, 600 bytes and their cleanup targets, also match. Cleanup identities absent from the symbol inventory are paired through their native exception references and checked against their complete source destructor bodies and targets.

The four push-back instances retain their native lengths of 1372, 1540, 1300 and 1300 bytes and now reproduce the native tag-temporary slots and ordering. The reallocation wrapper changes from 260 bytes to the native 140-byte implementation. Its existing append operation is now outlined through the ordinary compiler workflow. Newly outlined forwarding helpers are not claimed to have the complete native retained profile: one const compressed-pair forwarding wrapper remains separate where the native caller directly reaches its storage accessor. These consumers remain NonMatching.

## Current consumer and executable controls

The initial current-main control at `5a5b22116b2b480095b034fdc42a8827211bfad9` explicitly builds every declared `all_source` target. Of 751 actual source objects, 748 are raw identical. All 702 Matching objects are raw identical, including all allocated bytes, symbol attributes and semantic relocations. The three changed objects are the existing NonMatching `CGameSession`, `CMsgBalloon` and `ScriptItem` producers. The complete ELF and DOL are raw identical before and after the header change; the immutable manifest verification passes.

The official landing loop repeats actual current-main source controls after a rebase. It builds the baseline with the exact current-main header, restores the candidate header, rebuilds every actual source target, checks every Matching object, checks the five native producers and exception records, and compares the complete ELF and DOL. It does not reuse an old consumer inventory across source changes. Per-round receipts record the actual baseline and candidate heads.

`ScriptItem` is not completed by this prerequisite. The current public vector push-back definition can remain outlined in `SetPasswordUsed`; the earlier isolated full-body result belongs to a different accepted header context. The unit also needs a faithful solution to retained weak-function ordering and a source-selected complete link. Its object remains NonMatching.

The outside reproducible controls are `C:/MassStorage/GitHub/245-consumer-control.py`, `245-native-pushback.py` and `245-official-land.py`. Immutable baseline objects, complete artifacts, native proofs and per-round receipts are retained under `C:/MassStorage/GitHub/245-reference`.

## Traps

- A matching isolated producer can stop inlining after a shared public template definition changes. Rebuild the actual current consumer and distinguish the older producer evidence from the current source-selected result.
- PowerShell `Copy-Item` can preserve an older header modification time. Writing candidate bytes with a fresh modification time and explicitly checking `ninja all_source` prevents a no-work response from being mistaken for a rebuilt header control.
