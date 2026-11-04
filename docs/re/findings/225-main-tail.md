# main.cpp final function

## F-225-1: effect emitter and manager counts are unsigned short

The final `main` mismatch was the equal constants passed to `InitializeEffect(500, 1000, 1000, 10000, 32)`: the target copies the second argument to the third argument register, while the prior unsigned-long declaration generated separate immediate loads. [sprint-a12](sprint-a12.md#fake-match-wave-d-0016) records the earlier attempts with locals, casts, wrappers and optimizer controls.

Testing the ABI argument types identified `unsigned short` for both the emitter count and particle-manager count as a matching declaration. The initializer's name was previously inferred, so its mangled spelling is not independent evidence for the old unsigned-long signature. Changing the declaration in `CEffect.h`, the `CEffect.cpp` definition and the `main.cpp` prototype produces exact code at both ends: `main` reaches 100%, and all 58 functions of `CEffect.cpp`, including the 1,476-byte initializer at `0x800A0150`, remain 100%. The target symbol is now `InitializeEffect__FUlUsUsUlUl`.

The call supplies 1,000 in both parameters, within the unsigned-short range, and all callers found under `src/` and `include/` use this one declaration/definition/call chain. No incompatible function-pointer cast is retained. An initial diagnostic cast established the hypothesis; it was replaced by the consistent source signature before acceptance. This is a supported signature correction rather than a D-0016 fake-match entry. The exact original typedef spelling remains unknown.

Validation used direct Wii/1.1 compilation with the flags generated for these two units in `build.ninja`, sjiswrap and objdiff-cli 3.8.2. Before regenerating dtk objects, the diagnostic target copies changed only the equal-length symbol spelling in their ELF string tables; originals and frozen manifests were untouched. The coordinating maintainer performs the normal split/build/full-DOL verification before marking either unit Matching.

## Traps

Anonymous-namespace symbols include the source basename. A scratch file for this unit must still be named `main.cpp`; naming it `282-main.cpp` changes the `CMain` mangling and introduces unrelated relocation differences. Use an isolated scratch directory with the original basename.
