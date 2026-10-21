# Pii property script bindings (#270)

## F-270-1: Native record and additive interfaces

The original issue owns 26 functions and 4,436 instruction bytes at `0x80157DE0`–`0x80158FF4`. The binding names come from `docs/re/script-bindings.tsv`. This prerequisite does not select reconstructed binding source or complete the issue.

`CPpuEngine::PopSpeciesData()` at `0x80100F10` consumes an integer script value and returns the corresponding borrowed `const PiiSpeciesData*`, with zero becoming null. The native ppr getter chain immediately calls the existing species getter at `0x80124F10`. An independent AddEnemyToNoticeDisplay producer in #238 reproduces all 200 code bytes, its three call destinations, and eight EH bytes using the same declaration; its receipt is `238-reference/238-positive-native-proof.json`.

The complete 28-byte PiiSpeciesData layout is preserved. Additive getters describe the observed packed form, bone, flight-height and size fields, the full word at offset eight, the two float coefficients, and the signed type-effect result. `GetGenderAndTypeFlags()` returns the whole unsigned word, not an unsigned byte; the binding narrows its result explicitly.

The 56-byte PiiPropData record is no longer opaque storage. The native constructor at `0x800F2630` constructs a vector of 508 72-byte move records, each containing one vector of signed-short pairs and five vectors of signed shorts. It then constructs MapPiiTable at offset 12. The existing MapPiiTable contract occupies 44 bytes. Native `Initialize` at `0x800F3120` forwards 508 rows and the requested column count to that member. Native `GetWazaLevels` at `0x800F5B00` reads the signed level/move pair and five move-group vectors. Member names are descriptive; no original source spelling is claimed.

## F-270-2: Scalar vector construction

The recovered 2006 MSL vector header provides separate false and true default-construction overloads. The false overload constructs each object; the true overload clears the requested scalar storage with memset and increases the size. Its transformed constructor specialization maps signed short to an unsigned-short allocator/storage backend. The bounded reconstruction adds that supported short specialization and adapts the existing public vector interface to retain signed references and iterators. It does not add an empty wrapper solely to change code size. Both existing false construction loops are preserved.

The reference is `198-reference/primary-header-research/alpha2006/CW_RVL/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/vector`, with transformed constructor declarations beginning near line 2067 and store_as<short> in the adjacent msl_utility header. Reference hashes and provenance are recorded in `198-reference/primary-header-research/2006-primary-manifest.json`.

Actual SetPiiWazaLevels changes from 680 bytes with per-element construction to exactly 632 native bytes and every resolved target. Its complete 80-byte EH record also matches. Native cleanup calls independently identify unsigned-short storage destruction at `0x8007B2A0` and signed-short vector destruction at `0x800F44C0`; the naturally emitted 112/124-byte bodies reproduce every instruction and target. The actual getter uses list::push_front and its native list<int> cleanup at `0x801017D0`; its 184-byte body is also strict exact. These are typed producer/EH bindings, not aliases inferred from identical destructor bytes alone.

## F-270-3: Current consumer and frozen controls

The current-base checkpoint is `270-reference/270-prerequisite-checkpoint-32d30869ef/`, including candidate headers, ELF/DOL, original issue inventory, actual producer, strict code/EH receipts, current-consumer receipt and SHA-256 manifest. The baseline is main `32d30869ef5c52995465c5960f071bbec9d9f6c0`.

All 765 configured source objects compile. All allocated section metadata and payloads and all relocation contracts are unchanged from that baseline. Fifty-seven object files differ in compiler metadata/private symbol counters only. Private `@` and `__FUNCTION__$` symbols are compared by their exact section, offset and size; allocated payloads are still compared in full. No function bytes, relocation destinations, section sizes or alignments are masked. Fresh default linking and frozen-manifest verification pass.

Reproducers are `270-vector-consumers.py`, `270-vector-consumer-audit.py`, `270-canonical-candidate-audit.py`, `270-canonical-candidate-eh.py`, and `270-transformed-cleanup-audit.py` in `270-reference`. The canonical-header actual producer has 25/26 exact original functions, totaling 4,264/4,436 code bytes, plus all 22 original EH records totaling 264 bytes. It is not yet accepted source coverage.

## Traps

- A universal scalar construction loop omits the primary POD memset contract. Merely replacing it with memset leaves the signed-short storage adapter absent: the setter is four bytes short and the getter eight bytes short. The actual transformed constructor, typed cleanup destinations and native cleanup bodies establish the missing ownership layer.
- SetPiiProps uses the custom CPpuEngine receiver context, as independently seen in accepted script bindings. Keeping the generic base-engine context changed register allocation and caused its two wrappers to inline to 308 bytes. A typed local receiver restores the complete 312-byte helper and both native eight-byte wrappers without flags or retention code.
- PopArray returns the output cursor in the native move-level getter. Saving a separate original pointer afterward is necessary for the final count; reversing those roles preserves behavior but changes nine words. The native first receiver and subsequent copy establish the correct dataflow.
- The final WazaKouka binding still has eight register words differing in an otherwise 172-byte body. Wii/1.3, an explicit local script value, const qualification and the custom-engine local do not resolve it. No flag, register-retention variable or artificial source operation was adopted.
