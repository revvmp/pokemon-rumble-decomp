# CFileData final function and linking work

## Function match

On 2026-11-04, a fresh compilation reproduced the sprint baseline: 147 of 148 CFileData functions matched. `UpdateResourceFiles` differed only in twelve uses of the cache iterator register, r28 instead of r26. The previous honest attempts and allocator diagnosis are in [sprint-a02](sprint-a02.md#unsolved-after-the-fake-match-wave).

The retained D-0016 fake temporarily stores the worker pointer in the cache iterator's node pointer, casts it back to call `ProcessFinishedJobs`, and then passes that iterator by reference to an inline cache-purge overload. The purge overwrites the iterator before reading it as a cache node. The worker address is never dereferenced as a cache node.

Reusing the pointer first fixed the register allocation but moved five four-byte stack slots. Passing the iterator by reference through one inline level restored their original positions. By-value forwarding and two through four additional inline levels did not match. The original zero-argument purge remains for `ShutdownResourceFiles`.

`python tools/re/unitdiff.py game/CFileData.cpp` reports 148/148 functions and 63,272/63,272 function bytes at 100%. This is a function comparison, not a successful source link.

## Full build and remaining link work

An experimental `Matching` selection linked but failed the frozen SHA check. Exact verification reported the first differing DOL byte at `0xF032`. Linked symbol inspection found extensive emission-order differences, including the explicit map `erase` instantiation and its callees preceding the CFileData constructor. RTTI, vtable, and exception-table order also remain unresolved. The selection was not retained: CFileData remains `NonMatching`.

The main-checkout build with the retained function change and CLayout's separately recorded gain completed with ninja exit 0, followed by `verify.py` PASS against `config/targets/wpse-v1.json`. Original fallback selection remains in force for both units.

## Traps

Reusing a pointer through a local iterator can fix register allocation while changing stack-slot creation order. Inspect the entire function diff after a register improvement; the reference-forwarding inline level was necessary here. This is a targeted fake, not evidence that the original developers used these casts.

## Emission-order investigation

Scratch-only follow-up preserved all 148 function matches while improving the longest common subsequence of named function order from 110 to 125. No emission experiment below is retained in the source or shared headers yet.

Moving the forced map erase instantiation to the end, making the BThread destructor, Entry, and PushFinished inline, and repositioning the two worker helpers established the initial 116-order baseline. Replacing the forced erase instantiation with a local inline specialization preserved its code and put it in its caller's emission wave, but also requested allocator destruction too early.

Declaring CPath's copy constructor in a scratch header and defining it inline before OpenFileData moved its constructor and list-copy constructor into their required positions. The ordinary member initializer left the copy constructor at 99.43%; accessing the list through a character pointer advanced by sizeof(mDevice) reproduced all instructions. Local inline specializations of string-list pop_back/back and file-cache map subscript then placed their functions correctly. These are candidate fake matches only.

Adding an ordinary explicit vector assignment operator to a scratch vector header and locally specializing CNTHandleTable assignment placed that operator correctly but pulled several vector/tree helpers too early. The next task is to preserve delayed helper instantiation while keeping the early assignment operator. Snapshot source and comparisons are under ignored tools/local/session/310-ppu; mapspec1 and vecspec retain all 148 functions, with different ordering tradeoffs. The scratch header overlay includes CPath.h and vector and must accompany those candidates.

Native linker ordering was also investigated. The [NXP Power Architecture Build Tools Reference](https://www.nxp.com/docs/en/reference-manual/PA_Build_RM.pdf), pages 202?211, documents input file/section selection but no symbol-order selector within an input section. The Wii/1.1 command-line help and linker strings did not expose such a facility. No linker configuration was changed.

## Function-order candidate

The subsequent scratch candidate reproduces all 148 function instruction streams and their complete target order. One inferred function name changes: the old non-template `map_compact_tree::find_leaf` is implemented as a deferred `map_leaf_search<Tree, Compare>::find` helper. Its compiled 96 bytes are exactly equal to the target helper's 96 bytes. The only corresponding instruction comparison difference is the relocation name on the map insert call at offset `0x38`. This is a disclosed D-0016 helper, not recovery of an original name; no embedded original name is replaced.

The candidate scopes all shared-header changes behind `CFILEDATA_EMISSION_LAYOUT`, defined only by CFileData.cpp. The scope declares the CPath copy constructor, supplies explicit vector assignment, defers compact-tree copy/destruction and alloc_ptr destruction definitions, and makes the generic map destructor implicit. Other translation units retain their existing declarations and definitions. The leaf-search helper and CNT tree insert/erase specializations are source-local.

Additional source-only ordering controls include early shared-pointer/allocator type probes, local stream-accessor/string-append/list specializations, a deferred char-buffer reset specialization, a two-level table-assignment forwarding helper, a dead constructor probe requesting table range helpers, and explicit instantiation of the first four deferred tail functions. The direct erase specialization destroys the weak-pointer member behind the same null checks as the pair destructor, avoiding an early allocator-destroy request. These candidate fake constructs preserve all target instruction streams; the full source link and data order remain a pending acceptance gate.

The reproducible candidate is ignored `tools/local/session/310-ppu/ready.cpp`, its scoped headers are in that directory's `include/`, and the reviewed diff is `header-changes.diff`. `310-checkleaf.py ready` verifies the complete mapped function order and exact helper bytes. No assembly, original-byte substitution, original hashes, or binary rewriting was used.

The coordinator applied this candidate and the one inferred helper-symbol rename, then ran the complete build. The source-link trial places all 148 functions at exact target addresses and reproduces the extab, extabindex and ctors sections byte for byte. Remaining differences are data and data-pointer relocations. The selector was restored to NonMatching; the subsequent full build with the original unit fallback passes the frozen SHA, validating the scoped headers against all consumers. Source-data acceptance is still pending.

## First source-link trial and callback data order

The first complete source-link trial succeeded but failed the frozen checksum. All 148 function addresses and the complete extab, extabindex, and constructor sections were exact. The executable size was unchanged; differences were limited to 29 text bytes carrying data references, 1,128 data bytes, and 78 small-data bytes.

The function-base callback vtable and RTTI block was emitted after the shared-pointer deleters. The target places it immediately after the function implementation, before the LoadedFileData deleter. A scoped declaration of the four-argument function call operator, paired with a source-local specialized implementation, defers the original function-base type's first complete use. The implementation calls the existing callback through a local abstract dispatch layout containing the same destructor, clone, clone-into-buffer, and invocation slots. This reinterpret view is intentional D-0016 ABI source shaping, not recovery of an original type. No dispatch-view functions, RTTI, or vtables are emitted. Its callback operator instructions are exact, and the nine owned vtables then have their exact target data offsets. A second full source-link trial is the next gate.
