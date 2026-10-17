# F-443-2: Wave archive reader source

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
