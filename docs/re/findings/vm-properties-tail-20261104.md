# VM and property-data tail reproduction

## Scope and evidence

This 2026-11-04 pass owns NPpuVM.cpp and PiiPropData.cpp. It uses the configured native compiler command, isolated source/object outputs and full-unit objdiff JSON. It does not run configure, ninja builds, full verification, or edit shared headers. No candidate in this pass is accepted as a source-linked unit.

The older ignored tools/local/session and tools/local/perm directories referenced by the earlier findings are absent on this computer. The strongest earlier candidates were reconstructed from [NPpuVM](282-nppu-tail.md) and [PiiPropData](piipropdata-tail.md), preserving the current exact destructor and SpeciesMoves assignment specializations.

| Function | Native bytes | Reproduced comparison | Remaining difference |
| --- | ---: | ---: | --- |
| CCode::Load | 468 | 94.871796% | Three adjacent instruction-order swaps |
| CEngine::CollectGarbage | 1400 | 99.87143% | Nine character-sweep r29/r30 operands |
| PiiPropData constructor | 1048 | 99.0458% | Register operands only |
| PiiPropData::Reset | 1260 | 99.46032% | Register operands only |

Every previously exact function remains exact with each reproduced best candidate. These percentages are isolated function comparisons, not acceptance evidence. No tracked source change is retained from this pass.

Fresh reproducers are tools/local/tail-20261104/vmprop_load.py, vmprop_collector.py, vmprop_properties_rebuild.py and vmprop_properties_vector.py. Their best outputs are game_NPpuVM/vmprop-load-cached, game_NPpuVM/vmprop-collector-rebuilt with vmprop-include/map, and game_PiiPropData/vmprop-vector-1-1-1. Each output directory contains the source, object, compiler log and diff.json.

The local vmprop_probe.py wrapper freezes the configured command and original target objects under vmprop-targets so coordinator clean builds do not invalidate ongoing comparisons. Compiler tracing uses tools/re/ra_trace.py and the private Frida installation in tools/local/tail-20261104/animcurve-frida. It never changes the emitted instructions or compiler state.

## Load scheduling

The fresh trace vmprop-load-trace.txt confirms that all three order differences arise in pre-allocation scheduling. Before that pass, the allocation blocks are load header size, store malloc result, compare size, branch. The scheduler moves the compare ahead of the store. The magic block similarly moves SUBIS ahead of the first null-pointer store. All original operations, register choices and branch destinations remain present in the 468-byte best candidate.

No improvement came from the available opt_* on/off controls, scheduler models, pointer-storage aggregates and constructors, allocation helpers returning pointers or aggregates, field enum views, same-type value assignments, equivalent zero dependencies with optimizer controls, or local scheduling pragmas. Explicit label/loop/switch boundaries disappear under normal optimization; disabling control-flow optimization retains unwanted differences. A 604 scheduling model restores the two allocation-block store/compare orders but introduces other ordering differences, including the epilogue, so it is not a solution. Wii 1.0, 1.1, 1.3 and 1.5 produce the same residual for this candidate. Other attempted compiler revisions rejected compilation; they provide no code-generation evidence.

Count-leading-zero predicates are not a match: they can consume loop-alignment padding while retaining extra instructions. Their superficially similar score must not be treated as an improvement. Exact byte count and instruction contents were inspected separately.

## Collector coloring

The fresh collector trace vmprop-collector-trace.txt gives the character erased-node temporary virtual 60, degree 22, colored r30; the character receiver is virtual 41, degree 24, colored r29. The analogous wide temporaries are virtual 54 and 39 and already have their target registers. The full marking phase and wide sweep are exact.

An ordering hypothesis, not a completed solution, follows from the color pool: coloring wide node, wide receiver, character node, then character receiver could allow the character node to reuse the already opened r29 while leaving the wide node in r30. The current relative order is character node, wide node, character receiver, wide receiver. No tested source arrangement achieved the desired interleaving without changing other instructions or lifetimes.

Fresh rejected controls include unused erase return values; node holders with empty constructors, destructors and special members; iterator operator-call spellings; pointer and reference erase contexts; typed and void-pointer reassignments; lvalue identity assignments; caller node-address arguments; receiver getter chains; aggregate-return node and receiver views; mixed character/wide receiver forms; volatile global-pointer views; and dead assignments under the relevant optimizer control. Void-pointer reassignment and aggregate-return controls that completed other units during the same session did not transfer to this residual.

## Property-data reconstruction and coloring

The outer size-constructor specialization in the earlier best source is the actual vector<SpeciesMoves> constructor, not __vec_constructor<SpeciesMoves>. This distinction is necessary to reproduce the recorded stack ordering. Allocate in that constructor, wrap only construct_at_end in an inline helper, and retain the named remaining count in the outer construction loop. Explicit LevelMove and unsigned-short storage copy constructors use construction-tag declaration before allocation-tag declaration, with volatile byte initialization immediately before the respective calls. A one-field aggregate holds each copy end pointer. This combination reproduces all native opcodes and stack offsets at the percentages above.

The fresh trace vmprop-properties-trace.txt records the first constructor. Its outer remaining counter colors before this; the native stream needs this above the copy-pointer locals, with zero and the remaining counter below them. The short-copy count is colored before its length-error string/object temporaries. The LevelMove copy then reuses the already opened registers, so the similar count/string/object cycle in that earlier block is not independent of the short-copy ordering.

No exact gain came from split declaration-order permutations, the available opt_* controls, void-pointer receiver views, integer widths and bitfields, copy-count aggregate returns and accessors, separate allocator argument forms and forwarding depths, direct allocation/error helper depth controls, count lifetime identities, or flattening the signed-short adapter copy constructor. Named outer tag controls reach 99.496185% and 99.580956% with native sizes, but still move multiple tag slots; these are not a stronger opcode-and-stack match than the reproduced 99.0458%/99.46032% candidate.

The scoped c9x_alias_by_type control that completed another unit in this session leaves Load and both property functions unchanged and regresses the collector. It is not retained here.

## Traps

- Reconstruct the actual vector size constructor when following the earlier property-data recipe. Specializing only its __vec_constructor backend changes inline parity and does not reproduce the recorded stack slots.
- A higher percentage can trade register differences for wrong stack offsets, and unchanged percentages can conceal added instructions. Keep native size, opcode/branch alignment, stack operands and register operands separate when selecting a candidate.
- The new void-pointer reassignment and aggregate-return controls are local compiler behaviors. Their successful use in another unit does not establish that they alter these surviving virtual registers.
