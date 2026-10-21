# F-336-7: IUIListItem interface prerequisite

Part of #336. This header-only prerequisite selects no new source and does not complete either #336 or #337. The retained CUIList source unit spans both owned issues; its actual NonMatching integration is being prepared separately.

## Native declaration

IUIListItem is a four-byte abstract interface without fields or base classes. Its20-byte vtable at803C14FC contains RTTI804B5FD0, a zero offset, a pure destructor slot, the true-returning method801DA100 and empty update801DA110. The out-of-line abstract destructor body is801DA0C0. Native call801DB954–801DB978 supplies CUIList* and CLayout* to the final virtual slot. The original class name is recovered; IsSelectable, Update and the const qualification of IsSelectable remain descriptive source inferences. They are not recovered mangled spellings.

The first retained source function801DA0C0 belongs to #336. Earlier coalesced RTTI at804B5FD0 names IUIListItem through string8039C390; it does not transfer source-file ownership to an earlier caller. Its native12-byte name and full8-byte RTTI match. The interface destructor64 bytes, true-returning method8 and empty method4 match all76 code bytes and the destructor delete destination. The full20-byte vtable and8-byte destructor exception record at8003EB10 also match after relocation resolution.

CUIMenuItem's native destructor801DC780 calls this base destructor, and its constructor801DC6C0 puts the first derived field at4. Independent #217 native CUIPiiListItem has its first field at4 and complete size1C. The shared declaration is provided before that contributor publishes a derived header; no competing interface is introduced.

## Validation

Current baseline bf6e1899 was configured and built with actual all_source before adding the header. Candidate all_source exits0, with all725 existing source objects whole-object byte-identical. This new header is unused by the existing725 consumers. Explicit canonical-header producer336-item-header-probe.cpp and audits336-audit-item-header.py /336-audit-item-header-data.py prove the code, complete vtable, RTTI/name and exception record above. The immutable outputs are336-item-header-native-proof.json and336-item-header-data-proof.json in336-reference; current consumer manifest/result are336-item-header-sections-before.json and336-item-header-control-result.json in329-reference. These are outside diagnostics, not source selection.

## Traps

An earlier research PR890 mentions general BUI/UI manager work but does not own this interface's first retained function. Check the native first-function boundary and current claims rather than treating a dependency reference as a class-header takeover authorization. Earlier RTTI coalescing remains recorded separately from source ownership.
