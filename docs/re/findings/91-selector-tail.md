# CDlgPiiSelector final helper tail

## Accepted function gains, 2026-11-04

The coordinator verified 643/645 real functions after the following two source additions and the scoped symbol correction. The shared fallback build completed successfully and full DOL SHA verification passed. CDlgPiiSelector remains NonMatching: this is function acceptance, not a source-link acceptance.

### Number conversion emission

The existing `Metrowerks::number<unsigned long, 1>` conversion operator compiled to the exact eight-byte target once an external, unused `GetSelectorAllocationCount` caller requested its emission. An unused static caller, explicit template instantiations, and a conversion specialization did not emit it. Taking the conversion address emitted it but disturbed data offsets and regressed Setup/CreateBall. The retained caller has no intended runtime role and is a D-0016 emission fake. Its removal by the final link must still be verified when this unit becomes Matching.

### Duplicate minimum

The original unit contains two identical six-instruction integer-minimum bodies. The reconstructed template emits only one. The existing global template at 0x80095280 remains unchanged. The local 24-byte body at 0x8009CEA0 is represented by `SelectorMinimum`, a force-active static C++ function returning `b < a ? b : a`. The coordinator renamed only that local target symbol to `SelectorMinimum__FRCiRCi`. The clone matches all six instructions and preserves the second original body. This is a D-0016 duplicate-function fake, not a recovered original name.

## Remaining assign and insert helpers

The list-iterator vector assign wrapper matches all instructions except the stack locations of its empty int2type tag. The target loads the original tag at stack +8 and copies it to +12; source reverses these two slots. The insert wrapper also needs that layout and retains the tag-copy address in a saved register before converting its position iterator. The source computes the address after conversion instead.

Selective inline helpers, explicit dispatch specializations, dispatch definitions delayed to the end of the unit, always_inline/noinline attributes and pragmas, and wider tag storage did not produce an accepted improvement. Changing the unit's final dont_inline state affects hundreds of emitted templates and is not a local fix. Wider tag storage shifts iterator stack slots as well as the tag. No such candidates were retained.

## Traps

- A missing conversion operator can require a live external emission request even when explicit template instantiation is present. An address-taken data object can change unrelated data references; compare all functions, not only the missing helper.
- A local duplicate template body requires explicit representation and a scoped symbol identity. Keep the existing global body and document the clone; function comparison alone does not prove the unit links at the original positions.

## Explicit tag references, 2026-11-04

A fresh comparison found a third residual in Clamp: it still referenced the global integer minimum after the duplicate local body had been named SelectorMinimum. An explicit Clamp<int> specialization now binds the call to the retained local clone.

The two vector range-dispatch templates now take their unused int2type tag by const reference only under GAME_DLGPIISELECTOR_RANGE_TAG_REFERENCE. Their generated 68-byte bodies remain identical. The ABI already passed the empty by-value tag indirectly, so the changed source signature preserves its physical argument register. The scoped target names at 0x800929F0 and 0x80091A40 must use the const-reference mangling. This is a documented D-0016 interface reconstruction, not evidence of original source spelling.

The assign specialization declares its copy tag before the source tag and explicitly copies one object-representation byte through volatile unsigned-character lvalues. This reproduces the target's stack +8 to +12 copy. The insert specialization uses the address of that assignment expression as the dispatch argument, converts it back to the original tag pointer, and dereferences it. The address remains the address of the same tag object; there is no hard-coded frame address or instruction stream. Retaining the assignment lvalue forces the compiler to preserve the tag pointer in r31 across iterator conversion, exactly as the target does. Computing the byte copy and pointer separately rematerializes the pointer too late and loses one saved register.

Direct compilation of the retained source and normal headers matches all 645 real functions after the two diagnostic signature renames. Scratch evidence is tools/local/session/282-selector/tracked-final-diff.json. The coordinator completed a source-link trial successfully, but its frozen checksum failed. The unit remains NonMatching; 645/645 is function acceptance only. Failed linked artifacts are preserved in tools/local/session/282-selector/linked.elf and linked.dol.

Negative controls included four one-byte struct/union tag types, explicit copy constructors, reference forwarding helpers, pointer/integer casts, named temporaries, placement new, and reference-bound temporary tags. A reference-bound temporary restored the required r31 lifetime but reversed the two tag slots. Placement new emitted an unwanted call. The successful assignment-lvalue expression avoids both problems. The volatile byte copies and unusual argument expression are D-0016 fakes after the documented natural-source attempts; no original bytes or assembly enter the source pipeline.

### Additional trap

An expression that yields the same pointer can have a different register lifetime: a named tag address was rematerialized after a call, while the lvalue result of its byte assignment survived across that call. Compare the entire unit and the link, rather than assuming equivalent C++ expressions share allocation behavior.

## Source-link audit

The first full source-link attempt retains all 645 target function bodies, but their order differs. The global text start shifts by 16 bytes from an exception-table size change, while the selector function extent itself retains its size. The linked data section is smaller.

The vtable audit identified swapped inferred names for the two identical eight-byte CDlgPiiSelector methods: 0x8009AC10 is Close, and 0x8009AC20 is IsClosable. The inherited IDlg slot ordering establishes this assignment; changing IDlg would be wrong. The coordinator applied only those two local symbol corrections. All twelve target vtable relocation tables otherwise agree with the source.

The source .data string inventory has 113 entries versus 115 in the target. The only missing strings are the default_delete and shared_ptr_deleter RTTI names for std::tr1::function<void(BUI*)>. Adding the ordinary inline BUI::CCallbackListOwner::Add body, already reconstructed in CDlgCollection and CDlgPiiList, restores both strings and retains all 645 exact functions. This remains a scratch candidate pending integration with emission-order changes.

Moving the explicit assign/insert bodies into the existing unit-scoped generic vector wrappers also retains all 645 exact functions and avoids early strong definitions. A scoped generic Clamp call to SelectorMinimum avoids its early strong specialization. Neither alone solves the remaining weak-function order. Typed unused member-function pointer probes add data and disturb Setup/CreateBall relocations; sizeof probes avoid that data but did not recover the desired swap/iterator ordering. Do not retain the address-taken-data probes.

## Retained data-order baseline

The next retained source baseline still compares 645/645 against the regenerated real target. It includes the callback body described above, compile-time sizeof instantiation requests for shared_ptr<CItem>, shared_ptr<CUIManager>, shared_ptr<CPii>, shared_ptr<BUILayout>, shared_ptr<CUIPiiSpec>, shared_ptr<CUIPiiList>, shared_ptr<CUIPlayerID>, and function<void(const CPiiListFilterSort*)>. Their declaration order and position after CFiber and before CLayout reproduce the original template data order. The requests generate no runtime code or data; they are explicit D-0016 emission scaffolding, not recovered original source.

ICalcDraw, memory, and stdexcept are included before IDlgPiiSelector, and the local IDlgPiiSelectorInternal definition is placed immediately before CDataPerPlayer. Together these changes reproduce all 115 target .data strings in order and the offsets of every common named vtable/RTTI symbol. The source .data ends fourteen bytes before the split's padded extent; final-link boundary treatment still requires validation. The target-function order common subsequence improves from 514 with the initial strong specializations to 596 of 645. The unit remains NonMatching until all ordering and exception data have passed the frozen SHA build.

The native compiler accepts descriptive custom code sections. A toy partial-link experiment found that plain mwldeppc -r ignores LCF section ordering and merging, preserving its input sections even with deliberately invalid selector syntax. This cannot replace the existing C++ source-fragment pipeline. No custom-section changes were retained.

### Implicit destructor emission and fragment prototype

Removing the redundant empty `CDlgPiiSelector` destructor preserves all 645 target function bodies and identical `.data` bytes. The implicitly generated virtual destructor moves from owned ordinal 361 to its exact target ordinal 525, between the same two target neighbors. Moving an explicit inline definition out of the class did not produce this correction. This is retained as ordinary C++ source; the unit remains NonMatching pending complete emission ordering.

An isolated native three-object prototype compiles the real item-building function, twelve strong template specializations, and the remaining source, then joins those compiler outputs with the native unoptimized `-r` linker. It preserves all 645 function comparisons after one diagnostic inferred namespace rename and improves the target-order common subsequence to 607. It is not integrated: its implicit iterator copy constructor is emitted too early, and moving a larger requester fragment also claims shared-pointer RTTI before the main object. Declaring the implicit copy constructor explicitly changes caller triviality and breaks the item-builder comparison. No original object is an input to the native join. Early explicit instantiations alone reached 602; moving the compressed-pair primary declaration ahead of the includes did not improve their order. Scratch evidence is under `tools/local/session/282-selector/front` and the `282-selector-implicitdtor`, `instearly`, and `primarypair` probes.

The derived selector overrides now declare `IsClosable` before `Close`, matching their inherited slot order and the target function emission. All 645 function bodies and raw `.data` bytes remain identical. Together with the implicit destructor correction, the retained order common subsequence rises from 596 to 598.

A selector-only `GAME_DLGPIISELECTOR_EMISSION_LAYOUT` memory-header gate moves the unchanged `weak_ptr<T>::reset` body outside its class, keeping `swap` inside. This preserves all 645 bodies and raw `.data` bytes while moving both CPii/CItem resets and swaps into their target emission waves; order common subsequence is now 602. Moving both bodies produced no order gain, and moving swap alone regressed order. This declaration-placement scaffold is D-0016 rather than evidence of the original header layout. The selector define is scoped to this translation unit and the existing BWorld gates remain intact.

The same scoped out-of-class definition placement for `Metrowerks::number<T, Value>::operator T()` restores the unsigned-long conversion between its exact target neighbors. This unchanged body is gated in `alloc_ptr.h` by the selector emission macro. The real-header compile remains 645/645 and `.data` byte-identical; retained order common subsequence is 603. Moving vector clear_moved overload bodies out of class did not improve order; moving assign_reallocate inside its class regressed order.

Additional negative definition-placement controls preserved all function bodies but worsened or did not improve order: moving shared_ptr reset(Y*) or the unary function functor constructor inside their classes, moving shared_ptr assignment or swap inside its class, moving shared_ptr use_count outside its class, and moving CRemoteMemory::HasFlag outside its class with or without inline. None is retained. The retained 603 common subsequence is still insufficient for source linking.

Refreshing the native fragment prototype after the retained ordinary fixes yields 645 exact functions and target-order common subsequence 614. A separate scratch metadata-owner experiment uses declaration-only virtual methods and uniquely tagged constructors to ask the compiler to emit genuine vtables/RTTI without owned helper bodies. This works for the first shared-pointer deleter, but per-fragment `.data` pooling still places its type-name strings before the main object's remaining vtables; a complete metadata owner would be required. No such scaffold is integrated and final stripping of its requester constructors is not yet proved. `__declspec(novtable)` is accepted by Wii/1.1 and suppresses metadata, but also removes constructor vptr stores, so it is unsuitable here and was not retained. dllimport/import/export attributes on the toy class do not suppress metadata. Scoped late defer_codegen pragmas made no order change.

A native final-link toy also rejects independent section ordering as a metadata-ownership fix. With A.o before B.o and duplicate Base vtables, reversing only the final LCF `.data` selectors to B.o then A.o still selects A.o's vtable/RTTI in both successful link maps. The [native reference manual](https://www.nxp.com/docs/en/reference-manual/PA_Build_RM.pdf) documents file/section selectors, but these affect placement rather than the earlier duplicate selection in this tested toolchain. Reproducer and maps are `282-selector-lcfweak.py` and `282-selector/lcfweak`.
