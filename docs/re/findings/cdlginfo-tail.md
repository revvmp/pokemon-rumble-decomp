# CDlgInfo long tails

## 2026-11-04 constructor

The baseline has 72/75 exact functions. CDlgInfoItem construction now matches exactly in an isolated configured Wii/1.1 compilation, with no previously exact function lost. Update and wstring do_replace remain unmatched; the unit remains NonMatching pending completion and native linking.

The local vector<Line> copy specialization forwards through one outer helper and independent allocation/construction helpers. All three levels are necessary to reproduce the allocation-tag and four string-copy-tag slots. A sweep of 27 depth combinations identified the 1/1/1 structure as the only tested arrangement with every slot correct.

The remaining register cycle was caused by the local end pointer retaining the wrong allocation identity. A local Range containing first and last pointers, initialized with separate assignments, reproduces the target registers without adding instructions or storage. A one-pointer struct or union also matched; a vector const_iterator and integer/enum pointer round trips did not. An earlier scalar declaration of count improved one register but left the three-register cycle. The two-pointer Range is retained for clarity.

These local inline layers and range view are D-0016 allocation/stack-layout fakes. They copy the same source elements through the existing allocator and preserve normal cleanup. Evidence: ignored tools/local/session/info-owned-tail/ctoraccepted.cpp, .o, and .json. No shared STL/header change is used.

## Remaining investigations

Update differs only in the registers for hasText and hasColumns. Simple enum views, local declaration order, and scalar bool aggregates have not matched; enum views add boolean conversions and aggregate views disturb other register assignments. Prior root visibility helpers also failed.

## Update register identity

Update is now exact as well, bringing isolated coverage to 74/75. GetInfoTextFlag computes the string-size comparison directly into an output reference, preserving hasText as a named allocation value. The columns flag uses the direct conjunction expression, making it a temporary allocation value. Together these reproduce exactly r24 for hasText and r29 for hasColumns. Either change alone fails; forwarding through Line::HasText in the output helper also retains the wrong temporary. This source-local output helper is a D-0016 register-allocation fake. Scratch visibilityout1 contains the comparison evidence. The wstring do_replace tail is still pending.

## Wide-string replacement

All 75 owned functions are now exact in isolated compilation. The final do_replace difference was a cached pointer difference surviving across allocation, which removed three target instructions and changed the nonvolatile register set. Evaluating the later memmove length through nonconst pointer views prevents this common-subexpression merge: `(const_cast<pointer>(last) - const_cast<pointer>(first)) * sizeof(charT)`. The views only participate in subtraction; they do not read or mutate pointees. A const-volatile pointer view also matches. std::copy, char_traits copy/move, and another std::distance call do not.

The expression is confined to CDLGINFO_EMISSION_LAYOUT in include/string and activated only by this source file. Existing string gates and all other translation units retain their original body. This qualification view is a D-0016 common-subexpression fake. Evidence is replacecopy4 under the isolated scratch directory. Native emission ordering and source-link verification remain pending.

## Native emission preparation

The final isolated candidate has all 75 instruction streams in exact order and every exception-table byte and cleanup relocation correct after five inferred-name corrections. Original shared_ptr destructor bodies are identical, so function matching alone did not establish their type identities. The cleanup owners establish this cycle: old Item means Impl, old Impl means Info, old Page means Item, old Info means ItemLine, and old ItemLine means Page. Detailed old/new symbols and all affected cleanup-owner functions are recorded in ignored info-owned-tail/renames.json and rename-evidence.json for the parent to apply within this unit only.

CDlgInfo's Calc/Draw overrides are declared later in the class to obtain their emitted order; inherited vtable slots remain unchanged. Explicit empty CDlgInfo and CDlgInfoItem destructors were removed so the compiler emits their equivalent implicit virtual destructors in the late wave. Source-local sizeof owner/deleter/move-guard views establish the remaining template instance order without runtime storage. These emission triggers are additional D-0016 fakes.

GetTextWidth now calls a source-local nothrow declaration of the existing C ceil symbol. The default declaration incorrectly generated a Rect cleanup around a nonthrowing math operation, adding 24 bytes of exception metadata. The nothrow declaration removes exactly that excess while preserving all instructions. The native source-link trial is pending.

## Metadata ordering

The first native trial kept every owned function at its original address, but emitted the 64-byte default_delete<CDlgInfo> RTTI/name block before the Page, Item, and Impl shared-deleter metadata. A source-local sizeof(default_delete<CDlgInfo>) trigger before the other deleter triggers moves that block after all three, exactly as the original. No RTTI alias or object rewriting is needed. The isolated defaultseed2 candidate retains all 75 exact functions in exact order. Native verification is pending the parent rebuild.

The metadata trigger reduced the combined native trial to one differing byte in the entire DOL: offset 0x3E5D68, address 0x804B9DC8. The existing text-box width literal was 1.0f, while the original constant is 0x44800000, or 1024.0f. Correcting the layout rectangle width to 1024.0f fixes this data reconstruction error; instruction matching had hidden it because it compared relocated constant references. Final native validation remains with the parent.

## Acceptance

The parent rebuilt CDlgInfo as Matching through the real shared native pipeline. Ninja exited 0 and the frozen-manifest verifier passed after the literal correction. All 75 functions, owned metadata, exception tables, and linked bytes are accepted; the original hashes were not changed.
