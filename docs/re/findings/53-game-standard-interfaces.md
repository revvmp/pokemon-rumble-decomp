# F-53-2: Native type identity and shared control-block declarations

This header prerequisite supports issue #53 without reconstructing or taking
any neighboring game source unit. Its initial control is main `06215a99`.
The source branch's original-only `CBattleRoyalDisplay.cpp` split is separate
and absent from this header delivery. No executable functions, bytes or
Matching units are added to source-coverage credit.

## Native interfaces and independent source

F-541-5 already establishes the original twelve-byte
`std::tr1::detail::shared_ptr_deleter_common` class, two signed pointer-sized
reference counts at four and eight, and its three virtual slots. This change
keeps that declaration and layout, supplying its ordinary inline constructor
and destructor. The native display factory initializes both counts to one;
its returned-pointer copy increments the shared count at four. The new
`retain` member expresses that observed increment.

Native RTTI supplies the exact derived name
`std::tr1::detail::shared_ptr_deleter<@unnamed@CBattleRoyalDisplay_cpp@::CBattleRoyalDisplay, std::detail::default_delete<@unnamed@CBattleRoyalDisplay_cpp@::CBattleRoyalDisplay>>`.
The derived control allocation is sixteen bytes. Its pointer occupies twelve,
which is also the address returned for its empty default deleter. An ordinary
empty-base storage helper reproduces that overlapping empty-deleter/pointer
layout, without manually computing a member address. The default deleter
performs ordinary `delete` on the typed object; virtual dispatch follows the
native destructor slot.

`std::type_info` exposes the compiler's existing native metadata prefix:
name pointer at zero and base-list metadata pointer at four. These are the
same two fields independently consumed by the existing `Runtime/MWRTTI.cpp`.
Its equality operation compares the names using the existing `strcmp`.
Construction and copying are private; compiler-provided type records supply
the instances. A consumer comparing its supplied type-info argument against
`typeid(Deleter)` reproduces the native argument-register order.

All implementation here was independently expressed from those native
instructions and metadata. The MSL interface names were previously
corroborated in F-541-5. No implementation from the separately restricted
Freescale smart-pointer header was imported. A reconstructed public typeinfo
header was inspected only as an interface cross-check; its proposed hash field
is not evidence for this target's base-list metadata and is not imported.

## Actual canonical consumer

A diagnostic source outside the repository includes only project headers and
instantiates the new canonical compressed deleter with the observed virtual
destructor interface. It is compiled as `CBattleRoyalDisplay.cpp` so the
compiler uses the original anonymous-namespace spelling, with Wii/1.1 and the
unaltered `cflags_game` settings. The following three retained helpers match
every instruction byte and all three native relocation targets:

| Function | Native address | Bytes |
| --- | --- | --- |
| `get_deleter` | `0x8006BB40` | 76 |
| `dispose` | `0x8006BB90` | 36 |
| Derived control destructor | `0x8006BEA0` | 64 |

The 176-byte audit resolves `strcmp` to `0x80345DDC`, game `operator delete`
to `0x801B1580`, and the default-deleter type record to `0x804B5C40`. It does
not exclude relocation-bearing instructions. Its intentionally small abstract
display declaration is a template consumer, not a substitute for the complete
game class and not a source-selection claim.

## Existing-source and whole-artifact control

The normal configure, plain Ninja and actual-output frozen verifier pass.
The control reruns the real link and DOL conversion with the original two
shared headers on the same committed base and object order, preserves those
outputs outside the repository, then restores the proposed declarations and
repeats the actual build.

All 504 other baseline source objects preserve their allocated sections,
sizes, data and normalized relocation targets. In the remaining
`MSL_CPP/msl_thread.o`, both retained release bodies preserve all original
168 bytes and have no instruction relocations. Their original eight exception
bytes and twelve index bytes/relocations are unchanged.

That object additionally emits the now-visible common destructor: exactly
64 bytes, with its sole `operator delete` relocation resolving to the native
target. The complete body matches the original weak definition at
`0x80055BB0`, which belongs to the earlier game block. The added eight-byte
exception record matches `0x80006F58`; the original index entry at
`0x800442C8` confirms its 64-byte extent. The added twelve-byte index names
only that definition and its exception record. It is unreferenced and absent
from the final ELF; the original `fn_80055BB0` retains its original address and
extent. No ownership or byte credit is taken for it.

The complete DOL is byte-identical to the control and frozen original,
SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
The ELF has exactly the same section headers, section payloads and symbol
addresses, sizes, types and section indices. Only `.strtab` changes eight
compiler-generated numeric local labels, such as `@251` to `@292`; this is
explicitly not a whole-ELF-file identity claim. No retained allocation changes.

## Traps

The current-main repeat uses base `9fdce911`. Plain Ninja on both the
original-header control and proposed-header candidate includes every diagnostic
source object. All 505 other source objects preserve their allocated sections
and normalized relocations; the same bounded common-destructor extension is
the only change. Canonical-only helpers retain all 176 matching bytes and
three relocation targets. The full DOL and all allocated ELF content remain
identical, with the same eight numeric local-label changes confined to
`.strtab`. Configure, plain Ninja and the explicit frozen verifier exit zero.

- Supplying an ordinary inline base destructor can make an existing source
  object emit another weak definition and its exception records. Prove the
  original retained bodies unchanged, audit the added native definition, and
  check the full linked result rather than demanding an identical object
  digest or moving inline declarations inconsistently between translation
  units.
- A matching type-identity comparison can use the opposite source operand
  order and still return the same boolean. Only the observed operand order
  reproduces the target argument registers; compare every instruction.
- Building only `main.dol` can leave stale diagnostic objects for NonMatching
  units after a rebase. Capture a shared-header baseline after plain Ninja,
  which compiles those diagnostic objects as well as the linked sources.


## Refreshed main control at 76b5be1b

PR881 was rebased onto main `76b5be1b` without shared-file conflicts. A fresh same-base original-header control built all 516 source objects through plain Ninja, then the proposed headers rebuilt through the same real entry point. Configure, both Ninja builds and explicit frozen verification exited zero. The control ELF SHA-256 was `43c72dddbc437e6a609fdda9f50bb88d002764489b101f9b39560d9597eb260b`.

All 515 source objects outside msl_thread remain unchanged. The existing two release bodies, their exception records and relocations remain unchanged. The sole emission extension remains the original 64-byte common destructor with eight exception bytes and twelve index bytes, unretained in the final ELF. Whole-DOL SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; final ELF sections, header and symbol attributes remain unchanged, with only eight numeric local-label spellings changed in strtab. No additional source bytes are credited.

After allocator PR893 merged, independent wide-string and map-storage diagnostic probes switched from the approved temporary allocator copy to the actual canonical `include/MSL_CPP/allocator.h`. Fresh compilation still reproduces the two wide-string bodies (280 bytes), their three call targets, all 32 exception bytes and both index records; both map-storage constructors still reproduce 24 native bytes apiece. This does not publish either container implementation or resolve their remaining interfaces.


## PR881 all-source bookkeeping supplement

The final --no-progress default build skipped the newly introduced NonMatching C unit pf_entry.c. Its first folder contained 516 compiled source objects. This was a counting limit in the initial final-base control, not a retained-output change. Both original and proposed header controls were repeated with explicit all_source, followed by plain Ninja and actual frozen verification, all exit zero. The corrected complete baseline contains 517 objects: all 516 outside msl_thread preserve their allocated sections and relocation targets. The previously documented unretained common-destructor extension and eight local ELF label differences remain the only changes. The new C unit cannot consume the C++ interfaces; it adds no retained source credit in this header work.

The merged PR881 description records this supplement. Evidence remains in header-all-source-control.log, header-all-source-proposed.log and the corrected standard-header-baseline.json outside the repository.

### Additional trap

When configuring with --no-progress, default Ninja does not necessarily compile newly introduced NonMatching units. For a claim about all existing source objects, explicitly run ninja all_source before each hash capture, then separately run plain Ninja and the actual frozen verifier. A successful retained link alone is insufficient for that broader preservation statement.
