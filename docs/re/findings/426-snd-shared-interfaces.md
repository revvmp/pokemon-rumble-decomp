# F-426-2: Canonical MML and remote-speaker interfaces

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. These header-only prerequisites support issue #426's MML allocator and remote-speaker sources. On 2026-10-17 the user explicitly approved covering the complete shared `MmlSeqTrack` file across #425 and #426. Issue #425's other units remain outside that approval and are not completed here.

## Native evidence

| Type or interface | Evidence |
| --- | --- |
| `SeqTrack` | Native constructor `0x80280FB0` initializes player and channel pointers at `0xC4/0xC8`, calls LFO initialization at `this+0x58`, and initializes four six-byte `MoveValue` objects at `0x70/0x76/0x7C/0x82`. The complete reference member layout, including the newer pitch-bend interpolation, is `0xCC` bytes. |
| `MmlSeqTrack` | Native constructor `0x8027E000` calls `SeqTrack` and installs vtable `0x803C6570`. `Parse` at `0x8027E040` reads its parser at `0xCC`, passes the track and note flag, and branches to `MmlParser::Parse` at `0x8027CE20`. Complete size is `0xD0`. |
| `MmlParser` | Vtable `0x803C6560` orders `CommandProc` at `0x8027D4B0` before `NoteOnCommandProc` at `0x8027DE30`. The matching track dispatch proves the parser call interface. Enum and declaration names follow the pinned original-library reference. |
| `SeqTrackAllocator` / `MmlSeqTrackAllocator` | Vtable `0x803C6580` orders destructor `0x8027E1D0`, allocation `0x8027E0C0`, release `0x8027E130`, and available-count `0x8027E1C0`. Matching allocation/release bodies prove player at track `0xC4`, parser at `0xCC`, parser member at allocator 4, and pool at allocator 8. `Create` passes stride `0xD0`; complete allocator size is 12. |
| Placement new | The existing canonical `InstancePool::Alloc` uses placement new. Its standard identity implementation permits the native null checks and constructor call to match the allocator exactly. The new canonical `<new>` header provides only this required interface. |
| `RemoteSpeaker` | Matching constructor at `0x8027E4D0` proves six initial flags, state/command words at `8/12/16`, callback at `0x38`, and alarms at `0x40/0x70`. The current revision omits the older force-resume flag. All eleven linked methods reproduce the native instruction streams with this header. Complete size is `0xA8`. |
| `RemoteSpeakerManager` | `GetRemoteSpeaker` at `0x8027EF00` multiplies the index by `0xA8` and adds `0x38`. The reference's four-controller array and alarm layout give complete size `0x2D8`; source callbacks prove the accessor signature. |

Wii/1.1 consumer compilations using the actual canonical headers pass. The full MML track constructor/parser dispatch and allocator bodies match after excluding relocation words. Tiny destructor/count candidates are disambiguated by native vtable entries and paired branch targets. The remote-speaker constructor and all ten other retained methods match. These diagnostic comparisons support the interfaces; the source pull request will separately establish complete splits, source selection, relocations, object comparisons, and frozen DOL acceptance.

## Baseline validation

Header baseline commit `fe9a3d1e84e78999d3cd5b6939eb0db487ed6327` was configured with `python configure.py --no-progress --build-dir build/426-header-baseline` (0). `.venv/Scripts/ninja all_source build/426-header-baseline/wpse-v1/main.dol` exited 0 before and after adding the headers. All 384 existing source-object files remained byte-identical. Explicit frozen verification of `build/426-header-baseline/wpse-v1` exited 0 before and after the change. A native-compiler layout probe also accepted sizes `0x88`, `0xCC`, `0xD0`, 12, `0xA8`, and `0x2D8` for the parser parameter block, track, MML track, allocator, speaker, and manager respectively. No source, split, symbol name, or expected hash changes are included in this prerequisite.

After rebasing onto main `c360ce6bd56074b54e709f569221f4a893585765`, the default `build/wpse-v1` directory did not exist. A fresh `python configure.py`, plain `.venv/Scripts/ninja`, and explicit `tools/verify/verify.py` verification of `build/wpse-v1` all exited 0. The plain build also generated the actual default-directory objdiff report. The full SHA-1 build check and independent frozen SHA-256/direct comparison therefore used the same fresh artifact.

## Provenance and limits

The MML and remote-speaker declarations follow CC0 `zeldaret/ss` commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, with existing canonical SDK includes and types. `RemoteSpeakerManager` follows the same interface in CC0 `kiwi515/ogws` commit `27fa94a593096eed7442efc4e70981fd52025b9c`; the accessor's native stride and offset establish its storage shape. Both root licenses and the individual source/header files were inspected; there is no overriding file-level notice. Comments, debug offsets, and speculative reference remarks were removed from code and recorded here instead. The reference's stale `SeqTrack` size comment is superseded by the actual complete layout and native accesses above. Header declarations alone claim no new reconstructed code bytes.

## Traps

- The old ogws headers disagree about the `SeqPlayer` namespace. The newer matched MML reference consistently uses `nw4r::snd::detail::SeqPlayer`; matching allocation calls and track methods use that canonical interface.
- Reference offset comments can lag source edits: the newer `MoveValue<s8,s16>` pitch bend adds four bytes relative to older debug annotations. Compile the complete type and compare real field accesses instead of adding padding to satisfy stale comments.
