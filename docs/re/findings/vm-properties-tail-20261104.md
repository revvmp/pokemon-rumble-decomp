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

Every previously exact function remains exact with each reproduced best candidate. These are the initial reproduction results, superseded for the collector by the completion below. The percentages are isolated function comparisons, not acceptance evidence.

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

## Collector instruction completion after the remote-memory pass

The later 2026-11-04 pass completes all 1,400 unrelocated bytes of CollectGarbage. Its exception record and cleanup relocation identities also agree. All 61 previously exact functions retain their scores, giving 62 of 63 target functions at 100%; Load remains at its tracked 88.051285% baseline in the ready package. The separate 94.871796% Load experiment is deliberately not included in this exact collector change.

The new control changes the ordering of the two sweeps' allocations. A local CharacterStringSweep helper performs the character-map erase through the compact tree's proven three-word storage: count at zero, root at four, and first node at eight. Its outer method takes the original map iterator by value; an inner method takes the native iterator by mutable reference. The receiver is loaded through a volatile view of the existing sStrings global, preserving the target reload. Advancing the local erase position leaves the caller's iterator unchanged. Tree balancing, node deletion and count adjustment are unchanged; the stored pair consists only of a pointer and an unsigned integer and has no nontrivial destruction.

This helper initially retains the same 99.87143% score as the prior best but moves all nine differing operands from the character sweep to the wide sweep. The character sweep is now exact. Keeping the wide erase's removed node as its concrete node pointer, rather than the base-node pointer, completes the wide sweep in this new coloring context. The same type change in the earlier context had not completed the function.

The ready source defines NPPU_VM_GC_LAYOUT before its header. The private map overlay adds the three const-end accessors and switches the compact erase to a mutable iterator reference with a concrete node pointer only under that define. Other translation units retain their original header bodies. The source-local storage view, explicit helper levels, volatile receiver load, iterator lifetime control and scoped header layout are intentional D-0016 accommodations, not recovered original interfaces. No assembly, object patching or original-byte change is involved.

The reviewed package is `tools/local/tail-20261104/game_NPpuVM/vm2_collector_ready/NPpuVM.cpp` with `vm2_collector_ready_include/map`. The native object, whole-unit comparison and raw-byte/exception audit are beside the source. `vm2_collector_ready.patch` contains the source and header delta; `vm2_collector_ready.py` reproduces the package and rejects raw-byte, exception-record or function regressions. The coordinator owns shared-header installation, normal-build checks and final source-link acceptance.

The coordinator installed this collector package and confirmed its normal native report at 100% with no previous exact-function regression. Checkpoint `1f6e0fc5` includes it after two byte-identical clean builds and frozen verification. NPpuVM itself remains NonMatching because Load and the complete unit's source-link acceptance are unfinished; the frozen build still uses its declared original fallback.

Further rejected Load experiments include duplicated stores across equivalent conditions, comma and conditional expressions, dead pointer assignments, full-width header bitfields and integer/union views of the pointer-member stores. None improves the three adjacent scheduling differences. The by-value retry-delay control that completed CRemoteMemory also does not directly change the collector's original receiver/node pair; the local erase scope is the material difference.

## Load continuation after collector integration

`vmload_probe.py` freezes the exact collector package and uses its private map overlay, then substitutes only the cached Load body. This avoids the older `vm2_probe.py` helper, whose imports reconstruct the prior partial collector. The configured compiler and code-generation flags are unchanged. Dependency generation is omitted in this private wrapper because `-MMD` writes NPpuVM.d in the working directory, colliding with another same-unit probe even when object paths differ.

Additional rejected source controls include integer/enum/pointer union identities, unused special members on header and destination-storage views, the enum receiver union together with those non-POD views, output-reference helpers under optimizer controls, allocation/size aggregate holders, overloaded placement initialization, intrinsic memory copies, malloc function-pointer signature views, exception-region boundaries, and alternate outer loop/switch/label forms. The CMsgRes pointer-union OR identity does not transfer directly: it retains an extra move in each allocation block, sometimes replacing loop-alignment padding. Identity rotate intrinsics likewise retain unwanted native instructions in some forms. None is installed in canonical Load.

Raw-byte region auditing is necessary even when the score is unchanged. The direct after-store mutable or volatile size reads still score 94.871796%, but their residual swaps are store/load rather than compare/store. The e300 scheduling candidate has both allocation blocks exact, but changes three epilogue instructions in addition to the original magic-block swap. The 604 model also fixes both allocation blocks while changing the magic block, a character-loop setup pair and the epilogue. `vmload_region_audit.py` groups native-size candidates by exact differing word offsets; its initial audit covered 891 objects and 24 distinct offset patterns. These candidates do not satisfy whole-function identity.

Static disassembly of the unmodified Wii/1.1 compiler establishes the actual pass-count pragma. `CodeGen_ParseSchedulingPargma` at 0x469BE0 calls the parser at 0x469BF0, which recognizes `once` and `twice` at literal addresses 0x6B03B4 and 0x6B03BC. They set scheduling factor one and two respectively. The generator tests factor two at 0x46C98F before pre-allocation scheduling and any nonzero factor at 0x46CB8A before final scheduling. Thus `#pragma scheduling once` disables the first pass while retaining final scheduling; `twice` and `on` select both. The earlier numeric `#pragma schedule_factor` sweep did not change this option and is not evidence about alternate pass counts. Native `once` probes regress Load's count coloring and keep the wrong allocation ordering, so this is a diagnostic finding, not a matching control for Load.

The static inspection uses private `vmload_compiler_disasm.py` and `vmload_compiler_symbol.py`, which only read the original compiler and its embedded CodeView public symbols. It changes neither compiler state nor emitted objects. The [mweppc PCode header](https://github.com/doldecomp/mweppc/blob/919828d97d33d236da675824d43c5d57332afe74/include/compiler/PCode.h) was consulted for the older compiler's flag names; this pass does not assume that revision's layouts equal Wii/1.1.

## Traps

- Reconstruct the actual vector size constructor when following the earlier property-data recipe. Specializing only its __vec_constructor backend changes inline parity and does not reproduce the recorded stack slots.
- A higher percentage can trade register differences for wrong stack offsets, and unchanged percentages can conceal added instructions. Keep native size, opcode/branch alignment, stack operands and register operands separate when selecting a candidate.
- The new void-pointer reassignment and aggregate-return controls are local compiler behaviors. Their successful use in another unit does not establish that they alter these surviving virtual registers.
- An unchanged percentage can move the remaining mismatch to another region. The local character-erase helper preserves 99.87143% while making the character sweep exact and moving nine register differences to the wide sweep. A concrete node type then completes that new context. Compare the locations and operands before discarding an equal-score candidate.
- A compiler option's internal or debug name need not be its pragma spelling. The scheduling pass-count field is exposed as `#pragma scheduling once` or `twice`; the accepted but ineffective numeric `schedule_factor` lines did not exercise it. Check the parser or an observed compiler-state change before treating a broad pragma sweep as evidence.
