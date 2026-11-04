# CRemoteMemory remaining functions

## PrepareWrite exact instruction match

2026-11-04: direct compilation with the configured Wii/1.1 game flags makes `PrepareWrite` match 100%, up from 99.46565%, without lowering any other function score. Full source linkage remains unaccepted; the unit remains `NonMatching` with four other functions below 100%.

An explicit `int previous = (nextBlock - 1) & 1` before reading the stored generation fixes temporary register choices in the block-address calculation. This is a straightforward source reconstruction. The remaining six instruction differences were one inlined `WriteBlock` pointer allocated to r24 instead of target r23. A single inline `GetRemoteMemory(channel)` accessor around `sRemoteMemories[channel]` fixes that allocation. Its original existence is unproven: retain this helper under D-0016 as an inline-depth fake match. A second through seventh forwarding layer also matched but was discarded in favor of the smallest construct.

Rejected probes include alternate index arithmetic and signed/unsigned index types, all 24 declaration orders for ReadBack argument locals, ReadBack base-pointer helper and pointer arithmetic forms, SetState reference/value parameter variants, and composite Flag operator parameter/reference variants. None of these completed another function without regressions. No shared header was changed.

## Traps

- **A complete temporary-address fix can leave a separate inline allocation difference.** The explicit previous-block index raised PrepareWrite to 99.770996%, leaving only the pointer in the final WriteBlock expansion. Treat these as separate differences; changing the index further did not affect the latter. One forwarding layer for the pointer lookup resolved it without extra emitted instructions.

Further rejected FinishWrite probes: all delay/message/alarm declaration permutations, delay width/reference variants, a retry-alarm accessor, a remote lookup accessor, and destination/source/offset local permutations around memcpy. The best memcpy variant reached 98.45588% but still mismatched, so it was not retained. Register tracing confirms the base-pointer temporaries have interference degrees above the 29-register coloring threshold; the alarm-register discrepancy is therefore not fixed by merely renaming or moving its declaration.

A conditional scratch SetState expression distinguishes composite FOREIGN/INVALID state arguments from single-state arguments. Materializing `0x200` separately for composite states restores their missing `ori`, raising BeginRead to 98.676926% and FinishRead to 98.52134%, with Update unchanged at 100%. The remaining differences are mostly register allocation and one scheduled instruction; this artificial partial fix was not retained. Moving mask initialization inside the else branch or varying mask helper and local declaration placement did not complete it. Scratch evidence remains under `tools/local/session/remote-compile/conditional0.json`.

## ThreadMain exact instruction match

A later direct compiler comparison raises ThreadMain from 98.625% to 100%, making 17 of 20 target functions exact. Its two ReadBack expansions now match all 800 bytes, with every other function score unchanged. The accepted source change is a ReadRemoteBlock inline forwarder receiving the transfer offset as const u32&. The existing u16 member is promoted into the argument temporary, which changes register allocation without emitted stack traffic or extra instructions.

Passing all helper fields by value first reached 99.75%; the final promoted const-reference parameter fixes the r8/r6 exchange at both expansions. Integer/pointer casts and identity expressions on the data address, earlier local-value permutations, and helper definition placement did not reproduce it. This is an intentional D-0016 inline/parameter-shape fake, not recovery of a proven original interface. Evidence is ignored tools/local/session/remote-ppu/readtypes4.cpp and readtypes4.json. Full source linkage remains unaccepted, with BeginRead, FinishRead and FinishWrite still incomplete.

Further SetState probes added a source-local identity forwarder around the masked comparison, on top of the earlier conditional composite-bit materialization. This reaches 99.13846% for BeginRead and 98.643295% for FinishRead while preserving Update at 100%, but leaves a composite-result/register-coloring difference and BeginRead's two constant-load scheduling differences. It was not retained. Reference/value/width variants for the comparison, changed-state result, clear-mask operation and temporary lifetime did not complete either function. The best snapshot is ignored tools/local/session/remote-ppu/stateidentity0.cpp/.json.

FinishWrite memcpy forwarding, channel forwarding, pointer/integer arithmetic and reference-width variants did not complete its alarm or copy register allocation. The previously exact functions remain unchanged in main; only the verified ThreadMain helper is retained from this investigation.

## Fresh enum and lifetime pass

The 329-remote scratch pass reproduces the three remaining baseline scores: BeginRead 97.26923%, FinishRead 97.45731%, and FinishWrite 98.18627%. No implementation change was retained.

Applying the newer named-enum and assignment-identity techniques to the existing conditional SetState candidate did not complete either read function. Tested shapes include named Flag and independent enum values for the state-change bit, zero-enum construction, signed/unsigned private SetState parameters, reference parameters, explicit result locals and assignment identities, and scalar/one-member-structure views of the four volatile temporaries. Several shapes regress Update; none surpass the previous candidate while preserving the exact functions. The prior 99.13846% BeginRead candidate still coalesces the composite result into the input register, whereas the target keeps the literal in r0 and places the OR result in r5.

The first BeginRead call additionally orders its three immediate/address argument preparations differently. Named size/offset/callback permutations, const and enum argument locals, and inline forwarding with u16/u32 value or reference parameters did not fix it. Turning individual disabled optimization passes back on either leaves the result unchanged or regresses it.

FinishWrite still has two isolated differences: alarm/delay saved-register allocation and final memcpy address scheduling. A shared ScheduleRetry expansion, pointer-to-reference alarm rewrite, and const alarm/delay locals do not improve the baseline. Earlier findings already cover broader declaration and memcpy permutations. All new scripts and per-candidate comparisons are under tools/local/session/329-remote; no shared build or tracked implementation edits were made.
