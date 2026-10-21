# F-280-1: photo unit feasibility checkpoint

The original issue contains six functions and 12,504 code bytes at `8016CB20` through `8016FC08`. A fresh helper claim and isolated worktree start at main `5a5b2211`. No source selector, split, production source, shared header or symbol changes are accepted.

The first three functions reproduce all 108 instruction bytes under canonical Wii/1.1 game flags. The insertion callback sets the card-present and card-changed bytes at `804B90D8` and `804B90D9`; removal clears the former. The thread entry receives a 24-byte record containing a signed result at zero, the existing complete `PF_DEV_INF` at four and a signed drive byte at twenty. It stores the result of `FASetupfsi(drive, 6)`, then overwrites it with `FAGetdev(drive, &info)` and returns that result through the thread entry return register. Both direct native call targets and all three SDA references are resolved against the frozen DOL. These descriptive callback and record names are inferred.

The two synchronous API names and scalar arguments are factual reading evidence from the pinned THPConv FA public interface at commit `e7f2a17eeff43bc6b822a01af66ce670822ab77b`. Its restrictive source is not imported. The native wrappers at `8021DF00` and `8021DB00` call command producers at `8021D3A0` and `8021D1A0`; the latter writes the output pointer into command offset `1C`. The existing `PF_DEV_INF` record matches the actual photo caller's four output words.

The fourth function, `8016DF60`, is a two-argument wide formatter. Ordinary source `(NIntl::basic_formatter<wchar_t>(format) % directory % filename).str()` compiles to 1,204 bytes against the native 1,212. Its native constructor caches the first string's address in `r23` at `8016DF9C`, before `wcslen` at `8016DFA4`; the current canonical string constructor recomputes the address after the length call. This independently repeats the unresolved contract documented for the stage loaders and formatters. No allocator, register, flag or constructor permutation is attempted again here.

Raw controls are preserved outside the repository in `C:/MassStorage/GitHub/280-reference`: `280-photo-callbacks.cpp`, `280-photo-callbacks.o`, `280-audit-callbacks.py`, `280-callback-proof.json`, and `280-photo-formatter.cpp/.o`. The callback audit applies actual branch and SDA relocations before comparison. Its result is three exact functions, not source-selected acceptance. Both large save/dialog routines remain unimplemented, and the issue is incomplete.

## Traps

- An early typed formatter producer can expose the existing string lifetime/code-generation blocker before reconstructing two large filesystem routines. Preserve the positive callbacks and exact native retained-address site; do not repeat the prior allocator and constructor sweeps without new primary evidence.
