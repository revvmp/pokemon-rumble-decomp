# F-196-1: CPpuEngine callback snapshot matching

Target: `wpse-v1`, `CPpuEngine::SendMessage` at `0x80101CA0`, size `0x47C`. The baseline compiled 54 of 55 functions in `game/CPpuEngine.cpp` identically; `SendMessage` scored 82.94%. The earlier honest reconstructions and the evidence for the local vector-derived `HandlerList` are in [sprint-a03](sprint-a03.md#tail-wave-clayout-cfiledata-nppuvm-cpiitex-cppuengine) and [sprint-a12](sprint-a12.md#final-tail-wave-a12-third-shift-cremotememory-cppuengine-and-others).

## Result

All 55 target functions score 100% in objdiff-cli 3.8.2 after a fresh Wii/1.1 compile using the generated build command. The coordinating agent then marked the unit Matching and ran the complete Ninja link, frozen SHA comparison, and verification entry point successfully. The source unit is accepted; all 55 functions retain exact target order and the rebuilt executable retains its frozen hash. No executable bytes, assembly, compiler binaries, reference files, or frozen hashes were changed.

The target snapshot path retains eight-byte iterator objects comprising a list-node pointer and an otherwise unused byte. It also retains two levels of iterator copies in both the distance loop and the construction loop. A local identity-adaptor model reproduces these accesses. The target does not establish the original adaptor's source name or why those stores survived optimization, so the retained solution is explicitly a D-0016 fake match.

## Retained fake constructs

- `HandlerIterator` carries an empty identity policy and accesses the underlying node pointer through a volatile storage reference when incrementing. The comparison remains the ordinary list-iterator comparison.
- Two empty policy objects are declared at the call site and their object-representation bytes are cleared inside `HandlerList` after vector initialization. This preserves the target's stack slots and store order.
- Translation-unit-local specializations of `vector<int>::assign` and the two `__vec_constructor<int>::construct_at_end` forms reproduce the range construction and iterator-copy levels. This adaptor is private to this source file and is only used to initialize the empty callback snapshot.
- `AllocateHandlers` forwards through `__vec_deleter<int>::allocate`, placing the allocation and exception temporaries at the required inline depth.
- The construction loop declares a volatile zero byte before binding `const int& value = *first`. This order produces the target's value spill and gives the value register `r0` and the zero register `r3`.
- `opt_loop_invariants off` is limited to `SendMessage`; it preserves the target's loop-entry compares and node reloads.

## Experiments and validation

All experiments used uniquely named scratch source and object paths under ignored `tools/local/session/196-experiment/`, with the original source basename preserved for unnamed-namespace symbols. The compile command came from the configured Ninja target and retained the project's compiler and flags. Comparison used `objdiff-cli diff -1 build/wpse-v1/obj/game/CPpuEngine.o -2 <scratch-object> -o - --format json`.

The plain identity adaptor improved the function to 87.29%. Explicit node storage accesses and range construction reached 95.60%; restoring the missing construction forwarding level reached 97.12%. Correct inline depth and policy placement removed the register and stack-layout differences. The final candidate and its formatting-only cleanup both scored 100%, with all 55 target functions still at 100%.

No shared header changes were retained. A scratch-only `initialize_range` member helped diagnose inline depth but was unnecessary in the final source. Broad optimizer pragmas, a volatile node member, and an extra volatile integer spill either regressed the function or added loads/stores absent from the target.

## Emission and RTTI follow-up

The first function-perfect candidate still emitted several weak functions in the wrong order. A local inline specialization of the callback map's subscript operator calls `tree_.find_or_insert` directly, reproducing the four helpers immediately after `InsertCallback` and the later `insert_node_at` position. Including `alloc_ptr.h` and `vector` before the game headers puts the vector deleter and allocator destructors in their target order. A `sizeof(shared_ptr<NPpu::CCode>)` declaration before the game headers establishes specialization order, and a constant-false destructor probe in `GetScriptRoot` requests the string shared-pointer destructor before static initialization. All 55 target function names then occur in exact target order, and all 55 remain 100%. These include-order and probe constructs are also disclosed fake matches.

The original RTTI at `.sdata` `0x804B7150`, `0x804B7158`, and `0x804B7160` was named from its referenced type strings and constructor base layout: `__RTTI__10CPpuEngine`, `__RTTI__Q33std3tr137enable_shared_from_this<10CPpuEngine>`, and `__RTTI__Q24NPpu7CEngine`. The base list points to the enable-shared-from-this base at offset `0x68`, `ICalc` at `0x64`, and `NPpu::CEngine` at zero. Only these names changed; addresses, extents, original data, and expected digests were preserved.

## Traps

- A matching range-construction body can still have the wrong temporary order. Here a volatile zero-byte declaration after the dereference bound the value to `r3`; placing it before the reference binding reproduced `r0` and the required store ordering. This is verified for this function and is not a general rule for all iterator loops.
- Keep copied iterator policies and iterator node volatility separate. Making the entire node member volatile caused different copy code and comparison materialization; the retained fake limits volatility to the increment access.

## Link acceptance

The source `.sbss` section occupies `0x19` bytes with alignment eight, and the following `CRemoteMemory` source section also aligns to eight. The source `.sdata2` occupies four bytes with alignment eight, as does the following target section. The original inferred extents included this padding; no data sizes, split boundaries, or hashes needed adjustment. Three RTTI symbol renames were sufficient for the full source link.
