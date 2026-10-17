# WPSE reference extraction

The extracted reference tree is `orig/wpse-v1/extraction/`. It contains every top-level WAD content and metadata file emitted by dtk, each recognized decompression result, and the entries of every recognized U8 archive. Original compressed bytes and wrappers remain alongside the derived files. This is reference extraction, not reconstructed source or a matching game build.

## Where to look

| Material | Location |
| --- | --- |
| Main executable | `orig/wpse-v1/main.dol` |
| Boot executable, byte-identical copy of content 6 | `orig/wpse-v1/boot.dol` |
| All seven original contents and package metadata | `orig/wpse-v1/extraction/contents/` |
| Original game asset archive entries | `orig/wpse-v1/extraction/expanded/00005/files/` |
| Unpacked Pokemon resource archive | `orig/wpse-v1/extraction/expanded/00186/files/` |
| Unpacked game UI archive | `orig/wpse-v1/extraction/expanded/00182/files/` |
| Unpacked scripts archive | `orig/wpse-v1/extraction/expanded/00189/files/` |
| Unpacked English messages | `orig/wpse-v1/extraction/expanded/00271/files/` |
| Main sound archive | `orig/wpse-v1/extraction/expanded/00005/files/sound/waza_p.brsar` |
| Every unpacked archive directory | [Archive index](ARCHIVE_INDEX.md) |
| File inventory for browsing | [CSV inventory](wpse-v1-extraction.csv) |
| Full provenance, original entry names, offsets, tool hashes, and command results | [JSON inventory](wpse-v1-extraction.json) |

Expanded directories use stable numeric parent IDs to avoid Windows path-length problems and collisions between repeated nested archive names. Look up an ID in the CSV or JSON: `logical_path` preserves its archive ancestry, `parent` identifies the input file, and `sha256` fingerprints its complete bytes. Each U8 record additionally preserves entry offsets, sizes, original names, and normalized output paths. Numbers are specific to this deterministic traversal, not game resource IDs.

## Reproduce

Run from the repository root with Python 3.10 or newer, Git, the recorded dtk binary, and the pinned supporting decoder sources. The validated interpreter was Python 3.14.0. The user-supplied dtk is 1.8.4, commit `a0c455e46cab58e1d2e0885623f85089ff0499db`; its executable SHA-256 is recorded in F-0003 and enforced by the extractor. Its acquisition source remains unverified.

The supporting decoder source is [Plombo/vcromclaim](https://github.com/Plombo/vcromclaim), commit `6e537af6d314c3d8a6c7353645c137572b37b4b6`. Its `lzh8.py` and `huf8.py` are Bryan Cain's Python implementations based on hcs's decoders. Source stays in ignored local storage. The extractor loads those pinned files with narrow Python 3 compatibility substitutions: byte array initializers, integer division, byte output, `tobytes`, and the extended-size input-variable correction. No third-party decoder source is copied into versioned project files. Both source hashes are enforced before loading.

```powershell
git clone https://github.com/Plombo/vcromclaim.git tools/local/vcromclaim
git -C tools/local/vcromclaim checkout 6e537af6d314c3d8a6c7353645c137572b37b4b6
python tools/extract_reference.py
```

Skip clone/checkout when the pinned checkout already exists. The output directory must not exist. To validate a separate extraction without replacing any originals:

```powershell
python tools/extract_reference.py --output build/wpse-v1/extraction-repeat --report docs/re/local/wpse-v1/extraction-repeat.json
```

The script runs actual `dtk wad extract`, `dtk u8 extract --no-decompress`, and `dtk nlzss decompress -o` commands. LZH8 and Huff8 are handled by the supporting decoders because dtk NLZSS rejects the observed 0x40 format. Output directories are separate from inputs; a second run against an existing destination is rejected.

The convenience executable copies were made separately: `main.dol` predates this recursive run and was compared to the newly decoded content 1; `boot.dol` is a byte-identical copy of `extraction/contents/00000006.app`. The extraction command itself produces both executable references within its recorded tree.

## Coverage and validation

The completed run recorded 3,420 files totaling 158,750,916 bytes, including retained containers, decoded intermediate files, metadata, and duplicate banner/trailer resources. These counts do not represent unique assets.

| Operation | Files |
| --- | ---: |
| U8 archives unpacked | 156 |
| NLZSS decompressions | 26 |
| LZH8 decompressions | 103 |
| Huff8 decompressions | 5 |
| Banner wrappers opened | 2 |
| IMD5 payloads validated and extracted | 6 |
| LZ77 tag wrappers removed | 4 |
| Retained terminal resources and metadata | 3,118 |

All 183 dtk invocations in the successful run exited 0. A fresh second extraction also exited 0; all 3,420 files compared byte-for-byte across the runs, and ancestry/entry records agreed. The seven decrypted contents matched TMD sizes and SHA-1 digests. Every U8 output was compared byte-for-byte to its source entry; archive bounds and path safety were checked before extraction. Every decompression matched its declared output length. All six IMD5 payload hashes matched. All 3,420 recorded file sizes and SHA-256 values were independently re-read after extraction. Original root and reference WAD hashes stayed equal to the frozen package fingerprint.

Retained resources include 1,269 TPL files, 885 BRLAN files, 475 BRRES files, 192 BRLYT files, 114 BSP files, 88 PKC files, 28 MIDI files, 24 BRFNT files, ten BWAV files, ten BMG files, and seven CSV files. Three RSAR sound archives are present; two retain generic decoded filenames in the provenance tree. Declared file sizes in all 1,581 BRRES, RSAR, RLAN, RLYT, RFNT, REFF, and REFT resources agreed with actual lengths.

## Limits

BRRES, BRSAR/RSAR, effects, textures, layouts, fonts, messages, and game-specific BSP/PKC data remain in their original native formats. Subresources inside these formats have not been exported to PNG, audio, model interchange formats, or editable scripts. Native files are the relevant byte-preserving inputs for reconstruction; conversions would be additional derived outputs.

No files named with `.rel`, `.rso`, or `.elf` extensions were discovered. This does not rule out embedded or differently named executable code. The main and boot executables are preserved; the boot-to-main control flow and complete executable coverage still require reverse engineering. Unknown game-specific resource semantics, internal resource offsets, padding reproduction, compression reproduction, and package rebuilding remain future work. Extraction success does not satisfy the source reconstruction or exact build acceptance gates.

The earlier exploratory `orig/wpse-v1/assets/` tree is redundant but preserved. The failed first recursive attempt was moved to ignored `build/wpse-v1/extraction-incomplete/`; use the successful tree and its inventory above.
