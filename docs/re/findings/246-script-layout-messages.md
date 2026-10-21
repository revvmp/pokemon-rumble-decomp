# Script message and layout bindings (#246)

## F-246-1: original inventory and boundaries

The original issue lists 25 functions and 8,104 bytes. Its first function, `0x80142070` (116 bytes), is the `UsedPassword` vector cleanup already accepted with #245. The first new source function is `CatGets` at `0x801420F0`. A message-formatting unit ends after the character formatter destructor at `0x80143158`; the layout unit starts at `0x80143160` and ends after the light-set deleter destructor at `0x801440A0`. These names describe behavior; no original source filename has been recovered.

| Address | Bytes | Function |
| --- | ---: | --- |
| 0x80142070 | 116 | Previous ScriptItem vector cleanup, accepted #245 |
| 0x801420F0 | 192 | CatGets |
| 0x801421B0 | 80 | GetConsoleRegion binding |
| 0x80142200 | 80 | GetGameRegion binding |
| 0x80142250 | 80 | GetConsoleLang binding |
| 0x801422A0 | 80 | GetGameLang binding |
| 0x801422F0 | 472 | Format binding |
| 0x801424D0 | 1464 | File-local wide Format overload |
| 0x80142A90 | 1460 | File-local character Format overload |
| 0x80143050 | 264 | Character formatter destructor |
| 0x80143160 | 848 | Layout motion dispatcher |
| 0x801434B0 | 528 | NewLayout |
| 0x801436C0 | 176 | method_layoutDestroy |
| 0x80143770 | 8 | method_layoutPlay |
| 0x80143780 | 8 | method_layoutRequest |
| 0x80143790 | 8 | method_layoutRequestNext |
| 0x801437A0 | 612 | method_layoutSetString |
| 0x80143A10 | 372 | method_layoutSetVisible |
| 0x80143B90 | 336 | method_layoutGetLength |
| 0x80143CE0 | 496 | NewWorldLightSet |
| 0x80143ED0 | 76 | Light-set deleter lookup |
| 0x80143F20 | 36 | Light-set disposal |
| 0x80143F50 | 132 | Light-set shared-owner destructor |
| 0x80143FE0 | 116 | Light-set raw-owner destructor |
| 0x80144060 | 64 | Light-set deleter destructor |

The outside actual producers are `246-reference/ScriptMessages-overloaded.cpp`, `ScriptMessages-runtime.cpp`, and `ScriptLayout.cpp`. Their code audits are `246-audit-messages.py` and `246-audit-layout.py`. These are reconstruction diagnostics, not whole-unit acceptance. No source-selected link or closure is claimed here.

## F-246-2: primary string zeroing contract

The recovered original 2006 MSL string header declares a private array-reference `word_array` and `words()` getter, then defines `zero()` out of class. Its body obtains `word_array w = words()` and clears each word. The canonical representation already has the same three raw words but its initializer accesses the array directly.

The actual nine-function message producer isolates this declaration contract. With the independently recovered formatter parser and canonical string it has six constructor register differences in each local Format overload. Moving the zeroing body out of class alone preserves those differences. Restoring the genuine `words()` accessor layer resolves all twelve differences: all nine native instruction streams and all 88 direct call targets then match. The full original 2006 string oracle independently produces the same result. No allocator signature or object layout changes are involved.

Outside evidence is `246-primary-zero-control.py`, `246-reference/zero-outclass/string`, and `246-reference/ScriptMessages-zero-outclass-{code,calls}.json`. The original header is retained under `198-reference/primary-header-research/alpha2006/CW_RVL/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/string`, with the recovered-source manifest in that research directory. Same-base current-consumer controls are under `246-zero-controls/`; publication requires every current source object, existing native match, linked ELF and frozen DOL to preserve its accepted payload.

The native retained zeroing wrapper at `0x80087BA0` calls the four-byte identity getter at `0x80087BD0`. The original accessor declaration and actual wrapper bind that getter as wide-string `words()`, superseding its placeholder name without changing its extent or ownership. The actual message producer also has nine exact exception records totaling 136 bytes and all four cleanup targets exact.

The first same-base control on `c20030ab` builds 770 actual source objects: 752 whole objects are identical and thirteen differ only in numeric metadata. Five existing NonMatching objects have source-context or weak-emission changes. The generic native old-100 guard from the #152 prerequisite checks all 470 previously 100-percent functions in those five objects and preserves their complete bytes and semantic relocations: 139 in CDlgPiiSelector, 212 in CMsgBalloon, 34 in CNoticeDisplay, 29 in CPauseMenu and 56 in SaveData. Three unfinished PauseMenu formatters improve from 35 to 28 differing words and an unfinished SaveData formatter improves from ten to three. Every allocated linked ELF byte and the frozen DOL remain identical. Independent actual Balloon controls preserve 436 native code streams and 226 exception records (`152-reference/pad246-words-native436/native-proof.json`). The official merge verification repeats these controls against its actual latest-main base.

## F-246-3: layout caller contracts

The layout caller uses the existing `IGameMap::GetLayoutManager` virtual slot `0x54`. Its manager calls are Add at slot `0x1C`, Get at `0x20`, and Remove at `0x24`, independently matching #139's native vtable. IDs are signed `int`; Get returns an eight-byte shared owner by value. An outside extension of the existing `ILayoutManager` interface uses `shared_ptr<ICalcDraw>` throughout, with Add receiving a const reference. NewLayout constructs that owner directly from a new 232-byte CLayout, avoiding an artificial owner conversion.

The ordinary local PopString helper owns its Slice only for the conversion call. This source lifetime reproduces the dispatcher, SetVisible and GetLength temporary placement. Retaining the Slice in each binding's outer scope gives the same behavior and sizes but wrong native stack offsets. The final actual layout producer has fourteen exact functions and all 90 direct call targets; NewLayout has five remaining setup words at LoadFileList. Default-argument omission leaves those five words unchanged.

SetString converts character text with the actual SDK Shift-JIS converter into a stack buffer, terminates it, calls CLayout::SetText, then CColorBtnTagProc::Attach on the returned TextBox. Wide text is passed directly; other input becomes an empty wide string. NewWorldLightSet swaps the engine register, obtains its world before converting the owner for Add, and returns the new object's ID only when Add succeeds.

## Traps

- Template Format helpers have the right behavior but four extra bytes per helper and incorrect local pool context. Ordinary file-local overloads reproduce both native extents and all calls; no pragma or compiler change is necessary.
- The repository has two historical complete NIntl formatter headers. The legacy game header's parser and the modern independently recovered parser differ in expressions and visibility. Combining declarations without preserving actual consumer proofs is unsafe. The native character parser uses an `int` character temporary, as already represented by the legacy header; a `charT` temporary leaves two register words different.
- A raw CLayout pointer cached across the pane lookup changes the dispatcher's register lifetime and adds sixteen bytes. Using its retained shared owner at each actual call reproduces native code naturally.

## F-246-4: preserved complete source checkpoint

The string prerequisite merged as PR #1243, `b276fd0495`, after the official loop verified exact head `b5f8a1828a3639d048bd4dd3d8c738d3085a21aa` on base `82f86b5dbf`. Latest-head receipts are outside at `246-zero-land-controls/<full-base>/<full-head>/`. This adds no reconstructed source coverage and leaves #246 open.

Further actual-source work is frozen at `C:/MassStorage/GitHub/246-reference/246-checkpoint-20261021/`. The portable archive `246-checkpoint-20261021.zip` has SHA-256 `faf08f11d04b9f0def489c1f88f11f1bf47128a3f19de42ed9955e2c9f96749e`; its `sha256.json` inventories 162 input, source, proposal, object, disassembly and proof files. `checkpoint.json` explicitly records that neither unit is registered or selected for a source link. The original REST issue inventory is included. Reproduction scripts retain absolute research paths and must be pointed at a fresh legitimate worktree before reuse.

`ScriptMessages-overloaded.cpp` is the ordinary source candidate. The `coherent-formatter/` proposal removes the duplicate formatter class declaration: the legacy game header forwards to the common declaration and retains constructor/destructor definition visibility for its existing callers. The common class retains the legacy format-specification interface, the independently proven parser expressions and an `int` character temporary. This proposal and the now-accepted string getter reproduce all nine functions, 4,172 instruction bytes, 88 direct call targets, nine exception records totaling 136 bytes and four cleanup targets. `ScriptMessages-coherent.o`, its code/call proofs and `messages-zero-eh.json` preserve that actual producer. The proposed formatter headers are not published.

Same-base `246-formatter-controls/` compiles all 771 current source objects and passes both fresh frozen-DOL builds. Of these, 765 whole objects are identical and ArgumentFormatter changes numeric metadata only. Five existing NonMatching objects change context: CMsgBalloon, CNoticeDisplay, CPauseMenu, CTitleSession and SaveData. The generic native old-100 guard preserves all 344 previously 100-percent functions byte-for-byte with semantic relocations, and the per-function EH guard preserves all 185 corresponding exception records. The allocated linked ELF and frozen DOL remain identical. These are header controls, not acceptance of either proposed source unit.

The complete original 2006-header oracle, `ScriptMessages-runtime-primary.o`, also reproduces every unrelocated byte of the actual 954-byte `.data` payload beginning at `0x803B6980`. The following object begins at `0x803B6D40`, after ordinary alignment. This is a diagnostic about the original declaration context, not permission to trim or rewrite source bytes. The canonical producer instead emits 962 bytes and has different weak metadata placement. A real source-selected link and exact allocated-data proof are still required. Merely moving the deleter definition earlier, changing its constructor parameter to the primary `D&`, moving the error-string constructor out of class, or adding primary `what()` exception specifications did not resolve this metadata difference. Their sources, objects and negative receipts are retained.

## F-246-5: remaining acceptance work

`ScriptLayout.cpp` recovers all fifteen original layout-unit functions. Fourteen instruction streams and all 90 direct call targets are exact. NewLayout has its native 528-byte extent but five different setup words at LoadFileList. Explicit unsigned-long zero, default-argument omission and the permitted Wii/1.3 control preserve the same five differences. Do not repeat them without new declaration evidence.

SetString has all 612 instruction bytes and actual calls exact, but its exception record is 56 bytes versus the native 84 bytes. The native record divides the conversion and text-setting calls into additional protected ranges and includes an additional action involving saved register 27. The nature of that action remains unproven. A variable-length array is rejected as an illegal constant expression by the actual compiler; a raw-pointer conversion helper changes the function to 616 bytes without fixing its EH; the executable contains the string `__stack_alloc` but that spelling is not a declared source intrinsic. None is an accepted workaround.

Other layout exception records reproduce their unrelocated payloads, but the borrowed shared-ICalcDraw destructor at `0x800705F0` still needs complete typed-body/target proof and a canonical name before source integration. The Add/Get/Remove extension to the existing ILayoutManager header remains outside. First-owner #139 has no active competing proposal, but its own complete original inventory is not part of this claim.

Both translation units still need authoritative splits, source registration, whole-object data/RTTI/vtable proof and a fresh source-only Matching ELF/MAP/DOL gate. No partial source or fabricated Matching entry is published. The claim is released with `--blocked` because the remaining canonical declaration/metadata and exception-scope contracts lack discriminating evidence; work has not been presented as a complete issue.
