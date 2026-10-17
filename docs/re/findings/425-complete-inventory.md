# F-425-2: Complete original sound inventory audit

Target `wpse-v1`, immutable `config/targets/wpse-v1.json`. The original issue lists thirty survey entries / 5,008 native bytes at `0x8027CC00`-`0x8027E054`. They are covered by twenty-nine actual native functions, with the boundary correction below. No new source implementation bytes are claimed by this audit.

## Ownership and native boundaries

| Source unit | Original survey entries | Actual functions in #425 | Owned native bytes |
| --- | --- | --- | --- |
| MemorySoundArchive | 12 | 11 | 288 |
| MidiSeqPlayer | 11 | 11 | 84 |
| MmlParser | 5 | 5 | 4,556 |
| MmlSeqTrack | 2 | 2 | 80 |
| Total | 30 | 29 | 5,008 |

The survey's `0x8027CC60` / `0x44` Seek entry and `0x8027CCA4` / four-byte placeholder form one `MemoryFileStream::Seek` body of `0x48` bytes. The latter word is the trailing unreachable `blr` (`4E800020`), already identified in [F-424-2](424-snd-instancepool-lfo.md#f-424-2-nw4r-snd-memorysoundarchive-unit). The audit validates the whole native source body and records the two exact original subextents at offsets zero and `0x44`. It does not invent a second function or count the word twice.

The two MmlSeqTrack entries, constructor at `0x8027E000` and Parse at `0x8027E040`, come from the complete shared file in PR #829. The user's whole-file approval across #425/#426 is recorded in [F-426-2](426-snd-shared-interfaces.md). Its destructor at `0x8027E060` belongs to #426. Existing MemorySoundArchive, MidiSeqPlayer and MmlParser sources are established by F-424-2 and [F-425-1](425-snd-midiseqplayer-mmlparser.md). The named `IOStream::GetOffsetAlign` fallback at `0x8027C8C0` remains outside this issue's scope.

## Native data extents and alignment

The original metadata assigned eight bytes to the empty string at `0x804B89E0` and to `MmlParser::mPrintVarEnabledFlag` at `0x804B9360`. The actual source objects, final ELF and linker map show a one-byte NUL string and a one-byte bool. Original typed SDA accesses and complete raw values establish their correspondence. Only those two owned object extents are corrected to one; both split ranges and every address remain unchanged.

MmlParser's `.sdata` contains nine compiled bytes at `0x804B89D8`-`0x804B89E1`, including the T/G/empty strings and internal alignment. Seven following zero alignment bytes remain separate from the empty-string object, through `0x804B89E8`. Its native `.sbss` bool occupies `0x804B9360`-`0x804B9361`; seven alignment bytes remain through `0x804B9368`. No padding member, copied code or expected hash is introduced. These corrections make all compared MmlParser sections 100% in objdiff, including the previously mis-sized small-data and small-BSS symbols.

## Independent source proof

The four registered source units contain thirty-nine retained functions / 5,864 native bytes, including their explicitly uncredited neighboring coverage. Independent comparison checks every native function extent and non-relocated instruction word, the unaffected bits of each relocated instruction, all 82 paired code relocations and 156 data relocations, and the resolved destinations in the final ELF. Differently named compiler-local strings, literals and jump tables are paired through their actual native addresses and source-object contributions in the real linker map; no normalized artifact comparison is used.

The audit directly compares 745 raw compiled data-section bytes: MemorySoundArchive's 128-byte vtables, MmlParser's 560-byte data section, nine-byte small-data payload and 32-byte literal section, and MmlSeqTrack's sixteen-byte vtable. It separately checks the seven trailing small-data alignment bytes, native one-byte zero-storage flag and preserved small-BSS range. All retained code and every compared section in all four units score 100% in objdiff.

Actual Ninja link inputs select all four source objects and exclude their original split objects. The generated `build/wpse-v1/main.elf.MAP` attributes every retained native function, and each aliased local data target, to the corresponding source object at its exact address and extent. Separate original-inventory coverage records every one of the thirty survey rows, including both Seek subextents, and confirms twenty-nine distinct native function starts / 5,008 bytes.

The independent preparation starts from main `ec2e1acde5beaefdba7453a977cba9b67c8d2861` with the source owner's unchanged PR #829 commits. Fresh `python configure.py --map`, plain `.venv/Scripts/ninja`, and explicit actual-output frozen verification exit 0. After correcting the two data extents and recording this evidence, the committed candidate repeats the real pipeline and native/map/data audit. Final closure requires all consolidated issue inventories to pass on the same committed head after its final actual-main rebase; the source PR records that head and its commands.

The complete source-selected `main.dol` directly equals all 4,094,880 immutable reference bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope excludes the boot loader, other contents and WAD packaging. Source-byte credit remains with the original implementation contributions; this audit and its metadata corrections add none.

## Traps

- A survey can split an unreachable trailing instruction into a false function. Preserve every owned byte, but count the complete compiler-native body once and retain the original row-to-subextent mapping.
- A small-data or BSS symbol's inferred extent can include following alignment. Native source symbols, typed original consumers and actual final map addresses distinguish the one-byte object from seven alignment bytes. Correcting the owned metadata preserves the full frozen layout and restores exact object comparison.
