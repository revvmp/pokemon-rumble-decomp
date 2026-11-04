# main.cpp final function

## F-225-1: effect emitter and manager counts are unsigned short

The final `main` mismatch was the equal constants passed to `InitializeEffect(500, 1000, 1000, 10000, 32)`: the target copies the second argument to the third argument register, while the prior unsigned-long declaration generated separate immediate loads. [sprint-a12](sprint-a12.md#fake-match-wave-d-0016) records the earlier attempts with locals, casts, wrappers and optimizer controls.

Testing the ABI argument types identified `unsigned short` for both the emitter count and particle-manager count as a matching declaration. The initializer's name was previously inferred, so its mangled spelling is not independent evidence for the old unsigned-long signature. Changing the declaration in `CEffect.h`, the `CEffect.cpp` definition and the `main.cpp` prototype produces exact code at both ends: `main` reaches 100%, and all 58 functions of `CEffect.cpp`, including the 1,476-byte initializer at `0x800A0150`, remain 100%. The target symbol is now `InitializeEffect__FUlUsUsUlUl`.

The call supplies 1,000 in both parameters, within the unsigned-short range, and all callers found under `src/` and `include/` use this one declaration/definition/call chain. No incompatible function-pointer cast is retained. An initial diagnostic cast established the hypothesis; it was replaced by the consistent source signature before acceptance. This is a supported signature correction rather than a D-0016 fake-match entry. The exact original typedef spelling remains unknown.

Validation used direct Wii/1.1 compilation with the flags generated for these two units in `build.ninja`, sjiswrap and objdiff-cli 3.8.2. Before regenerating dtk objects, the diagnostic target copies changed only the equal-length symbol spelling in their ELF string tables; originals and frozen manifests were untouched. The coordinating maintainer performs the normal split/build/full-DOL verification before marking either unit Matching.

## Traps

Anonymous-namespace symbols include the source basename. A scratch file for this unit must still be named `main.cpp`; naming it `282-main.cpp` changes the `CMain` mangling and introduces unrelated relocation differences. Use an isolated scratch directory with the original basename.

## F-225-2: source-link ordering remains open

After the signature correction, the normal coordinated build verified the full DOL SHA at main checkpoint `8cd9a16a`. This validates the function gain with the unit still NonMatching, not a complete source-linked main unit.

A direct emission-order investigation found that all 145 functions match individually, but only 86 form the same ordered subsequence in the compiled object and target. The first raw `.data` difference is at offset `0x370`: source starts font-map deleter RTTI, while the target starts layout-error callback RTTI. The target callback/deleter sequence is layout failure, file error, bound CMain callback, rumble guard, void callback, Home Menu callback, Home Menu guard, font map, archive font, resource font, pair font, and message resource.

Twelve reverse-order `sizeof` probes for those callback/deleter classes preserve 145/145 individual matches and improve the common function subsequence to 103/145. They are diagnostic only and were not retained in source: 42 function movements, other class-data ordering and a full source-link verification remain. Target vtable sizes may include padding, so compare the next symbol boundary rather than assuming an extra virtual slot.

An attempted local explicit specialization of the message-resource shared-pointer copy constructor before its first visible function use failed: a declaration without a parameter name gave an undefined `r` error in the generic template body, while naming it and defining the specialization after the use produced redefinition. A header hook before template instantiation would be needed for this approach. No shared header was changed.

The reusable scratch diagnostics are `tools/local/session/282-main/order.py`, `emission.py` and `lcs.py`. Scratch is not acceptance evidence; the ordering and required next steps are recorded here for the queued source-link work.
