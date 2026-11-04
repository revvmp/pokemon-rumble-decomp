# CPii snapshot-vector swap tail

## Current investigation

The retained source baseline has one remaining owned function: std::swap<vector<SD_PiiPersonalData::Data>>, 3052 bytes, at 99.16514 percent. Direct isolated compilation also reports a shared bad_function_call::what literal mismatch, which is not the unresolved owned function in the global report. No CPii changes have been applied.

An isolated specialization of the true __vec_constructor copy constructor raises swap to 99.48886 percent. As in CDlgInfo, the copy uses a two-pointer Range view, one outer copy helper, and independent allocation/construction helpers (one inline level each). This reproduces the complete copy-construction prefix, including its two allocation-tag slots and all registers. The remaining assignment blocks have only nonvolatile register differences, with identical instructions and stack locations. The target first assignment keeps the end pointer in r24 and cached capacity in r27; the candidate assigns those r28 and r24.

Evidence is tools/local/session/pii-owned-tail/ctor1111.cpp, .o, and .json. This is a scratch gain only, not an accepted function or link.

## Traps and rejected probes

Assignment first/last declaration order, Range aliases, integer/enum pointer views, mutable pointer views, and separate forwarding depths did not resolve the remaining allocation cycles. Flattening assign into operator= flips many stack slots and regresses. A Range passed by value to an assignment helper creates unwanted code; by reference also regresses. The named-growth recommendation technique from PiiPropData leaves this candidate unchanged. Enum capacity views likewise leave it unchanged. Explicit swap variants with and without std::move all produce the same respective baseline or candidate. Explicit vector assignment definitions at original depth reproduce the baseline; removing two forwarding levels preserves its remaining register cycle. A single-pointer Range containing only the copy end pointer retains the same gain; putting only the first pointer into a Range loses the gain. No changes to shared headers have been retained for these experiments.

Additional pointer output-reference helpers, enum end-address views inside the assignment core, incrementally formed end pointers, and recommendation helpers did not improve the remaining cycle. The scratch header was restored to the current main header after access/declaration experiments. CPii remains unchanged.
