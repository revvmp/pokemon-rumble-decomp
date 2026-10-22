# Sprint ra: register allocator and instruction scheduler of mwcceppc Wii/1.1

This note recovers the back-end register allocator (graph colouring, coalescing, spill choice) and the list scheduler of the CodeWarrior Wii/1.1 compiler, and turns them into source-writing rules. The evidence is Hex-Rays output of a fresh idalib analysis of `tools/local/compilers/Wii/1.1/mwcceppc.exe`, compared with the GameCube back end in [doldecomp/mweppc](https://github.com/doldecomp/mweppc) (`Coloring.c`, `InterferenceGraph.c`, `RegisterInfo.c`, `Scheduler.c`). Frida traces of sandbox and game compiles confirmed every step. The tracer is now `tools/re/ra_trace.py`.

## Compiler functions

Addresses are for the shipped `mwcceppc.exe` (image base `0x400000`). Names with a leading underscore are present in the binary; the others are recovered from the GameCube source.

| Address | Name | Role |
| --- | --- | --- |
| `0x46BC70` | `CodeGen_Generator` | Per-function back-end pipeline |
| `0x46EEF0`, `0x46F110` | `allocate_local_vregs` | Gives variables their virtual registers |
| `0x5141C0` | `CFunc_AppendLocal` | Prepends a local to the `locals` list (`0x77B6DC`) |
| `0x458380` | `IsTempName` | Name starts with `@` or `$`, or is unnamed |
| `0x595690` | `assign_register_by_type` | Picks the class, then `assign_register_to_variable` (`0x5B6650`) takes the next number |
| `0x5B83F0` | `globallyoptimizepcode` | Machine-code optimiser (CSE, copy propagation, loop-invariant motion) |
| `0x5BB930` | `scheduleinstructions` | Scheduler driver |
| `0x5BBB10` | `scheduleblock` | Builds the dependence graph of one block and list-schedules it |
| `0x5BBCF0` | `selectinstruction` | Priority function |
| `0x5BBE90` | `findsuccessors` | Register, memory, volatile and side-effect dependences |
| `0x5BC5C0` | `addsuccessor` | Edge with latency, updates `height` |
| `0x5BC1D0` | same-base disambiguation | New in Wii: two accesses through the same base register with disjoint offsets do not depend |
| `0x5B8E20` | `colorinstructions` | Colours each class: SPR, CR, VR, FPR, GPR, retries after spilling (at most 10 rounds) |
| `0x63DD60` | `buildinterferencegraph` | Repeats build and coalesce until no move is removed |
| `0x63EA10` | node initialisation | Allocates the 32-byte nodes |
| `0x63E5F0` | interference build | Liveness walk; call operands force non-volatile |
| `0x63E2A0` | `coalescenodes` | Move coalescing |
| `0x594AF0` | `machinespecific_coalesce_restriction` | Which moves may coalesce |
| `0x63DF70` | `markspecialregisters` | Binds nodes to their variables |
| `0x63DE20` | `findrematerializations` | Single-definition, location-independent values |
| `0x63EE50`, `0x63EDC0` | `makeinterfere`, `interferes` | Edge lists hashed into 0x4000 buckets |
| `0x5B92F0` | `simplifygraph` | Builds the colouring stack |
| `0x63F9E0` | `estimatespillcosts` | Use and definition counts weighted by loop depth |
| `0x5B9120` | `colorgraph` | Pops the stack and picks colours |
| `0x5930A0` | `pickcolor` | Lowest free register; VLE masks only |
| `0x592FB0` | `machine_specific_sort_graph` | Reorders the stack only for VLE |
| `0x5B6380` | `obtain_nonvolatile_register` | Hands out r31, r30, ... (f31, f30, ...) |
| `0x5B6520`, `0x5B6570` | `volatile_registers`, `available_registers` | Initial pool and the simplify bound K |
| `0x5B8FC0` | `rewritepcode` | Applies colours, deletes self-moves |
| `0x63F010` | `insertspillcode` | Spill code |
| `0x63BEC0` | `IRO_SplitLifetimes` | IR-level live-range splitting (not analysed here) |
| `0x704CC6` | `machine750` | Scheduler model used by `-proc gekko` (issue width 2) |
| `0x6F1B9A` | `opcodeinfo` | 22-byte entries; name at `+0`, scheduling rank at `+9` |

Data used by the tracer:

| Address or offset | Field |
| --- | --- |
| `0x77647C` | `interferencegraph` (array of 32-byte nodes) |
| `0x797480` | `coloring_class` (0 SPR, 1 CR, 2 VR, 3 FPR, 4 GPR) |
| `0x77520C` | `pcbasicblocks`; block `+20` first instruction, `+28` index |
| node `+0` | stack link |
| node `+4` | variable object (its name is the hash node at object `+12`) |
| node `+12` | spill cost |
| node `+16`, `+18` | virtual register, colour |
| node `+20` | flags: `0x1` spilled, `0x2` pushed, `0x4` coalesced, `0x800` named by a call |
| node `+22`, `+24` | working degree, edge count |
| instruction `+12`, `+40`, `+42`, `+44` | flags, opcode, operand count, 14-byte operands (kind, class, effect, register) |

## Pipeline at `-O4,p`

`CodeGen_Generator` runs, for each function:

1. The IR optimiser, then instruction selection. Instruction selection numbers the virtual registers (below).
2. `globallyoptimizepcode`: peephole, CSE, copy propagation, loop-invariant motion.
3. `scheduleinstructions(0)`: the game flags give schedule factor 2, so blocks are scheduled once before allocation. Prologue and epilogue blocks are skipped.
4. Forward peephole, then `colorinstructions`.
5. Post-allocation CSE, prologue and epilogue, peephole, and `scheduleinstructions(1)` over every block.

A trace shows `schedule_factor = 2` and `scheduling = 4` (the 750 model).

## Virtual-register numbering

Registers 0 to 31 of each class are the physical registers. Virtual registers start at 32 and are numbered in this order:

1. Parameters in declaration order (`this` first).
2. Named locals in `locals` list order. `CFunc_AppendLocal` and the declaration code prepend, so the list is in reverse creation order: the last declared local gets the lowest number and the first declared the highest.
3. Locals with temporary names (`@n`, `$n` or no name): inline-function parameters and return variables and compiler temporaries, again in reverse creation order. An inline expanded earlier gets higher numbers than one expanded later.
4. Instruction-selection temporaries, in generation order. A binary operator evaluates its left operand first (the right first if only the right contains a call) and allocates the result register after both operands. A call evaluates nested calls in its arguments from the last such argument to the first, loads the register arguments left to right, and copies the result into a new register after the call.
5. Registers created later by the machine-code optimiser, for example values that loop-invariant motion hoists, get numbers above all of these.

The IR optimiser often deletes a named local or an inline return variable and keeps the value in an expression temporary. Whether a variable survives therefore decides whether its value ranks with the variables (item 2 or 3) or with the temporaries (item 4). The tracer prints the variable name for surviving nodes and `-` for temporaries.

## Interference and coalescing

The graph has a node per virtual register and one per physical register; the 32 physical nodes interfere with each other. A definition interferes with everything live after it, except the source of a move. A load or store base register interferes with r0. A call defines every volatile register, so a value live across a call interferes with r0 and r3 to r12 (f0 to f13). A register named as an operand of a call instruction (the exception-cleanup liveness operand) is marked `0x800` and made to interfere with every volatile register.

`coalescenodes` walks every move and merges source and destination when they do not interfere. The merged node takes the lower number. It refuses when:

- either node holds a variable that is not a local (object `datatype` other than `DLOCAL`);
- both are virtual registers and either holds a named variable (name not starting with `@`), so named locals and parameters merge only with physical registers;
- either node is marked `0x800` and holds a variable smaller than 4 bytes;
- the higher number was named by a call (the set built during the interference walk);
- the higher number is the caller-SP register.

Build and coalesce repeat until no move is removed.

The GameCube back end coalesced two virtual registers only when both were front-end temporaries, and never iterated.

## Simplify

K is the number of free physical registers of the class: 29 for GPRs (r1, r2 and r13 are reserved), 28 when r31 is the frame pointer, and 32 for FPRs. `simplifygraph` repeats passes until a pass pushes nothing:

- Each pass scans the virtual registers in increasing number, skipping pushed and coalesced nodes.
- A node whose degree is below K is pushed onto the stack and its neighbours' degrees drop by one (two for register pairs). The drop applies at once, so a node later in the same pass can become pushable.
- Other nodes wait for the next pass.

When a pass leaves only nodes of degree K or more, `estimatespillcosts` runs and one node is pushed optimistically. It is the node with the highest score, where the score is `FLT_MAX` for flag `0x80`, `FLT_MAX/2` for flag `0x400` and otherwise spill cost divided by degree; ties go to the lower number. Then passes resume.

## Select

`colorgraph` pops from the top, so the last node pushed is coloured first. In the common case where every degree is below K, this is plain descending virtual-register order. Nodes that waited for later passes are coloured before all nodes of earlier passes.

For each node the pool is the free volatile registers plus every non-volatile register opened so far. Neighbours' colours are removed and `pickcolor` takes the lowest-numbered register left: r0, r3, r4 ... r12, then the lowest open non-volatile. If nothing is left, `obtain_nonvolatile_register` opens the next one (r31, then r30, ...) and the node is retried. A node that cannot be coloured is spilled, spill code is inserted and the whole class is coloured again.

Two consequences:

- The first node that needs a non-volatile register gets r31, the next r30, and so on. A later node that fits into an already open non-volatile register takes the lowest such one, not r31.
- Among values that do not cross calls, the one coloured first gets the lowest register. With several simultaneously live values, the highest-numbered gets r0 (unless it is used as a base address) or r3, and the earliest-created gets the highest register.

`pickcolor` and `machine_specific_sort_graph` change the choice only for VLE targets. `rewritepcode` deletes moves whose two registers got the same colour.

## Scheduler

The scheduler is the GameCube list scheduler with one addition (`0x5BC1D0`, same-base disambiguation of memory accesses).

- The dependence graph of a block is built from the last instruction to the first. Register true, anti and output dependences, memory dependences filtered by alias analysis, volatile accesses and side effects all make edges.
- A call, an instruction with side effects or one the machine model serialises is ordered after every later instruction.
- An instruction with no successor gets the block's branch as successor.
- `height` is the longest latency path to the end of the block, and `latest = critical path - height`.
- `selectinstruction` takes the first ready instruction in the current list order. A later ready candidate replaces it if:
  1. the candidate has reached its deadline (`latest <= cycle`) and the current one has not; else
  2. the candidate uncovers more successors (successors whose only remaining predecessor it is); else
  3. at equal uncovering, the candidate has the greater height; else
  4. in the pre-allocation pass only, the candidate has the lower opcode rank (`opcodeinfo` `+9`).

  Full ties keep the earlier instruction. The list order is generation order in the pre-allocation pass and the pre-allocation schedule in the final pass.
- The 750 model issues two instructions per cycle, subject to its unit table at `0x75A7B0`.

Because the pre-allocation schedule is what the allocator sees, a different generation order can move a definition past a use and change the colours as well as the order.

## Rules

Each rule was checked in sandbox compiles with the game flags. The sandbox files are in the session scratchpad; the snippets are reproduced here.

1. **Named locals take non-volatile registers in declaration order, ahead of parameters.** `int x = g(1); int y = g(2); int z = g(3); f(x); f(y); f(z);` gives x r31, y r30, z r29, also when the assignments come in another order (`t3`). Parameters are numbered below all locals, so `this` gets the lowest non-volatile register in use when it conflicts with everything (`C::B`: the IR induction pointer r31, `i` r30, `n` r29, `this` r28).
2. **Temporaries and inline variables rank above named locals.** Inline return variables and parameters (`@n`) and compiler temporaries are numbered after the named locals and are coloured first. Among inline variables, the inline expanded first is coloured first (`t5`: the first `helper` call's value r31).
3. **Expression temporaries rank above everything and in reverse generation order.** A value held only in an instruction-selection temporary, such as a call result whose variable the IR optimiser deleted, is coloured before all variables, and the last one created is coloured first. `PopInt()` results stored into a bitfield struct stay in temporaries, so the later pop is coloured first (sandbox `inl2`: second pop r31, first r30). When the same pops feed separate calls, the inline return variables survive and the first pop gets r31 (`inl3`).
4. **A value that must be coloured after a named local has to be a named local declared after it.** BWorld `FindObjectsInSector`: `(outerRadius + radius) * (outerRadius + radius)` and `inner * inner` are temporaries coloured before `radius`, which then cannot take f0. `f32 outer = outerRadius + radius; outer *= outer;` and `inner *= inner;` keep each square in the local's node, coloured after `radius`, and match.
5. **High-degree nodes are coloured first.** A node with degree K or more (29 GPRs, 28 with a frame pointer, 32 FPRs) waits for a later simplify pass and is coloured before every node of earlier passes. Long-lived parameters in large functions reach this. In ScriptWorld `SetStageDescTarget`, `pEngine` (degree 41) is coloured first and gets r31; in `FindObjectsInSector` the four float parameters (degree 36 to 146) take f31 down to f28 in reverse parameter order. A parameter one interference short of K is coloured last instead and gets the lowest non-volatile register: in homebutton `InitializeHomeButtonInfo`, `pInfo` and `pInfoEx` had degree 28 in our build. The cause was not a register rule but two source differences (an `|=` and a direct `WaitUntilLoaded` call) that removed live values; with them the function matches.
6. **Normalise registers before chasing a register swap.** `python tools/re/regscan.py --min 95` compares every near-miss function with register numbers masked and sorts them by the remaining differences. What remains is a difference in instructions or their order, which comes from the source, and fixing it often fixes the registers too (rule 5).
7. **Named variables never coalesce with other virtual registers; temporaries and `@` variables do, into the lower number.** A chain of copies through inline return values therefore collapses into its oldest member and ranks where that member ranks.
8. **The lowest open register wins.** Once r31 and r30 are open, a later node that is free of both takes r30 (sandbox `a.cpp`: `b` reuses `y`'s r30).
9. **Instruction order ties keep generation order.** Two ready instructions with equal uncovering and height are emitted in the order instruction selection produced them. Register arguments are loaded left to right, so a target that loads a later argument register first came from different source structure, not from the scheduler.

## Fixes

- BWorld `FindObjectsInSector` (99.86% to 100%, commit `0f5081f9e`): rule 4.
- homebutton `InitializeHomeButtonInfo` (98.73% to 100%, commit `33c676572`): rule 6. `pInfo->messageFlag |= HBMMSG_NOSAVE_ALL;` and `CFileData::WaitUntilLoaded(pData->mModel.operator->());`.
- BWorld `FindObjectsInRing` (99.37% to 100%, commit `c3616dbbf`): rule 4, as in `FindObjectsInSector`.
- CMsgRes `ConvertEscape` (94.66% to 95.57%, commit `91578f171`): rule 2. Declaring `isSjis` after `isTrailByte` gives it the lower number, and `isTrailByte = false;` before the terminator store restores the target's block structure. On its own the store lowered the score to 93.32%; only the declaration move made it an improvement. Register differences remain.

## Diagnosed, not fixed

- **ScriptWorld `SetStageDescTarget`, ScriptGame `MakeMapID`.** The target colours the first `PopInt` value first, so in the original the values survive as inline variables. In our bitfield `MapIndex` construction they are temporaries (rule 3). Aggregate initialisation, member-wise assignment, `u8`/`s8` locals, an inline `MakeTarget` helper and explicit `Pop`/`ToInt` all keep temporaries. In `MakeMapID` our values also reach degree K, while the target's do not.
- **CPiiListFilterSort `ShowTypeFilterMenu`.** The hoisted constant `1` (r193) and the `kind` load (r194) are temporaries; the load is coloured first and takes r3. The target needs the constant coloured first, so `kind` must be a variable. Moving the shift into the loop condition (`(1 << kind) & mask`) colours the constant first, but `kind` then takes r0.
- **CDlgInfo `CDlgInfoItemLine::Update`, BWorld `AddObject`.** The swapped nodes are high-degree nodes released in different simplify passes (rule 5). Declaration order does not reach them.
- **CModel `TPartialAnm<1>::AppendMotion`.** The `||` result is the inline return variable `@46163` (r42) and the `&&` result a temporary (r44) generated inside it, so the `&&` is coloured first. The target colours the `||` result first. Helpers for the `&&` part are folded away by the IR optimiser and do not change the numbering.
- **CShadowCamera `SetupShadowMaterial`.** The target loads the `GXSetTevKColor` colour argument (r5) before `this` (r3), and the `postMtx` subtraction gets r5 because `addi r3` was scheduled before its use. Both point to a different generation order, not to the scheduler's priority function (rule 9).

## Tools

`python tools/re/ra_trace.py <source> [-f Function] [-d 1|2|3] [-i dir]` compiles one file under Frida with the game flags and prints, for every coloured class, the nodes in colouring order with their virtual register, colour, variable name and degree, and K. `-d 1` adds the code entering allocation, `-d 2` the code before the first schedule, `-d 3` the code before the final schedule. It needs `pip install frida`. Compare the colouring order with the target's registers to see which node has to move, then use the numbering rules to choose a source change.

`python tools/re/regscan.py [--min 95] [--unit name]` lists near-miss functions with the count of differences left after masking register numbers (source differences) and the raw count of differing instructions. Functions with zero source differences are pure allocation or scheduling problems.

## Traps

- The trace is per function name without the class, so `-f Update` reports every `Update` in the unit.
- A unit compiled from a copy elsewhere renames its anonymous namespace (`@unnamed@<file>_cpp@`); keep the original file name when compiling variants.
- `IRO_SplitLifetimes` and IR copy propagation decide which variables survive; their rules are not recovered here.
