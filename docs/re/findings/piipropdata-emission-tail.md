# PiiPropData source emission and complete acceptance

The normal production build accepted PiiPropData on 2026-11-04. The coordinator ran configure and ninja successfully, then passed the frozen `wpse-v1` executable manifest. All 48 real PiiPropData functions and all 26 retained exception records match. No previously exact function regressed or disappeared. The complete executable is 4,094,880 bytes with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

This acceptance combines the recovered copy/initialization boundaries in [the primary MSL findings](piipropdata-primary-msl-20261104.md), the constructor/Reset work in [the allocation-tag findings](piipropdata-allocation-tag-tail.md), and the emission controls described here. The ordinary throw expression remains in the allocation code. The rejected direct Runtime throw diagnostics are documented separately in [the receiver findings](20261104-piipropdata-receiver-tail.md).

The accepted implementation is `src/game/PiiPropData.cpp`, `src/game/PiiPropDataAccessors.cpp`, and `src/game/PiiPropDataIterators.cpp`, with scoped declarations in `include/game/PiiPropData.h` and `include/vector`. All three are genuine native compiler inputs. The final linker receives their original compiler outputs in main/accessors/iterators order and selects their native code sections in the configured order. The partial-link analysis object is not substituted for those raw inputs in the executable. No original instructions, object contents, relocations, exception records, or hashes were copied or rewritten.

## Original layout and acceptance boundary

The frozen original object is `tools/local/tail-20261104/vmprop-targets/PiiPropData.o`, SHA-256 `7bc6619c8e22e4d2f520d96a8e0ad6e8f5f81d0e54a46ead4c4f3da80af80eda`. Its retained ranges are:

| Section | Original range | Retained size |
| --- | --- | --- |
| `.text` | `0x800F2580`–`0x800F60B4` | `0x3B34` bytes, including original inter-function alignment |
| `extab` | `0x80025684`–`0x80025BAC` | 1,320 bytes |
| `extabindex` | `0x8004A694`–`0x8004A7CC` | 312 bytes |
| `.data` | `0x803A91C8`–`0x803A9213` | 75 bytes |
| `.sdata` | `0x804B70E8`–`0x804B70F8` | 16 bytes |

The data contains the 20-byte `vector length error` string and the 55-byte serialized exception hierarchy. The small-data range contains three four-byte sort shuffle values, each `0xFFFFFFFC`, followed by four bytes of alignment. An inferred eight-byte extent on the final original shuffle label includes that alignment; it is not an eight-byte source variable.

The first isolated complete acceptance is `tools/local/tail-20261104/links/pf-piiprop-exact/verification.json`. Its native link and ELF-to-DOL conversion both exited zero, followed by frozen-manifest `PASS` and zero differing bytes. The cleaned files were then compiled under their final production basenames. Every nonempty allocated section, including bytes, normalized relocations, flags, and alignment, matched the corresponding first-passing object. `game_PiiPropData/pf_pii_production/allocated-equivalence.json` records 14 main sections, six accessor sections, and two iterator-copy sections, all equivalent.

The final-basename trial is `links/pf-piiprop-production`: native link zero, conversion zero, the same frozen SHA, exact size, and zero differences. The coordinator subsequently installed the packet and ran the normal configure/ninja/verification workflow successfully. That build reported all 13,462 real CPU functions exact, all 3,078,460 CPU code bytes matched and source-linked, and all 902 raw-link aliases identical to their selected inputs. The remaining project source fallback at this checkpoint is the separate 8,192-byte AX DSP payload; it is not PiiPropData code or data.

## Why function comparison was insufficient

The initial near-complete body candidate had 46 exact functions and only constructor/Reset register differences, but its source emission placed SpeciesMoves construction and destruction helpers immediately after the constructor. Their target positions follow the six sort functions. GetProperty also needed to precede CompareLevelMoves, and two iterator inequality requests needed to occur earlier.

Moving GetProperty and adding literal-false typed iterator requests corrected the ordinary ordering around those helpers without changing any caller instructions or exception records. The remaining natural sequence still placed five implicit functions too early: SpeciesMoves constructor, LevelMove-vector destructor, short-vector constructor, short-vector destructor, and SpeciesMoves destructor. Four of these have exception records.

A six-section candidate placed every code body correctly and preserved all retained data. The native `pf-piiprop-section-diagnostic` trial nevertheless differed in 343 bytes: 32 known constructor/Reset instruction bytes, four known saved-register bytes, and exception placement. Explicit sort instantiations fixed code sections while moving five sort exception records early. Per-function code and per-record equality did not establish the final layout.

Qualified friend sort definitions plus discarded typed calls could restore the sort exception sequence, but their tested section attributes were ignored. Generic header section attributes, section pragmas around SetWazaLevels, and section pragmas around the original algorithm definitions did not supply a complete solution. These friend and copied-sort bodies are not retained.

## Keep allocation code in one compiler input

A four-input prototype gave every function its original address and reproduced the complete exception-index table. Separate strong C++ definitions owned the five early helpers and three backend destructors. The allocation-bearing main and late-helper inputs, however, each retained their own 20-byte error literal and 55-byte exception-type string. The map showed both pairs at `0x803A91C8` and `0x803A9218`. The resulting DOL grew by 64 bytes after final alignment, and the frozen comparison failed. This is `links/pf-piiprop-group-diagnostic`.

The failure is the same class of problem established for CModel: exact bodies, exact per-function unwind records, and correct code addresses can coexist with duplicated local pooled strings. The final PiiPropData implementation keeps all allocation and throw code in the main compiler input. Its two companion files contain only pointer/value accessors and iterator copy constructors, with no exception records or retained data.

## Delayed assignment and stack-slot parity

Suppressing the early push-back bodies was straightforward with scoped declarations and later exact definitions. SpeciesMoves assignment was harder because the original implicit assignment was requested while parsing SetWazaLevels. Replacing it with a handwritten pointer loop, an indexed loop, or `std::copy` changed its body and exception details. A separate member-storage view also changed the generated body. These controls were rejected.

A forward-declared typed function template allows SetWazaLevels to make the same two-argument call without immediately requesting the implicit assignment body. Its late definition assigns the same SpeciesMoves object through a second inline helper. The emitted entry is:

`AssignPropertyMoves<PiiPropData::SpeciesMoves>` → `AssignPropertyMoveMembers` → the ordinary implicit SpeciesMoves assignment.

The second inline level is necessary. With one level, the function still had the original 2,044-byte extent and register assignments, but the compiler reversed temporary stack-slot groups and changed ten exception-record bytes. Two levels restore every instruction byte and the entire 152-byte exception record. Repeating the experiment with additional helper levels confirmed the existing parity rule: two, four, and six total levels matched; the intervening parity did not. The accepted source uses the smallest matching chain.

An explicit request after the late SpeciesMoves and push-back definitions places this assignment before GetWazaLevels. The main input then emits all 26 exception records in the original sequence. A scoped, explicit vector assignment body performs exactly the same base assignment and return as the implicit body, allowing its native code section to be selected without changing any function or unwind record.

The one synthetic identity mapping is:

| Address and size | Prior inferred identity | Accepted D-0016 emission identity |
| --- | --- | --- |
| `0x800F5000`, `0x7FC` | `PiiPropData::SpeciesMoves::operator=(const SpeciesMoves&)` | `AssignPropertyMoves<PiiPropData::SpeciesMoves>(SpeciesMoves&, const SpeciesMoves&)` |

The full mangled mapping is saved in `pf_pii_production/symbol-map.json` and applied by the coordinator. The prior SpeciesMoves assignment spelling has no entry in `docs/re/reference-symbols.tsv`; it is a reconstructed identity, not an established original spelling. The replacement does not claim to recover a previously known template name.

The observed ABI is unchanged: destination reference in `r3`, source reference in `r4`, and destination-reference result in `r3`. The 2,044-byte function, its relocations, and its exception record match after the identity mapping. The full passing native link proves that no second old-name body, unresolved caller, or changed retained reference remains in this executable.

## Final scoped emission controls

The main input retains all original body visibility needed by inlining and exception analysis. Its scoped header view declares SpeciesMoves construction/destruction and the relevant vector destruction/push-back wrappers, then supplies their exact bodies at the required source phase. The short-vector constructor and concrete destructor declarations select native sections independently of their exception-record position.

The accessor fragment supplies 15 strong, trivial method definitions. The iterator fragment supplies the two strong const-iterator copy constructors. Their native definitions replace the main input's weak copies while preserving the main compiler's implicit-copy analysis. Giving the main class explicit iterator copy constructors instead changed return ABI and caller instructions; the isolated declaration view avoids that regression.

The final header scopes are `GAME_PIIPROPDATA_COPY_LAYOUT` for the copy/initialization and allocation-tag declarations, `GAME_PIIPROPDATA_EMISSION_LAYOUT` for the main emission view, `GAME_PIIPROPDATA_ACCESSOR_LAYOUT` for accessor declarations, and `GAME_PIIPROPDATA_ITERATOR_LAYOUT` for the two copy declarations. The packet was built over the then-current shared vector header, retaining the previously accepted selector, model, and other unit controls. It does not replace that header with an older scratch snapshot.

The literal-false iterator requests, delayed concrete definitions, explicit generation request, two-level assignment wrapper, native section placement, scoped declaration views, and strong accessor/copy fragments are D-0016 accommodations. They are not claimed as original source structure. The recovered MSL copy and initialization boundaries are supported separately by the primary-source evidence.

## Traps

- Moving code with native sections does not move its exception records into the same order. Compare the retained record sequence after native weak/strong selection.
- Splitting allocation bodies can duplicate both the message literal and the compiler-generated exception hierarchy. Preserve a single allocation-containing compiler input when those literals belong to one original pool.
- A wrapper with the original instruction count can still reverse temporary stack slots and cleanup offsets. Check inline-depth parity before changing register declarations.
- A declaration-only header can preserve a call while postponing its implementation, but implicit copy-constructor declarations can change return ABI. Keep the original implicit-copy view in the main input and isolate strong trivial copies when necessary.
- A fragment containing correct strong functions can also emit weak helpers requested by their bodies. Validate actual ownership and dead stripping through the complete native map and frozen executable.
