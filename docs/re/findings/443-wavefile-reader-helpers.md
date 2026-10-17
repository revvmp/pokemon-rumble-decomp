# F-443-1: WaveFileReader helper declarations

Target `wpse-v1`. This shared-header prerequisite declares the original public `WaveFileReader::GetWaveDataAddress(const WaveFile::WaveChannelInfo*, const void*) const` and static `GetSampleFormatFromWaveFileFormat(u8)`. It changes no fields, class layout, virtual methods, source selections or splits. It supplies the interface needed to reconstruct the owned WaveFile reader unit in issue #443; it does not complete that issue.

## Evidence and provenance

The declarations follow CC0 zeldaret/ss commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `include/nw4r/snd/snd_WaveFile.h` and `src/nw4r/snd/snd_WaveFile.cpp`. The target retains the const address helper at `0x802913B0`, native extent `0x64`. Its instructions access the already modeled reader pointer and channel offset, and choose either the supplied data origin or the wave-info origin. The format conversion helper is inlined into `ReadWaveInfo` at `0x802911E0`; the recovered switch maps the original byte format to the existing SampleFormat enumeration. An isolated reconstructed-source compiler probe reproduces the non-relocated instruction words of all four retained WaveFile functions. Final source-unit relocation, data and link acceptance belongs to the later source change.

## Isolated native comparison

The unchanged baseline is main `71e1fb74c9bb727ee7ce8dfc1f0f6a065a5a21be`. Its default configure, full Ninja, explicit frozen verifier and `all_source` build exit 0. All 427 actual registered native object outputs were saved before adding the two declarations.

After the declarations, default configure, full Ninja, explicit frozen verification and `all_source` again exit 0. Of the 427 complete native objects, 424 have identical SHA-256 digests. `snd_Bank.o`, `snd_BankFile.o` and `snd_Channel.o` differ only in `.strtab` numeric compiler-local names. Their allocated sections, raw RELA entries, symbol values, sizes, types and ordering are identical. This is metadata spelling rather than complete object identity and is disclosed as such.

The frozen original and actual rebuilt DOL are directly byte-identical: 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope is the main game executable, excluding the boot loader, other contents and WAD packaging. Later unrelated main changes remain outside the isolated native-object comparison; the exact final committed head must pass configure, full Ninja and explicit frozen verification after its last rebase.

## Traps

The same compiler-local naming behavior documented in F-444-2 occurs with these declarations: a complete object hash can differ solely because numeric local strings change. Every allocated section and raw relocation entry must still compare exactly, and the full frozen artifact remains the final gate.
