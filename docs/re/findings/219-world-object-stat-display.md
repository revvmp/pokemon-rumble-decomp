# F-219-1: CWorldObjStatDisplay inventory and factory

## Scope and status

Issue #219 starts at `0x80121340`. Its original survey contains 24 functions and 3,784 function bytes through `0x80122294`. The same class and template cluster continues into #220 through `0x801248C0`; the inferred whole-file inventory is 48 functions and 13,396 function bytes. No source split or Matching selection has been accepted from this investigation.

RTTI names the concrete class `CWorldObjStatDisplay`, with `IWorldObjStatDisplay`, `ICalcDraw`, and `ICalc` bases at offset zero. The concrete allocation is 36 bytes. A `weak_ptr<BWorld>` occupies offsets 4–11, followed by two 12-byte `DrawLayerSet` instances at offsets 12 and 24. These collections are sets of shared ownership objects, rather than lists. The interface appends `Add(BWorldObj*)` after the inherited five virtual methods; its return type is inferred from unused results.

The concrete vtable at `0x803AFB80` points to its destructor, Calc, inherited CalcMtx and CalcView, Draw, and Add, in that order. The concrete RTTI record is at `0x804B7700`; the interface RTTI record is at `0x804B7708`.

## Independent factory control

An outside source probe constructs `shared_ptr<CWorldObjStatDisplay>`, initializes the world weak reference from `BWorld::shared_from_this()`, and returns the converting `shared_ptr<IWorldObjStatDisplay>`. Under unchanged game flags, its factory is exactly 432 bytes at `0x80121340`. Every nonrelocation instruction word and all twelve actual relocation destinations match the frozen executable. These include both class and control-block vtables and `BWorld::shared_from_this()` at `0x800C2F90`.

The factory's 136-byte exception table at `0x8002B4C4` also matches, including all five destructor destinations. The corresponding native exception index at `0x8004C71C` records the 432-byte factory extent. Independently comparing the three destructor implementations and their actual branch destinations establishes the identities:

| Address | Identity | Bytes |
| --- | --- | --- |
| `0x801234B0` | `move_ptr<CWorldObjStatDisplay>` destructor | 116 |
| `0x80123420` | `shared_ptr<CWorldObjStatDisplay>` destructor | 132 |
| `0x800A70A0` | `shared_ptr<BWorld>` destructor | 132 |

Raw controls are outside the repository at `C:/MassStorage/GitHub/219-reference/factory-native-paired.json` and `factory-eh-paired.json`, produced by `C:/MassStorage/GitHub/219-factory-audit.py`. This is a bounded code and exception control, not whole-unit source acceptance; retained data, complete function placement, source-only coverage, and frozen full-link verification remain required.

## Draw and iterator evidence

Draw at `0x801214F0` is 436 bytes. It reserves a temporary vector for both sets, appends each set's raw object pointers, sorts by virtual GetType, then calls Draw on each object. The native comparator at `0x80121EA0` uses signed comparison. This consumer evidence permits explicit signed casts in its comparison, but does not independently establish the interface's original return type; the canonical declaration remains unchanged.

The native vector begin and end helpers return pointer values in 8 and 20 bytes. Iterator construction copies a four-byte object through a reference, dereference loads its stored pointer, increment advances it by four, and inequality compares two stored pointers. The sort call receives addresses of four-byte iterator objects. Initial additive outside raw-pointer and wrapped-iterator proposals produced Draw bodies of 2,684 and 2,700 bytes. Neither proposal was adopted. Native disassembly is preserved in `219-reference/native-draw.txt` and `native-tiny-helpers.txt` outside the repository.

The subsequent outside probe leaves both iterator copy constructors implicit. This preserves their trivial return ABI while still emitting their native 12-byte synthesized copy functions. The complete actual Draw body then becomes 436 bytes, matching every instruction and all 32 actual relocation destinations together with its signed comparator. Its factory and factory exception controls remain exact. The decisive evidence is the actual consumer call chain, rather than a small isolated iterator fixture. Raw receipt: `C:/MassStorage/GitHub/219-trivial-iterator/draw-native-paired.json`, produced by `219-draw-audit.py`.

The pinned 2006 tree reference's `increment(pointer&)` contract also explains the native increment helper's repeated iterator stores during parent traversal. A separate outside reference-taking increment helper reproduces all 92 native bytes at `0x80121DE0`, whereas the existing by-value `tree_next` wrapper produces 80. This additive diagnostic preserves existing `tree_next` callers. Reference: `C:/MassStorage/GitHub/281-reference-tree`, increment definition and generic iterator; probe: `219-reference-increment-probe.py`. No shared header has yet changed, and complete consumer controls remain mandatory.

Contemporaneous [WG21 paper N2103, page 7](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2103.pdf) records configurable Metrowerks raw-pointer and debug-wrapper iterator forms. This supports investigating the library contract, but does not establish this target's configuration or the original wrapper spelling.

## Whole-file reconstruction controls

The complete outside class now defines Calc, Add, and the destructor, in addition to the factory and Draw. Calc's ordinary world-object and statistic loops reproduce all 720 nonrelocation bytes; the class destructor reproduces all 2,596. The generic tree copying operations follow the pinned reference's allocator guard, tagged parent/color access, recursive copying, and complete storage-pair swapping. Actual copy and copy_children reproduce 1,724 and 432 nonrelocation bytes. Three compressed storage pairs preserve the 12-byte tree layout while retaining allocator, node allocator, and comparison state. Manually swapping only the three scalar fields was semantically incomplete and left an outlined swap call; swapping complete pairs reproduces the native inline operation.

The outside code graph maps every function in the 48-function inventory. At the 2026-10-21 checkpoint, 43 instruction streams match, with actual code-branch targets propagated through the independently anchored callers. Sixteen corresponding exception records reproduce 2,680 table bytes excluding their target words. The 78 destructor target words remain separately paired; three external cleanup implementations also reproduce native instruction streams: tree owner destructor at `0x80058FD0` (1,308 bytes), node allocation guard destructor at `0x8005A770` (96), and character-array shared holder destructor at `0x8005A9F0` (96). Exception cleanup establishes that copying belongs in the constructor layer directly above the state-owning tree base, rather than in the outer wrapper.

These are outside diagnostic controls, not a source-selected link. Receipts are `C:/MassStorage/GitHub/219-tree-storage/native-code-graph.json` and `native-eh-graph.json`, generated by `219-audit-graph.py`. The exact-code graph also pairs all 12 nonbranch HI/LO/SDA relocation references and recursively checks 38 retained data objects totaling 777 bytes, including their actual pointer destinations. Receipt: C:/MassStorage/GitHub/219-tree-storage/native-data-graph.json, generated by 219-data-graph.py. This does not establish complete retained-section inventory or placement. Five code gaps remain:

| Native function | Candidate / native bytes | Remaining evidence |
| --- | --- | --- |
| Pointer-vector push_back | 1,140 / 1,148 | Retained pointer endpoint arithmetic, temporary ownership register order, and exception-context slots |
| Set clear | 1,296 / 1,296 | Fourteen addresses of exception-context stack objects |
| Add | 508 / 516 | Iterator/pair conversion and end-value lifetime context |
| Two sort instantiations | 1,004 / 1,008 each | Initial endpoint caching and iterator-operation context |

The current-main rebase retains accepted #46/#47 find_leaf, insert_node_at, root/node allocator storage, balance declarations, and the #1116 pointer-vector iterator prerequisite. It also exposes a declaration conflict: current utility explicitly defines pair's copy constructor; the pinned reference leaves it implicit. Preserving that explicit copy gives this Calc 732 bytes, while changing only that copy contract outside restores 720. Do not remove the accepted declaration without controlling existing consumers. Current baseline verification passes the frozen DOL and all 739 declared source objects; dependency-based outside overlay controls are recorded under `219-current-controls/`.

Bounded negative controls remain outside and were not adopted. A member iterator distance taking its other iterator by value makes both sorts 996 bytes; the separately coordinated free by-value proposal makes them 988. An accessor-based vector relocation range leaves push_back unchanged. Nested Add scopes, primary-like iterator conversion templates, and primary insertion expression structure do not fix Add. Primary pair get_pair helper temporaries explain the native two zero-byte stores but give Add 520 bytes; this remains a diagnostic rather than an accepted utility change.

## Shared-header prerequisite control

The bounded prerequisite changes only tree, set and utility declarations and definitions. The [pinned 2006 tree reading aid](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/tree) supplies the allocator/comparator storage hierarchy, conditional iterator constness, reference-taking increment and allocator-guard copy operations. Its adjacent utility leaves pair's same-type copy constructor implicit. Current reconstructed headers retain descriptive internal names; their spellings are not claimed recovered.

The proposal preserves existing find_leaf, insert_node_at, balancing, length-error and mutable erase APIs. It adds a const-iterator erase overload for the standard set iterator contract, without changing the pointer-specialized set path. Both iterator specializations select the correct reference and pointer constness. No vector changes are included.

At main `d2e01bae`, the unchanged baseline verifies the frozen DOL and builds all 744 source objects. Actual Ninja dependency records select 34 consumers of these three headers; the other 710 have no dependency on them. Rebuilding all 34 with a sparse outside overlay succeeds with no changes to any existing function name, size or instruction bytes. Thirty-two allocated objects also retain all normalized section bytes and relocation identities. BModelObj and BWorld retain every function byte, actual relocation identity and per-function exception record, but reorder weak helper functions and their corresponding exception records. These ordering differences are retained explicitly rather than described as raw object identity. Raw controls: `C:/MassStorage/GitHub/219-d2e-controls/typed-tree-report.json` and `typed-tree-weak-order-control.json`.

This prerequisite does not select the display source or complete either original block issue. Complete display code, retained layout and source-only acceptance remain separate gates.

The final actual-header producer consumes current canonical vector directly, with only the display's missing pure Add slot supplied in an outside interface overlay. It names the factory through the now-canonical IWorldObjStatDisplay::Create declaration. This exposes an important stale-probe difference: current pointer-vector push_back already reproduces all 1,148 native bytes, its branch targets and its exception record. The final graph therefore has 44 exact functions, 17 exact exception records and 86 independently paired destructor targets. All 30 nonbranch code references and 38 recursively reached data objects remain paired. Four gaps remain: clear's exception-context addresses, Add and both sorts. Receipts: `C:/MassStorage/GitHub/219-final-header-producer/native-code-graph.json`, `native-eh-graph.json` and `native-data-graph.json`.

With the proposed headers applied, configure, Ninja, all_source and frozen-manifest verification all exit zero. The proposed header build preserves the frozen main.dol. This is a header prerequisite acceptance gate; the display unit is not selected by it.

## Resolved shared-header context

The final prerequisite supersedes the earlier storage replacement. It preserves the concurrently accepted tree storage and mutable iterator, including keyed find/find_or_insert, reference-returning count, red_black_tree forwarding, tree_leftmost, increment and swap APIs. A separate const iterator supplies the set contract while retaining trivial copying. Pair has its converting constructor and implicit same-type copy. The [pinned 2006 MSL utility](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/msl_utility) supplies compressed_pair's memberwise swap interface with empty-base elimination. Using that interface reproduces the 1,724-byte native copy without changing the incoming storage types. Replacing those types had regressed a newly accepted 100-byte map constructor and was rejected despite a passing frozen DOL.

At main `5a5b2211`, the unchanged baseline verifies the frozen DOL and builds all 751 source objects. Fifty-four depend on the four changed headers; 697 do not. All affected objects compile. Fifty-two retain every normalized allocated section and relocation identity. CLayout's remaining difference is compiler-generated __FUNCTION__ label numbering: all 77 functions, 56 exception records and actual referenced string payloads remain identical.

Two already nonmatching MapPiiTable functions change. InsertString grows from 976 to 1,000 bytes against native 1,004; InsertWString grows from 980 to 1,004 against native 1,008. These are disclosed gaps, not exact matches. The public signatures and class layout remain unchanged. The new iterator borrows the same four-byte node pointer; its conversion copies that pointer and the unused pair bool without ownership operations. Its const string reference still calls the same const c_str observer. Both functions preserve their ordered call targets and their local string destructor cleanup target. The other 53 functions and 39 exception records remain exact against the current baseline. No previously exact function is lost. Raw receipts: `C:/MassStorage/GitHub/219-approved-controls/proposal-report.json` and `delta-audit.json`, generated by `219-approved-consumer-controls.py` and `219-approved-delta-audit.py`.

The actual resolved-header producer retains 44 of the whole file's 48 exact instruction streams and actual code targets, 17 exception records totaling 2,828 bytes with all 86 cleanup destinations, and 30 data references reaching 38 retained objects totaling 777 bytes. Four code gaps remain: clear's exception-context addresses, Add and both sorts. Retained function/section placement and source-only acceptance remain outstanding. No original issue closure is claimed. Native receipts: `C:/MassStorage/GitHub/219-approved-native/native-code-graph.json`, `native-eh-graph.json` and `native-data-graph.json`.

Independent source-context controls are rejected: ordinary inline display member definitions worsen retained order; moving the raw shared_ptr constructor or allocator::destroy out of its class leaves the remaining gaps unchanged; the primary get_pair path gives Add 520/native 516. These controls do not justify flags or forced layout.

## Traps

Equal function sizes alone misidentified two neighboring ownership destructors. Pair exception-table destinations with independently checked destructor code and actual branch targets before naming ownership helpers. Also, a native signed comparator proves this comparison's interpretation, rather than the signedness of an externally owned virtual declaration. A hand-written copy constructor can change a four-byte iterator's return convention even when its body looks identical to an implicit copy; use the native caller and returned-value ABI to distinguish them.

An outside overlay can preserve stale library bodies after main advances. The frozen pointer-vector copy kept an obsolete 1,140-byte push_back gap after current canonical headers already produced the exact 1,148-byte implementation. Recompile the actual producer with a sparse overlay containing only proposed changes before attributing a remaining gap to current shared code.
