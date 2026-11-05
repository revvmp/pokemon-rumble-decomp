# Selector object-data matching audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The accepted final executable is exact, but its pre-link objdiff data diagnostic is not complete. On 2026-11-05, the current report contains 1,245,840 matched data bytes out of 1,788,360 (69.663826%). CDlgPiiSelector contributes only 48 matched bytes out of 20,260 even though all 645 functions and the final linked artifact were accepted previously.

This audit reads the existing compiler and original split objects. No source, original executable, expected hash, or production object was changed. Isolated comparison files are under `tools/local/selector-data-20261105/`.

## Section evidence

| Section | Original split size | Compiler size | Observation |
| --- | ---: | ---: | --- |
| `.data` | 6992 | 6986 | All 6986 common bytes are identical; the remaining six original bytes are zero |
| `.sdata` | 240 | 562 | Extra compiler RTTI is discarded by the final linker; explicit retained identities improve comparison |
| `.sbss` | 8 | 8 | Inferred original placeholder size includes padding and differs from its real one-byte compiler symbol |
| `.sdata2` | 48 | 48 | Exact |
| `extab` | 8520 | 11352 | Extra weak-function records exist before final stripping |
| `extabindex` | 4452 | 7284 | Extra weak-function records exist before final stripping |

The original `.data` extent is `0x8039F4D0–0x803A1020`; the compiler's meaningful bytes end at `0x803A101A`. Preserve that section boundary. Correcting actual symbol extents and explicit mappings is the first approach; any later exclusion of padding would need separate native-alignment proof through the complete link. Padding must not be replaced by artificial source objects merely to improve the object score.

## Proven small-data identities

The original split and compiled ELF symbol tables establish these local literal correspondences. Source and target offsets agree for the literals. Compiler-generated numbers are comparison identities, not names introduced into reconstructed source.

| Original symbol | Compiler symbol | Offset in `.sdata` |
| --- | --- | --- |
| `@25765` | `@26307` | `0x10` |
| `@25766` | `@26308` | `0x18` |
| `@27480` | `@26437` | `0x1C` |
| `@27534` | `@26501` | `0x24` |
| `@24836` | `@26580` | `0x28` |
| `@24837` | `@26581` | `0x30` |
| `@24839` | `@26583` | `0x38` |
| `@24842` | `@26586` | `0x40` |
| `@24845` | `@26589` | `0x48` |
| `@24846` | `@26590` | `0x4C` |
| `@24847` | `@26591` | `0x54` |
| `@24849` | `@26592` | `0x60` |

The eight-byte RTTI record named `lbl_804B6518` corresponds to `__RTTI__Q33std3tr175enable_shared_from_this<Q229@unnamed@CDlgPiiSelector_cpp@15CDlgPiiSelector>`. Both records occupy `.sdata + 0x70` and have identical object bytes. Their type identity also agrees with the selector's established shared-pointer inheritance.

An isolated ordinary `objdiff-cli report generate` with explicit `symbol_mappings` exits zero and raises `.sdata` from 94.618835% to 97.72796%. Its remaining mapped symbol differences are incorrect inferred extents:

| Original address | Symbol | Inferred target size | Compiler size |
| --- | --- | ---: | ---: |
| `0x804B64A8` | `sMaxPlayerNum__29@unnamed@CDlgPiiSelector_cpp@` | 8 | 4 |
| `0x804B64CC` | `@27534`, the empty string | 4 | 1 |
| `0x804B8FE0` | `_1__Q33std3tr112placeholders` | 8 | 1 |

These corrections were proposed to the coordinator; this isolated experiment did not edit shared symbols or splits. Their following zero bytes require explicit padding treatment if the measured denominator changes. Mappings alone leave matched data at 48/20260 because this report awards data credit only for fully exact sections.

## Final pooled metadata extent

The inferred `lbl_803A0F7C` object spans 164 bytes, combining four real compiler symbols and alignment. Their actual offsets, sizes, and complete byte representations agree with the original `.data` prefix:

| Address | Proposed descriptive target identity | Compiler identity | Size |
| --- | --- | --- | ---: |
| `0x803A0F7C` | `sSelectorMultiPlayerDataDeleterTypeName` | `@23727` | 93 |
| `0x803A0FDC` | `sSelectorCharArrayDeleterTypeName` | `@23728` | 36 |
| `0x803A1000` | `sSelectorBadWeakPointerMessage` | `@STRING@what__Q33std3tr112bad_weak_ptrCFv` | 13 |
| `0x803A1010` | `sSelectorExceptionMessage` | `@STRING@what__Q23std9exceptionCFv` | 10 |

The two type strings name `std::detail::default_delete<@unnamed@CDlgPiiSelector_cpp@::CDlgPiiSelector::MultiPlayerData>` and `std::detail::default_delete<char[]>`; the last two strings are `bad_weak_ptr` and `exception`. The descriptive target names express recovered contents and are not claims of original symbol spelling. All are local comparison symbols. The proposed replacements keep the existing section range unchanged and leave intervening and trailing zeros as alignment.

Exact coordinator packets are `symbol-corrections.json`, `symbol-mappings.json`, and `data-tail-corrections.json` in the isolated diagnostic directory. The last packet contains old/new symbol lines and compiler mappings for these four objects.

## Small-BSS ownership

The compiler emits the one-byte placeholder at `.sbss + 0`, then a local four-byte `spDefaultMotionPane__29@unnamed@CDlgPiiSelector_cpp@` at `+4`. That pointer is referenced by the extra weak `CLayout::CreateMotion(const char*)` at its first instruction. The original selected unit does not retain that function or any relocation to `0x804B8FE4`.

The original placeholder has three consumers: `CDataPerPlayer::Setup + 0x820`, `Setup + 0x8C4`, and `RunSelection + 0x3D0`. All three target the placeholder itself with addend zero. Their compiler correspondences agree. Therefore the remaining seven bytes must not be mislabeled as a retained motion-pane pointer: they are padding in the original selected extent, while a discarded compiler definition happens to share part of that pre-link range.

## Traps

- A complete final source link does not imply 100% object-level matched data. Preserve both measurements; the current objective specifically requires the latter.
- An eight-byte inferred BSS symbol can combine a one-byte object with padding. Inspect original relocation consumers before copying a compiler-only neighboring symbol into the target map.
- Extra weak RTTI and exception records are legitimate compiler output. Correct explicit retained-symbol identities and boundaries before attempting source changes that could break the accepted full artifact.

## Validation limits

The mapping experiment is diagnostic only. It does not claim full selector data acceptance, a corrected denominator, or a newly verified full build. The coordinator owns shared configuration, complete link verification, exception mappings, and final progress updates.
