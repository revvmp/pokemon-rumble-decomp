# CPiiPersonalData Import scheduling

2026-11-04: `Import` reaches 100% in the configured Wii/1.1 direct compile, retaining its exact 616-byte extent. All 44 real functions in the unit compare at 100%; the other functions are unchanged. The maintainer selected the unit as Matching, rebuilt successfully, and verified the complete DOL against its frozen SHA: PASS. All 44 functions are now source-linked.

Prior ordinary assignment permutations and 64-bit reconstruction spellings are recorded in sprint-a12 and sprint-a17. A new control matrix again found no exact ordinary local/snapshot order. Selective volatile source reads supplied the missing scheduling constraints: move the random-seed assignment before creation-time assignment, combine the high/low creation words directly, and qualify only the creation-time low word and original-trainer ID source reads volatile. No read or store is added or removed; every field value and assignment result remains the same. The qualifier is a D-0016 scheduling fake, not a recovered original save-data declaration.

The retained combination is mask `0x12` over flags-low, creation-low, creation-high, random-seed, original-trainer-ID reads. It produces the target's low-word hoist, flags update, seed store, and two creation-word stores with the exact registers. The earlier mask `0x01` reached 99.83766% but was not retained. Scratch source and complete objdiff are in ignored `tools/local/session/personal-61-compile/vol-win.cpp` and `vol-win.json`.

## Traps

- **An apparently unrelated later read can control an earlier store schedule.** Qualifying creation-low alone does not complete Import; original-trainer-ID must also be qualified. Compare the complete function rather than accepting a local schedule fragment. This is narrowly validated D-0016 evidence, not general advice to make save data volatile.
