# F-315-1: Reset registration returns an integer identifier

The Pad integration consumes the result of `RegisterResetCallback` at
`0x801B5C30`. The existing public name and argument types remain canonical.
The return is an integer registration identifier, rather than a pointer or
`void`: registration starts at one for an empty list, otherwise increments
the last callback record's integer identifier, copies that value into the
new record, and returns the same value. Pad stores the result at offset
`0xC858` in `PadData`. Sound ignores the result.

The #317 reconstruction independently reproduces the registration body's
final 604 bytes, including the returned identifier, and its complete 120-byte
exception record. Its remaining differences are in an earlier list cleanup
path and do not establish source completion. This header prerequisite adds
no matching source unit and completes no issue.

The landing gate runs after the official frozen verifier on every rebase.
It uses the exact current main header as a same-base control, compiles the
complete Ninja `all_source` inventory, links and verifies the full executable,
then restores the proposed header and repeats those entry points. It rejects
any whole-object, whole-ELF, or DOL difference and records the exact source
head, base, inventory, and hashes outside the repository. This includes the
actual `sound.o` consumer compiled with its unchanged source and flags.

## Traps

A fallback-linked sound unit can hide an incompatible replacement name for
its reset callback callee. Retain `RegisterResetCallback`, include its public
header in Pad, and store the returned integer ID. A passing final DOL alone
does not validate an unselected source consumer's contract.
