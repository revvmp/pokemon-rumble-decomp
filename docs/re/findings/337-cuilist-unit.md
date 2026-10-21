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
