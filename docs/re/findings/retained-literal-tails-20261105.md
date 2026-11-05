# Four retained data tails

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The final Matched Data audit found four otherwise exact game units whose `.data` sections scored just below 100%. The remaining discrepancies were inferred target symbols spanning several independently emitted objects. Existing source objects contain all required data; no source changes are proposed.

| Unit | Data section bytes | Initial section score | Diagnostic corrected score |
| --- | ---: | ---: | ---: |
| CFileData | 2,952 | 99.96612% | 100% |
| CTitleSession | 2,472 | 99.959526% | 100% |
| CNumericInput | 2,192 | 99.95436% | 100% |
| PhotoSave | 1,288 | 99.9223% | 100% |

The private objdiff project at `tools/local/nw4r-data-audit/final-four/` reproduces these diagnostic results with the production comparator. Its target copies change symbol metadata only; all section bytes and ranges remain unchanged. `proposals.json` contains three size corrections, eighteen new symbol definitions, one obsolete label removal and the required explicit mappings. The four sections retain their combined 8,904-byte denominator. Production acceptance still requires the coordinator's ordinary dtk regeneration and frozen full-DOL verification.

## CFileData

The symbol `@STRING@OpenFileStream...` at `0x803BEFD0` was assigned 48 bytes. Its actual filename literal, `CFileData.cpp`, occupies fourteen bytes. The remaining span contains the independently emitted `bad_function_call` literal at `0x803BEFE0`, size eighteen, and `exception` at `0x803BEFF4`, size ten, separated by alignment padding.

The existing source symbols are the OpenFileStream literal, `@STRING@what__Q33std3tr117bad_function_callCFv`, and `@STRING@what__Q23std9exceptionCFv`. Their exact bytes match the original subranges. The proposed additional target identities are `sFileDataBadFunctionCall` and `sFileDataExceptionMessage`.

## CNumericInput and CTitleSession

CNumericInput's `@STRING@UpdateText__13CNumericInputFv` at `0x803A7584` contains the thirteen-byte `T_2l_TextBox` literal. Its inferred 28-byte extent had absorbed the separate ten-byte `exception` literal at `0x803A7594` and alignment padding. The second literal maps to the already compiled exception `what` string; the proposed target identity is `sNumericInputExceptionMessage`.

CTitleSession's `lbl_803AEDC4` is the same ten-byte `exception` text, followed by two alignment bytes. Giving it the demonstrated ten-byte size and mapping it to the compiler's exception `what` string completes the existing 2,472-byte section.

## PhotoSave

The inferred `lbl_803B804A` begins inside a zeroed duplicate-vtable hole and extends 590 bytes to the unit's data boundary. It does not describe a real source object. The current source and final ELF expose fourteen retained RTTI names, base lists and diagnostic strings inside the span. A fifteenth object is the ten-byte exception `what` literal at `0x803B828C`.

| Address | Source symbol | Size | Meaning |
| --- | --- | ---: | --- |
| `0x803B8058` | `@5987` | 17 | logic_error type name |
| `0x803B806C` | `@5988` | 12 | logic_error base list |
| `0x803B808C` | `@5991` | 44 | shared_ptr_deleter_common type name |
| `0x803B80C8` | `@5996` | 15 | exception type name |
| `0x803B80D8` | `@12250` | 35 | reserve length-error message |
| `0x803B80FC` | `@12251` | 55 | reserve exception type sequence |
| `0x803B8134` | `@12510` | 27 | string out-of-range message |
| `0x803B8150` | `@12511` | 55 | out-of-range exception type sequence |
| `0x803B8188` | `@12512` | 27 | string length-error message |
| `0x803B81A4` | `@6252` | 25 | deleter function type name |
| `0x803B81D8` | `@6586` | 80 | character-array shared deleter type name |
| `0x803B8228` | `@6587` | 12 | character-array shared deleter base list |
| `0x803B8234` | `@6588` | 50 | disconnect-message guard default-deleter type name |
| `0x803B8268` | `@6632` | 36 | character-array default-deleter type name |
| `0x803B828C` | exception `what` literal | 10 | exception message |

Every proposed symbol's raw bytes were checked against the corresponding original object subrange. The original relocations at unit offsets `0x2DC` and `0x498` match the two retained base-list relocations. The duplicate weak vtables have source relocations but no original relocations at their zeroed holes, confirming that those holes must remain padding rather than invented source records. Removing the false umbrella label and identifying only the retained objects produces 100% without shortening the data section.

## Traps

An inferred terminal string symbol can absorb adjacent compiler-generated exception literals. A nearly exact section score does not prove a one-byte source error: inspect all typed and literal subobjects before changing source. Final-ELF symbol lookup can omit duplicate weak literal identities, so corroborate them with their compiled object names, exact contents and original subranges.
