# CGameSession emission and source-link pass

## Retained candidate

The UI lane supplied all219 reconstructed function bodies, a method-order candidate, and seven SaveData vector-deleter type seeds. This pass completes source emission ordering and the data layout. The retained unit is only src/game/CGameSession.cpp; the experimental tail fragment was removed after auditing incorrect inferred destructor names.

Shared changes are isolated by CGAMESESSION_EMISSION_LAYOUT in memory, vector, tree.h, SaveData.h, ISession.h, IGameSession.h, and StageDesc.h. Other units retain their previous definitions, except the proven removal of a spurious final pure-virtual declaration from IGameSession. Existing gates are preserved. These source-order probes and deferred template bodies are D-0016 emission-layout fake matches, not recovered original organization.

## Timing mechanisms

- The UI method order is retained, with sMaxMoney immediately before GetMoney. Create precedes GetInstance and GetGameSession.
- Named-false probes in SaveToSlot and LoadFromSlot request each eight-function vector helper cluster in its target wave; they emit no instructions.
- Early shared-pointer sizeof probes control the reverse specialization wave. Constructor/accessor specialization declarations follow those probes; declaring them first changes destructor order.
- ISession and IGameSession destructors, StageDesc construction/destruction, two shared-pointer default constructors, the MapDesc vector destructor, and CPlayer pointer get have separate declarations and equivalent definitions at the target helper wave.
- Seven vector-deleter and seven split-buffer type seeds precede UsedPassword in the gated SaveData header. A MapDesc split-buffer seed occurs before game includes. The pointer tree type is requested before vector to preserve its three helper functions after the seven vector deleters.
- Generic tree find_leaf and insert_key bodies are defined after the ordinary source methods. Deferring insert_key moves its true/false byte literals after class RTTI. A named-false insert_node_at request in Calc retains the original node-helper function wave.
- iosfwd precedes tree.h, making the basic_string primary template older than the tree primary. This places the tree length-error literal before the basic_string error literals without changing function order or instructions. Moving only insert_node_at or insert_one definitions did not move the relevant boolean literals: the pointer-set adapter uses insert_key.

## Identity corrections

Original vtable relocations disprove the four inferred shared_ptr_deleter destructor names. Their bodies are byte-identical, so function scoring concealed the error. The proven owners are CRemoteMemory at0x800BC430, CPlayer at0x800BC470, MapDesc at0x800BC4B0, and anonymous CGameSession at0x800BC4F0. The root corrected these symbols. Natural compiler emission now agrees with the original order; no strong destructor overrides are needed.

The original IGameSession base vtable is0xB0 bytes, whereas the reconstructed interface emitted0xB4. RegisterTeamPiiCollection is the final derived-only CGameSession virtual, not a pure virtual of IGameSession. Removing the base declaration preserves the derived vtable and all callers. It also corrects the four-byte name/RTTI padding discrepancy. Repository searches found only local CGameSession calls, and the derived class retains its declaration.

## Verification

The exact configured Wii/1.1 game flags and actual gated headers produce all219 functions in complete target order. Direct objdiff reports218 at100%; Calc's two remaining differences are literal relocation names. Normalized .data matches through all2574 meaningful bytes, with two trailing alignment bytes supplied by the link. The source emits additional unused weak metadata that must be discarded by the real linker. Full frozen-manifest acceptance remains the coordinating root agent's responsibility.

Evidence is in ignored tools/local/session/329-gamesession, including baseline.json, earlystring0.cpp, native-linked2.elf/.dol, and the adjacent329-gamesession-vtaudit.py. The initial linked candidate had exact text addresses and exception tables but incorrect data. After removal of experimental tail vtables, its only text-byte differences were the two boolean literal references and tree error-string address. The final source ordering changes address those data placements.

## Discarded tail experiment and traps

Before the vtable audit, a source tail supplied twelve strong, byte-exact C++ destructor definitions to override the supposedly misordered weak bodies. Full specialized classes also emitted strong vtables, relocating three vtables and growing data. CodeWarrior's class __declspec(novtable) suppressed those definitions while preserving all destructor bytes. This is a verified compiler facility, but the entire tail and its gates were removed because corrected symbol identities make it unnecessary.

Implicit destructor specialization declarations can compile without suppressing generated destructors; later explicit definitions then fail as redefinitions. sizeof/typedef probes, unused inline destructor-call helpers, and named-false construction/destruction blocks did not repair the falsely inferred ordering. A generic explicit destructor instead moves into the earlier get_deleter/dispose wave. Audit identical-function identity against vtables and cleanup relocations before introducing source-layout workarounds.

## Full-link acceptance

Root enabled the single CGameSession source as Matching, ran ninja successfully (exit0), and the frozen-manifest verifier returned PASS. The accepted result includes all219 functions with corrected identities, their full order, vtables and RTTI, literals, exception tables, and the complete executable SHA. No source tail or partial_sources configuration is retained. This supersedes the pending-link qualification above.
