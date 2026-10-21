# F-337-1: CUIList translation unit and partial source integration

Part of #337 and the coordinated first-function owner #336. Both issues remain unfinished. This registers actual source as NonMatching; the original complete unit remains the link fallback. No source-selection or closure credit is claimed.

## Unit boundaries

| Section | Start | End |
| --- | --- | --- |
| extab |8003EB10 |8003F20C |
| extabindex |80051A08 |80051B10 |
| .text |801DA0C0 |801DC6C0 |
| .rodata |80367068 |803670A8 |
| .data |803C1090 |803C1890 |
| .sdata |804B8448 |804B8498 |
| .sdata2 |804BA4D0 |804BA4E0 |

The first function is the IUIListItem abstract destructor. The final retained member-binding destructor at801DC680 ends801DC6C0. The next function constructs CUIMenuItem, storing its distinct vtable803C1D70 and copying its wide-string member; it starts the next unit rather than extending CUIList. Native class names are original; the source basename CUIList.cpp is inferred. The22 contiguous exception-index records cover exactly the framed functions of this31-function/9544-byte unit. No .ctors entry belongs to it.

Data begins with CUIList's focus member descriptor and strings, followed by its binding RTTI/vtable, CUIList and IUIListItem vtables and coalesced library names. The final exception string ends before CUIMenuItem's first TITLE_DISABLED_c literal at803C1890. Small-data strings and the binding RTTI end804B8498, where TITLE_c begins. The three scalar/double constants at804BA4D0/D4/D8 are the only referenced .sdata2 entries. The two2-by4 constant tables at80367068/7088 describe input orientation and button masks; no native bytes are embedded in reconstructed source.

## Complete class layout

CUIList is180 bytes, deriving from canonical BUILayout134. Fields134/138 are input-orientation and scrolling-mode words; vectors of shared CLayout and IUIListItem begin13C and148. Selected index154, first visible index158 and relative range15C/160 precede four flags164â€“167. Weak selected-item owner168/16C and raw item identity170 precede three4-byte direct-delete callback-list owners174/178/17C. Private owner spelling is descriptive, with construction/deletion/noncopyability only and no invented transfer contract.

All three callback payloads are directly proven function<void(BUI*)>. Native registrations8008B1F0 and8008B360 insert into174 and178 through helper80078FB0; their function vtable8039EE58 points to RTTI804B6458, whose complete type names member-one CDlgPiiListImpl/BUI* binding. Registration80078B0C inserts into17C after constructor8007C8B0 installs vtable8039C988; RTTI804B6038 names the equivalent CDlgCollectionImpl/BUI* binding. This supersedes the earlier unresolved payload caveat in F-336-5.

Flag164 retains directional selection changes when enabled; otherwise native801DBFD0â€“801DBFE4 restores the old selected and visible indexes. Flag165 suppresses the selected highlight when focus is absent. Flag166 enables activation on held-input selection changes; flag167 permits wrapping. Names SelectionInputEnabled, RequireFocusForHighlight, ActivateOnHold and WrapSelection are descriptive inferences. The17C callbacks fire when the first visible index changes, so the earlier cancel-callback name is superseded by ScrollCallbacks. Other public method spellings and integer signatures remain source inferences unless separately documented.

## Implemented and missing source

Nineteen actual class functions are implemented. The real unitdiff entry point reports17 of31 native functions at100%,1056 of9544 code bytes. The destructor840 and scrolling-range calculation232 remain nonmatching register context. Twelve target functions are absent by name: array Load2996, UpdateSelection1456, OnInput1656 and OnFocus292, plus eight native template helpers1016 bytes. Ordinary compiler-emitted helper bodies remain explicit diagnostic output until their exact retained identity is established.

The selected-layout and first-visible getters need an out-of-line scrolling-range call. Meaningful local snapshots of the layout/item counts and relative window restore that calling context. Canonical Clamp(value,low,high) is required for the scroll-range setter: direct nested std::min/std::max materializes an incorrect small-data zero instead of the native value-parameter stack temporaries. No flags or synthetic guards were added.

Outside source establishes the missing Load's real early error return, successful Calc/UpdateSelection calls and bulk split-buffer copy loop, but the candidate remains3000 versus2996 bytes. A generic separate destruction-count candidate makes the destructor840/all23 targets exact, but changes three current consumer objects and remains unpublished. Its native-delta diagnostics are in F-336-5; those do not override the canonical consumer gate. Existing vector, string, list and memory headers are untouched by this integration.

## Validation and remaining acceptance

The actual configure/defaultNinja split build and explicit frozen main.dol verification exited0 before symbol naming. The source compiles through all_source and tools/re/unitdiff.py game/CUIList.cpp, not only an outside probe. Later validation below records the final renamed split. All data, RTTI, EH, placement and source-only map gates remain required before Matching. The unchanged original unit is deliberately the final link input.

Outside evidence remains in337-reference:337-cuilist-actual-unitdiff.log,337-split-build.log,337-all-source-build.log and the ordinary source variants. Adjacent336 proofs and rejected candidates remain in336-reference. Those outside artifacts do not replace the committed source or frozen acceptance pipeline.

## Traps

An issue edge is not a source-unit boundary. The first owner336 and continuation337 are both claimed by the same contributor, and the complete native unit is split together; no fake split ends at801DB250. Isolated source probes can inline the scrolling-range helper into getters differently until the meaningful count/window locals and caller context are present. Record this as compilation context rather than adding noinline directives.

Final current control uses base0fcd66c5 and actual all_source builds in both336 baseline and337 candidate trees. All732 previous source objects are whole-object byte-identical; the candidate adds only CUIList.o for733 total. No numeric-name allowance was needed. The manifest/result are337-integration-baseline-manifest.json and337-integration-control.json outside. The renamed split defaultNinja and frozen verify also exit0; actual unitdiff remains17/31 native functions100%,1056/9544 bytes. This is partial reconstruction, not complete unit acceptance.

## Final integration and focus continuation

PR1102 merged as7771c01195 after official verification of830c12c7e4f40d71c5caeb76d395172864773bae againstf5e4ca93. A fresh actual all_source baseline/candidate comparison preserved all733 previous objects whole; the candidate adds CUIList.o for734. No numeric-name allowance was needed. This supersedes the earlier732-object control only for current consumer coverage; both immutable proofs remain outside in337-reference, with final evidence337-final-integration-control.json and337-final-integration-baseline-manifest.json. The unit remains NonMatching.

An independent ordinary OnFocus implementation now reproduces all292 native instruction bytes and19 resolved targets at801DC250, plus the complete136-byte exception table at8003F10C. It checks BUI::IsFocused(), creates normal_hover then queues hover when focused, otherwise creates hover_normal then queues normal, and finally updates selection. String contents and small-data locations are native evidence; names and source structure remain inference. Evidence337-focus-code-proof.json and337-focus-eh-proof.json accompanies the ordinary outside source337-focus-probe.cpp. Canonical publication awaits the coordinated BUI IsFocused declaration and the independently owned217 GetLayout inline prerequisite. No root327 function body is taken over.

The new ordinary UpdateSelection outside candidate covers scrollbar visibility and geometry, page labels, each visible item update/highlight, and selection-owner identity callbacks. Initial1540 bytes reduced to1480 by direct weak assignment from the shared owner and unsigned scrollbar-index conversion; repeated GetLayout calls and a direct float clamp plus signed count comparison yield1456, equal to native size. This is not a matching claim: the complete resolved comparison still differs in pool-base/register and weak-owner context. The partial source pool naturally begins at BACKWARD, while the complete native TU pools earlier Load strings before it; no padding or copied native pool was added to mimic that context. Candidate337-update-selection-probe.cpp/.o and rejecting337-update-selection-code-proof.json remain outside.

Two natural scrolling-range local-order candidates remain232 bytes but differ at20 and15 instruction words respectively; neither replaces canonical source. Their rejected comparison is337-scroll-order-control.json.

## F-337-2: Native focus body integration

The bounded BUI IsFocused declaration is coordinated with the former327 owner and preserves all existing slots, fields and methods. Its native producer is88 bytes at801C8DE0, ending with blr at801C8E34; the earlier84-byte reading omitted that final instruction. Independent ordinary producer source uses GetManager, returns the manager focus comparison when present, and returns false otherwise. All88 bytes and the single direct target match, recorded in337-isfocused-producer-proof.json. The producer body remains outside and the327 split is untouched.

After consuming217 prerequisite1110/f1455ca4bf, actual src/game/CUIList.cpp now defines OnFocus. The actual full-TU object reproduces all292 native instruction bytes and19 resolved targets, with string contents checked, and its complete136-byte EH table. Evidence337-focus-actual-code-proof.json and337-focus-actual-eh-proof.json uses the actual all_source object, rather than only a standalone probe. The complete unit remains NonMatching and retains its original fallback pending the other missing functions and full data/source-only proof.

The new ordinary input-handler candidate covers page and selectable-item movement, wrapping, activation and scrolling callbacks, and neighbor focus transfer. It is1472 bytes isolated and1652 bytes in the actual class-method context versus1656 native; repeated selected-index normalization naturally inlines only with the real method body available. The candidate and corrected directional/button table source remain outside337-input-probe.cpp and337-input-unit-context.cpp. These are typed scalar tables reconstructed from input behavior, not executable-byte emission. No input body or additional neighbor/focus-manager interfaces are yet canonical.

Current focus consumer control pins baselinee1e4fde12550d7f157ee3c487e6f2f99a9f86266 and actual737-source builds. All736 other source objects remain whole-object identical; only the intentionally extended CUIList object changes. Evidence337-focus-e1e4-consumer-control.json. Earlier controls with unequal baseline heads are explicitly rejected and retained for diagnosis.

# F-337-4: Complete CUIMenu boundary and primary-header controls

The renewed helper claim for337 owns the following complete CUIMenu translation unit,801DC6C0 through801DF650, rather than taking over336's preceding CUIList source. The retained original inventory has61 functions and11812 instruction bytes. Its first11 functions occur in337, followed by all15 functions in338 and all35 in339. Original337's preceding20 CUIList functions remain part of the unfinished336 source unit; completing CUIMenu alone cannot close337. The338/339 issues were open and unassigned when their containing unit was screened. The CUIMenu.cpp basename and unencoded member/API names remain descriptive source inferences; CUIMenu and CUIMenuItem class names are native RTTI evidence.

The complete outside producer and sparse typed headers are retained at C:/MassStorage/GitHub/337-menu-reference. CUIMenuItem is24 bytes: IUIListItem base, width4, wstring8 and integer kind20. Its vtable is803C1D70. CUIMenu is1C0 bytes with CUIList base180, readiness flag180, four-float bounds184, cell height194, item center198, title position1A0, minimum width1A8, column/row counts1AC/1B0, spacing1B4/1B8 and retained cursor row1BC. The bounds are a plain four-float aggregate, matching native uninitialized construction; substituting nw4r::ut::Rect adds zero stores. The geometry namespace/function spellings are inferred and no neighboring340 source or split is taken over.

The strict frozen-DOL audit337-audit-load.py proves eight actual Item/Menu methods,1844 instruction bytes and seven EH records totaling92 bytes, with every instruction and target resolved. Both Load methods are included:60 and1176 bytes. Division by2.0f naturally produces the native operand order; multiplication by0.5f did not. Explicit spacing if/else branches preserve the native separate stores. Visibility uses TITLE_c and TITLE_DISABLED_c, distinct from TITLE and TITLE_DISABLED text pane names. Native OnInput is940 bytes, not the initially estimated956; its original split object and DD3D0-to-DD77C extent agree.

The recovered unadapted2006 MSL distribution is retained under198-reference/primary-header-research/alpha2006, with its primary manifest alongside. Original string uses representation-first compressed_pair and a raw-word zeroing loop. The minimal representation/copy candidate plus that original zeroing loop makes the actual CUIMenuItem constructor192 bytes, resolving the old196-byte result. The same combined producer preserves the eight previously proven methods. Audit337-audit-minimal-primary.py proves nine functions2036 bytes and eight EH records120 bytes, using the immutable minimal-zero/include snapshot. Representation-first storage with three reconstructed direct zero stores alone still produces196 and is insufficient.

Original msl_smart_pointers declares dynamic_pointer_cast and grants it access to the pointer/control constructor. Its native-supported outside implementation emits the292-byte cast used at800E91D0. Audit337-audit-selection.py also proves its96-byte EH record and the complete132-byte shared_ptr<CUIMenuItem> cleanup at800EC150 with40 EH bytes. Combined controls cover11 functions2460 code bytes and10 EH records256 bytes. This is an outside additive API proposal, not an accepted shared-header change.

The actual Menu UpdateSelection and OnInput bodies are reconstructed outside but are not accepted: the current selection variant has pane-base-position switch/context differences, while input has three extra branches after shared-owner cleanup and a few scalar operand/context differences. The indexed item getter's native4-byte forwarding entry at801DD780 and176-byte implementation at801DD790 need a source-supported API identity before publication. No redundant helper is introduced merely to reproduce these entries. The860-byte RunUIMenu and1624-byte menu runner, remaining emitted helpers, full data/vtables/EH, current consumers and frozen full-link gates remain outstanding.

## Fresh CUIList reading control

The same primary representation and original append-to-three-argument-replace declaration chain make retained CUIList Load2996 bytes with every non-relocation instruction exact, while all30 earlier functions remain exact. The three-argument replace definition is ordinarily out of class; the old direct five-argument append path reversed short-size temporary slots48/4C. This is a source-backed call structure, not a stack-placement shim. The outside object is337-CUIList-primary-append.o.

One call identity remains unresolved: this native Load calls the84-byte null-check helper at8007CC20, whereas the reconstructed common functional header names its byte-identical instance80070700. The bodies are raw-identical, but that does not permit conflating their addresses. A private per-signature helper or internal-linkage contract is only a hypothesis until actual callers establish it. The local-null diagnostic audit explicitly substitutes the native binding to inspect subsequent cleanup evidence and must not be cited as canonical acceptance. Its full Load EH proof is still incomplete; additional typed cleanup bindings must be established independently. No336 claim or canonical source change follows from this outside control alone.

## Traps

Keep shared outside overlays immutable during paired controls. A concurrently expanded string overlay briefly introduced exception-base declarations that conflicted with canonical derived what() exception specifications. The accepted Menu controls use explicit two-file snapshots instead. Re-read the actual minimal candidate: source-backed representation and zeroing are separate requirements, and a broader positive overlay does not prove each change independently.

# F-337-5: Original per-signature function emptiness checks

The recovered original2006 MSL functional header declares a private static is_empty(...) inside function<Signature>, followed by pointer/member-pointer and function-wrapper overloads. This resolves the identity question above: separate signatures have separate helpers. The earlier reconstructed common detail::is_null(...) helper incorrectly unified those identities. The primary functional SHA256 is bd50076df6dd0239ed82771fc4c46041e2b61701ec3a4d4f791ff09b762a218e; the complete distribution and provenance remain under198-reference/primary-header-research. No original implementation is imported wholesale.

The bounded canonical correction covers the existing one-argument function specialization. Its generic constructor calls its private static is_empty. Pointer, member-pointer and function-wrapper checks preserve the primary null-test semantics through ordinary overloads. The existing zero- and four-argument reconstructed specializations are outside this prerequisite's scope; no claim is made that the whole functional header now implements the complete original template.

Two actual native constructor bindings distinguish the otherwise identical84-byte helpers. The BUI-pointer callback signature calls8007CC20. The const CPiiListFilterSort-pointer callback constructor8009C700 calls8008FF10 at offset60. Both static helpers and their complete8-byte EH records match the frozen DOL in the actual all_source CDlgPiiSelector object. The strict audit337-audit-is-empty-native.py checks the source constructor relocation, frozen native call instruction, full helper code, native exception-index extent and complete EH payload:168 instruction bytes and16 EH bytes total. The recovered symbol names are is_empty__Q33std3tr118function<FP3BUI_v>Fe and is_empty__Q33std3tr135function<FPC18CPiiListFilterSort_v>Fe.

Current-base preservation uses759 actual source objects and710 Matching selections after consuming primary string prerequisite1194. Of those,725 objects remain whole-object identical and33 differ only in numeric labels or metadata. The remaining NonMatching CDlgPiiSelector object retains every old named symbol and all672 old numeric/data/EH payloads. Exactly two existing212-byte function constructors change only their null-check relocation to the corresponding private static helper. Two original84-byte helpers and their EH/index records are added; all other old instructions, semantic references, data, vtables and exception records are preserved. All163 previously100-percent functions in the two initially flagged objects retain their strict instruction and relocation streams. Allocated ELF sections and the raw DOL remain identical. Same-base reports and actual objects are retained in337-menu-reference/is-empty-control-b411c905,337-is-empty-preservation-proof.json and337-is-empty-old100.json. The official landing loop repeats these checks against each actual rebased head.

The corrected private identity plus the source-backed string append declaration chain makes retained CUIList Load2996 instructions/all96 targets and its complete744-byte EH record/all24 cleanup targets exact. Its layout reference-allocator cleanup is80084BA0, independently proven196 bytes with its40-byte EH record. This supersedes the earlier association with801DC380. The only frozen EH reference to801DC380 belongs to Menu's item-vector reserve801DDCA0; its precise typed emission and the original source-unit boundary require further investigation. Matching bytes from an arbitrary same-sized deleter do not prove that identity. The39-function11724-byte composite control and30 exception records2192 bytes include additional earlier cleanup/callback bodies and are not31-function CUIList acceptance or an issue closure.

## Traps

Identical variadic helper bodies can belong to different template signatures. Native caller bindings and the original private declaration resolve their identities; substituting the first matching body address is insufficient. Likewise, identical shared-owner deleter bodies cannot establish element or allocator-reference types. Follow the complete EH edge to its typed producer before naming it. The corrected source-bound controls preserve the earlier diagnostic artifacts without treating their wrong helper association as accepted evidence.
