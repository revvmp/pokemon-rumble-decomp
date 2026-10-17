# F-804-3: Complete VoiceManager source and issue inventory

Target `wpse-v1`. VoiceManager owns eleven retained functions / 1,892 native bytes at `0x80290420`-`0x80290BB4`. Together with CalcPitchRatio in [F-804-2](804-snd-util.md), these cover every original #804 entry: twelve functions / 2,312 native bytes. The complete Util unit additionally supplies seven #443 functions; its source credit is counted once.

## Source and storage

The readable C++ follows CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `src/nw4r/snd/snd_VoiceManager.cpp`, whose provenance identifies kiwi515/ogws. It uses the canonical intrusive VoiceList, scoped interrupt lock, placement construction, callbacks and existing class declarations. The proven target Voice layout and memory-size signature are prerequisites from [F-804-1](804-voice-util-layout.md). No Voice.cpp split or neighboring #803 source is taken.

The function-local manager occupies `0x8045248C`-`0x804524A8`, with its 12-byte compiler destructor-chain node immediately before it at `0x80452480`. Typed original high/low address operands establish both destinations. The one-byte construction guard is at `0x804B93A8`; its split extends to the next four-byte boundary `0x804B93AC`. The following padding remains separate. Numeric compiler-local node name `@1233` is a correspondence label, not a recovered semantic original name.

## Acceptance

The split-only full default Ninja and explicit frozen verifier exit 0. The source is registered NonMatching and compiled with unchanged Wii/1.1 `cflags_nw4r`. All eleven retained functions, .text, .bss and .sbss score 100% in objdiff. Normal Matching selection then passes the full default Ninja and explicit frozen verifier.

The independent native audit checks eleven original/source extents and every non-relocated instruction word, all 71 paired code relocations with identical offset/type/destination/addend, all three zero-storage objects' native extents and final addresses (41 bytes total), and every retained function's final address. The generated link rule selects the source object and excludes the original VoiceManager object. Source-only helpers strip normally. The complete 4,094,880-byte DOL directly equals the frozen reference, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The independent Util audit on the same build confirms eight functions / 1,168 native bytes, 42 paired code relocations, three paired data relocations, 9,360 raw initialized bytes and 48 resolved literal bytes. These two actual source-selected units therefore supply nineteen functions / 3,060 native bytes. The final pull request records the exact committed-head gate after its latest-main rebase and shared-header prerequisite merge. The artifact claim covers only main.dol, excluding boot loader, data contents and WAD packaging.

## Traps

- A one-byte guard object does not imply a one-byte split boundary. dtk rejects an automatically generated following split at `0x804B93A9` for invalid alignment. Keep the native object size one, and end its split at the next four-byte boundary; the full frozen link proves the alignment.
- An imported memory-size method can retain a revision-specific missing argument. Original `mulli r3,r4,0x104` proves this nonstatic method consumes an explicit voice count; F-804-1 corrects the declaration before source selection.
