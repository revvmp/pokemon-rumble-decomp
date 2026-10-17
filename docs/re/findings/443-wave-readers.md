# Wave archive and wave file readers

## F-443-2: Wave archive reader source

Target `wpse-v1`. The complete `nw4r/snd/snd_WaveArchive.cpp` unit occupies `.text` `0x802910A0` through `0x802911A8`. It contains the constructor at `0x802910A0`, native extent `0x74`, and `GetWaveFile(int) const` at `0x80291120`, native extent `0x88`: two retained functions / 252 native bytes. The twelve bytes between the functions belong to the original unit alignment. The eight bytes after the last function remain with the following fallback boundary.

The canonical existing class header models the two table/data pointers and original helper declarations. The source validates the RWAR signature and supported version, locates table and data blocks, bounds-checks an archive index and resolves the stored data reference. It uses the original C++ class methods and descriptive pointer names. No raw executable data or decompiler output is used as implementation.

## Provenance

Adapted from CC0 zeldaret/ss commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/snd/snd_WaveArchive.cpp`. The reference identifies kiwi515/ogws as its original source. The existing canonical header and types are retained. Debug assertions and comments are omitted, and version/signature constants use the already declared canonical names. Target instructions, relocation and final link establish applicability to this release.

## Acceptance

The split-only default configure, full Ninja and explicit frozen verifier exit 0. The source is first registered NonMatching and compiled through `all_source`; objdiff reports `.text` 100%. The unit uses the already evidenced NW4R compiler Wii/1.1 and unchanged `cflags_nw4r`.

With normal Matching selection, default configure and full Ninja exit 0, followed by the explicit frozen verifier. The independent native audit checks both original extents, every non-relocated instruction word, the single paired relocation's offset/type/destination/addend, actual source object selection without the corresponding original object, and both final ELF addresses. The source-only `VerifyFileHeader` body is stripped. The unit has no retained initialized-data or storage contribution.

The complete actual DOL is directly byte-identical to the 4,094,880-byte frozen reference, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope remains the main game executable, excluding the boot loader, other package contents and WAD packaging. This is partial progress on the original 22-function issue #443; its Util tail, WaveFile reader and WaveSound entries require their own source-selected acceptance.

## Traps

No new reusable trap emerged. As in F-495-1, the split ends at the final function's end rather than the following alignment boundary; split-only and source-selected full verification both pass with that boundary.

## F-443-3: WaveFile reader source and unit compiler evidence

The complete `nw4r/snd/snd_WaveFile.cpp` unit occupies `.text` `0x802911B0` through `0x80291414`. Its two constructors at `0x802911B0` and `0x802911D0` occupy `0x20` and `0x8` bytes. `ReadWaveInfo` at `0x802911E0` occupies `0x1CC` bytes. `GetWaveDataAddress` at `0x802913B0` occupies `0x64` bytes. These four retained functions total 600 native bytes. F-443-1 supplies the two original helper declarations in the existing canonical class; no layout changes are needed.

The source follows the same pinned CC0 zeldaret/ss commit and `src/nw4r/snd/snd_WaveFile.cpp`. It converts the original wave format, sample rate and DSP sample positions, reads the channel table, copies the already modeled ADPCM parameter fields and resolves relative or absolute sample addresses. The existing WaveFile format enumeration replaces equivalent numeric format cases. The source preserves original C++ methods, readable pointer/reference names and the canonical AdpcmInfo representation.

The isolated reference-order probe using the general NW4R `-ipa file` setting produced a `0x228`-byte ReadWaveInfo by inlining the address helper. Disabling IPA alone produced `0x19C` bytes because the format converter was then out of line. Placing the format-converter definition before ReadWaveInfo and using `-ipa off` produces the target's `0x1CC` bytes with every non-relocated word exact. Deferred-inline variants and Wii/1.0 did not fix the initial oversized body. This conclusion is scoped to this unit: compiler Wii/1.1, otherwise unchanged `cflags_nw4r`, with `-ipa file` replaced by `-ipa off`. No global compiler rule changes.

The split-only default configure, full Ninja and explicit frozen verifier exit 0. Registered NonMatching source compilation through `all_source` succeeds and objdiff reports `.text` 100%, with all four retained function comparisons 100%. Normal Matching selection then passes the full default build and explicit frozen verifier. The independent combined-reader audit checks all six retained functions / 852 native bytes across both units, every non-relocated word and all six paired relocation offsets, types, named destinations and addends, actual source object selection without either original object, and final ELF addresses and extents. Both units have no retained initialized-data or storage contribution; source-only header and format helpers are stripped.

The complete actual DOL directly equals the frozen artifact with the size and SHA-256 above. This accepts six of the original 22 issue functions without claiming the remaining Util tail or WaveSound entries.

### Additional trap

Disabling IPA can also remove an intended inline expansion when its definition appears after the caller. This unit needs both the evidenced per-unit IPA setting and the format-converter definition before ReadWaveInfo. Keeping only either change fails the target native extent; the confirmed combined variant passes raw, relocation and full-artifact comparison.
