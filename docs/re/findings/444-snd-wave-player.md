# F-444-1: WsdFileReader and DSP ADPCM decoder

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. This checkpoint accepts five of the 37 functions in issue #444: four `WsdFileReader` functions and `DecodeDspAdpcm`, totaling 1,096 native code bytes. The other 32 issue functions remain pending.

## Units and evidence

| Unit | Sections | Retained functions | Proof |
| --- | --- | --- | --- |
| `snd_WsdFile.cpp` | `.text` `0x80291820` through `0x80291BE4`; `.sdata2` `0x804BB090` through `0x804BB094` | Constructor, `ReadWaveSoundInfo`, `ReadWaveSoundNoteInfo`, `ReadWaveInfo`; 956 bytes | All four functions and both sections score 100% in objdiff; all 13 relocations have equal relative offsets, types and targets or constant bytes |
| `snd_adpcm.cpp` | `.text` `0x80292C68` through `0x80292CF4` | `DecodeDspAdpcm`; 140 bytes | All 35 instruction words are identical; objdiff 100%; no relocations |

Both source objects are selected by the normal link, and neither original object is selected. The final ELF preserves all five original function addresses and sizes. `main.dol` is directly byte-identical to the 4,094,880-byte frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The unchanged baseline and split-only builds both exited 0 and passed the explicit frozen verifier before the source objects were selected. The accepted source build used `python configure.py`, `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, all exit 0. A separate native audit checked raw non-relocated words, all 13 paired relocations, source selection, final ELF addresses and extents, and direct DOL equality.

## Reference revision and layouts

`WsdFileReader` follows CC0 zeldaret/ss commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/snd/snd_WsdFile.cpp` and `include/nw4r/snd/snd_WsdFile.h`, with the two output structures from its `snd_WaveFile.h`. Debug assertions are omitted for this release build, comments are removed, and include paths and version macros use the project's existing headers. `Util::Table::items` retains the existing canonical member spelling. No existing shared header changes are needed.

The target accepts RWSD versions 1.0 through 1.3. It checks each block offset before adding it to the header. Without an embedded wave block, `ReadWaveInfo` reads a wave file through `WaveArchiveReader`. Version 1.2 and later use the supplied effect-send bytes. `WaveSoundNoteInfo` contains the byte-sized hold value at offset 5, copied from `WsdFile::NoteInfo` offset 8. These behaviors distinguish the target from the older ogws revision.

`WsdFileReader` is 12 bytes, with header, data-block and wave-block pointers at 0, 4 and 8. `WaveSoundInfo` is 12 bytes; pitch is at 0 and the pan/send bytes occupy offsets 4 through 9. `WaveSoundNoteInfo` is 20 bytes; wave index is at 0, envelope bytes at 4 through 8, original key at 9, pan at 10, surround pan at 11, volume at 12 and pitch at 16. `WsdFile::NoteInfo` is 48 bytes, and `WsdFile::WsdInfo` is 32 bytes. Names and layouts have high confidence from the pinned reference and exact target code.

`IsValidFileHeader` is emitted as an unretained source helper and is inlined into the retained constructor. The final link strips its standalone copy. The unit's only retained small-data constant is `1.0f` at `0x804BB090`; the survey's eight-byte extent included the next four padding bytes and is corrected to four.

The decoder follows CC0 kiwi515/ogws commit `27fa94a593096eed7442efc4e70981fd52025b9c`, `src/nw4r/snd/snd_adpcm.cpp`. The coefficient index is `int`, which removes the extra sign extension emitted by ogws's `s16` index and reproduces the target exactly. Compilation uses the established Wii/1.1 NW4R flags, with `-func_align 4` only for this packed decoder unit. Its first address is eight bytes past a 16-byte boundary. No broader NW4R alignment conclusion follows.

## Remaining inventory

The beginning of #444 is the tail of `WaveSound`, whose first function is in neighboring issue #443. Its split is left to that owner. `WaveSoundHandle::DetachSound` begins at `0x802917E0`. `WsdPlayer` starts at `0x80291BF0` and ends at `0x80292C68`, wholly inside #444. The source-selected acceptance of those units and the complete 37-function inventory remain pending.

## Traps

- **Exact decoder instructions can still shift the link.** The default 16-byte function alignment moved the decoder from its packed start and failed the DOL checksum. The evidenced per-unit `-func_align 4` preserves the original start and passes the full frozen verification.
- **The older decoder's index type adds an instruction.** An `s16` coefficient index emitted an extra `extsh`; the `int` index reproduces all 35 original words with the unchanged NW4R optimization flags.
