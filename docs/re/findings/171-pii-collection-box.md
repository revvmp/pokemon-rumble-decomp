# F-171-1: CollectionBox ownership and initial source reconstruction

Target: frozen `wpse-v1` executable. Original issue #171 contains 22 functions and 7,616 code bytes at `0x800E1AD0`–`0x800E3904`. No source acceptance or issue completion is claimed here.

## Native ownership

`CPiiCollectionBox` is 24 bytes, containing two vectors of 28-byte entries. The factory at `0x800E23A0` allocates 24 bytes, initializes both vector states, resizes the first to 496 records, constructs the shared owner, and imports saved records starting at species index one. The entry contains collection type at zero, three signed per-mode BRR maxima at offsets 4/8/12, defeated count at 16, and two form masks at 20/24. The original script binding name `method_pprSetCollectionBRR` supports retaining the BRR spelling without expanding it.

The native exception table contains two references to `0x800E1AD0`, both owned by the factory. The cleanup destroys the factory's entry vector; it is not the preceding shared-owner vector's destructor. Its trivial element cleanup agrees with the 28-byte records. The original survey's proposed new class at `0x800E5010` is also premature: the factory's exception metadata names that 116-byte vector backend destructor. The shared deleter and its type strings belong to CollectionBox, ending before the next unit's `wos_piiinfo` string at `0x803A8B70`. The current evidenced text end is `0x800E5084`, crossing into unassigned issue #172.

`0x800E4040` copies the first entry vector to the second. `0x800E4730` restores it in the opposite direction. Their initial ordinary assignment producers are 1,768/1,756 bytes against native 1,772/1,760; these are diagnostic results, not matches.

## Initial actual producer

The outside producer and native audit are in `C:/MassStorage/GitHub/171-reference/`. Ten class/comparator bodies reproduce 852 instruction bytes: the recording method, seven accessors, defeat comparator, and `GetManyDefeatedPii`. The latter skips reserved species slot zero, sorts pointers by descending defeated count with a pointer-order tie, and returns their indices. A direct source call graph additionally pairs the entry-vector cleanup and three-element sort helper. The immutable actual producer at `C:/MassStorage/GitHub/171-reference/171-20-functions-baseline/` proves 20/22 original functions and 3,944 code bytes. `171-strict-code.py` applies all 76 code relocations and compares complete native instructions; `171-eh-audit.py` proves all eight associated exception records, 148 bytes including cleanup destinations. Both exit zero. Whole-source linking, physical placement, remaining metadata, and the two residual bodies remain unfinished gates.

The recording method clamps defeated count to 9,999,999 through reference-taking `std::min`. A named static constant reproduces the native immediate comparison and constant-address selection. A temporary literal instead compiles to 184 rather than 188 bytes. This result applies to this function, not to every game clamp.

The two large actual bodies remain diagnostic: export is 1,724 bytes with 19 differing instruction words, and the shared-owner factory is 1,932 against native 1,948. The factory lacks two genuine count-loop entry guards, four instructions in total. The independent native reading aid agrees with the current temporary-entry and conditional placement-copy structure; no extra guard was fabricated. Wii/1.3 and the full original 2006 header oracle retain both residual body differences.

## Primary declaration evidence

The pinned original 2006 vector header dispatches single-element push and repeated-value construction using trivial copy-constructor and assignment traits, independently of the scalar/POD storage classification. The current reconstructed header uses POD for both. The export's 16-byte saved record requires non-scalar reserve copying but the native assignment fast path for push. An outside trait-dispatch correction restores its 1,724-byte extent. This is not yet a canonical shared-header proposal: the remaining words and all current consumers must be controlled first.

The saved record's first word has four three-bit fields, a one-bit Boolean, and nineteen reserved bits; the remaining words store defeated count and both form masks. The Boolean's byte store is independently visible in export. The outside declaration preserves the existing four-word view; it has not been published.

## Split validation

The complete evidenced CollectionBox split, including the continuation through `0x800E5084`, passes `configure.py --map`, incremental `ninja`, and frozen-manifest verification. The verified fallback DOL retains SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This establishes the split boundary only, not source acceptance. The `0x804BA058` constant is four bytes; its previous eight-byte survey extent included following padding. Existing unrelated split block ordering was restored after dtk regeneration, with block contents independently checked unchanged.

## Traps

- A survey boundary can bisect a factory's cleanup graph. Establish the native exception-table owner before assigning a preceding destructor to a neighboring class.
- The primary default-count loop's size increment in its `for` increment does not repair this factory's missing 16 bytes. Copy/range construction controls from #1214 are a separate contract.
- Copying an entire entry before export increases the body to 1,748 bytes; it is not the native dataflow. Direct access and existing typed getter calls both retain the 19-word export gap.
- Individual iterator arithmetic or algorithm controls are insufficient here. The coherent primary global `__wrap_iterator<Container, T**>` declaration and primary sort/selection/three-sort bodies together reproduce both 1,008-byte sort specializations, all targets, and their exception records. Either component alone remains different. The outside proposal preserves the accepted unsigned-short construction layer and current generic vector copy/destructor declarations; no shared header was published.
- Aggregating saved-record initialization instead of assigning the observed fields reduces export to 1,700 bytes and is not native. Removing the compatibility union, changing signed integer spelling, or making the class accessors const does not repair the 19-word export gap. These controls remain outside the repository.

## Blocked checkpoint

The original issue remains incomplete. No canonical source, class-layout correction, vector/iterator algorithm proposal, or `Matching` object is published by this checkpoint. The accepted change establishes only the fallback split and records the immutable actual producer, strict auditors, and rejected controls above. Moving the unchanged CollectionBox constructor from its guessed inline header definition to an ordinary out-of-class definition also retains export's 19 words and the 1,932-byte factory; the separate receipt is `171-reference/171-ordinary-constructor/`. Further work requires genuine source or compiler-context evidence for these two bodies and subsequent whole-unit metadata/placement/source-selected frozen validation.
