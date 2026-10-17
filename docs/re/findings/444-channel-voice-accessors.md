# F-444-2: Channel and Voice accessor prerequisite

Target: `wpse-v1`. This prerequisite adds `Channel::GetVoice() const`, `Channel::SetVoiceOutParam(int, const VoiceOutParam&)`, and the existing native `Voice::SetVoiceOutParam(int, const VoiceOutParam&)` declaration. The two Channel methods are inline accessors over the already declared `mVoice`. No class members, base classes, virtual methods, sizes or offsets change. No source unit selection changes and no issue closure is claimed.

## Evidence and provenance

The methods follow CC0 zeldaret/ss commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `include/nw4r/snd/snd_Channel.h`, and its `snd_WsdPlayer.cpp` call sites. WsdPlayer's target `UpdateChannel` at `0x80292700` loads the Channel voice pointer at `0xD0` and calls `0x8028ED20` with an output index and a `VoiceOutParam` reference. The target's existing `VoiceOutParam` occupies 24 bytes, and the loop advances its input by 24 bytes. The existing Channel and Voice member layouts already express the accessed fields. The declaration enables the later owned source reconstruction; this prerequisite adds no call sites or source coverage.

## Isolated native comparison

The source baseline is main `539bb56eb18d16f9a3eb584391b5490786f919cb`. Its unchanged full build and frozen verifier passed before this header change. The comparison uses the actual 378 `all_source` object outputs: 371 normally selected source objects plus seven registered NonMatching source objects. Two new objects from the separate partial issue #444 branch are explicitly excluded.

An isolated build with only these header additions compiled all 378 registered objects. Of those, 375 complete object files have identical SHA-256 digests. `snd_AxManager.o`, `snd_Bank.o`, and `snd_Channel.o` differ only in `.strtab`: CodeWarrior's numeric local names advance after the new inline methods are parsed. For each of those three objects, every allocated section is byte-identical, all raw RELA entries are byte-identical, and all symbol values, sizes, types and ordering are identical. Every changed symbol spelling is an internal numeric `@` name. No code, retained data, BSS extent or relocation changes.

`python configure.py --build-dir build/444-header-baseline-candidate` and `.venv/Scripts/ninja.exe` exited 0. The explicit verifier of `build/444-header-baseline-candidate/wpse-v1` exited 0 and directly compared that candidate, independently of the build's default-path SHA-1 rule. The complete candidate DOL is byte-identical to the 4,094,880-byte frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The object comparison is scoped to the isolated baseline above. Later rebases retain the current main source selections and require a passing exact-head local configure, Ninja and frozen-verifier gate before merge. Unrelated main changes are outside the isolated comparison.

## Traps

- **New inline declarations can change only compiler-local strings.** Full object hashes changed for three callers even though their allocated sections, raw relocations and symbol attributes were identical. The change is disclosed rather than called complete object equality; acceptance still requires direct complete-DOL comparison against the frozen artifact.
