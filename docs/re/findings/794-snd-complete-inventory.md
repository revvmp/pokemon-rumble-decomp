# F-794-2: Complete sound issue inventory audit

## Scope and ownership

Issue #794 was claimed through the collaboration helper after it became unassigned. Its forty functions contain 8,056 instruction bytes from 0x8027E700 through 0x80280758. This audit adds no source, metadata, or reconstructed-byte credit. It checks source delivered by PR #836 and the complete RemoteSpeaker unit owned by #426 in PR #829.

The independent candidate contains main c4e4aff7 plus the bounded #829 source dependency. The #425 and #794 owners subsequently agreed to consolidate their completion audits into source PR #829. Closure now requires all three original inventories to pass on the same committed source-selected merge candidate after its final rebase onto actual main. Earlier read-only scratch results are supplemental; they do not establish this claim's acceptance.

## Complete inventory

| Unit | Functions | Bytes | Source delivery |
| --- | ---: | ---: | --- |
| RemoteSpeaker tails | 7 | 1,840 | #829, first-function owner #426 |
| RemoteSpeakerManager | 5 | 640 | #836 |
| SeqFile | 3 | 308 | #836 |
| SeqPlayer | 25 | 5,268 | #836 |
| Total | 40 | 8,056 | Existing source; no duplicate credit |

| Native address | Size | Canonical function | Unit |
| --- | ---: | --- | --- |
| 0x8027E700 | 0x94 | `ExecCommand__Q34nw4r3snd13RemoteSpeakerFQ44nw4r3snd13RemoteSpeaker14SpeakerCommand` | RemoteSpeaker |
| 0x8027E7A0 | 0x308 | `UpdateStreamData__Q34nw4r3snd13RemoteSpeakerFPCs` | RemoteSpeaker |
| 0x8027EAB0 | 0x10C | `SpeakerOnCallback__Q34nw4r3snd13RemoteSpeakerFll` | RemoteSpeaker |
| 0x8027EBC0 | 0xE8 | `SpeakerPlayCallback__Q34nw4r3snd13RemoteSpeakerFll` | RemoteSpeaker |
| 0x8027ECB0 | 0xE8 | `SpeakerOffCallback__Q34nw4r3snd13RemoteSpeakerFll` | RemoteSpeaker |
| 0x8027EDA0 | 0x48 | `ContinueAlarmHandler__Q34nw4r3snd13RemoteSpeakerFP7OSAlarmP9OSContext` | RemoteSpeaker |
| 0x8027EDF0 | 0x70 | `IntervalAlarmHandler__Q34nw4r3snd13RemoteSpeakerFP7OSAlarmP9OSContext` | RemoteSpeaker |
| 0x8027EE60 | 0x98 | `GetInstance__Q44nw4r3snd6detail20RemoteSpeakerManagerFv` | RemoteSpeakerManager |
| 0x8027EF00 | 0x10 | `GetRemoteSpeaker__Q44nw4r3snd6detail20RemoteSpeakerManagerFi` | RemoteSpeakerManager |
| 0x8027EF10 | 0x9C | `Setup__Q44nw4r3snd6detail20RemoteSpeakerManagerFv` | RemoteSpeakerManager |
| 0x8027EFB0 | 0x44 | `Shutdown__Q44nw4r3snd6detail20RemoteSpeakerManagerFv` | RemoteSpeakerManager |
| 0x8027F000 | 0xF8 | `RemoteSpeakerAlarmProc__Q44nw4r3snd6detail20RemoteSpeakerManagerFP7OSAlarmP9OSContext` | RemoteSpeakerManager |
| 0x8027F100 | 0x6C | `__ct__Q44nw4r3snd6detail13SeqFileReaderFPCv` | SeqFile |
| 0x8027F170 | 0x10 | `GetBaseAddress__Q44nw4r3snd6detail13SeqFileReaderCFv` | SeqFile |
| 0x8027F180 | 0xB8 | `ReadOffsetByLabel__Q44nw4r3snd6detail13SeqFileReaderCFPCcPUl` | SeqFile |
| 0x8027F240 | 0x4C | `InitSeqPlayer__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x8027F290 | 0x148 | `__ct__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x8027F3E0 | 0x188 | `__dt__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x8027F570 | 0x120 | `InitParam__Q44nw4r3snd6detail9SeqPlayerFiPQ44nw4r3snd6detail14NoteOnCallback` | SeqPlayer |
| 0x8027F690 | 0x294 | `Setup__Q44nw4r3snd6detail9SeqPlayerFPQ44nw4r3snd6detail17SeqTrackAllocatorUliPQ44nw4r3snd6detail14NoteOnCallback` | SeqPlayer |
| 0x8027F930 | 0x7C | `SetSeqData__Q44nw4r3snd6detail9SeqPlayerFPCvl` | SeqPlayer |
| 0x8027F9B0 | 0x70 | `Start__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x8027FA20 | 0x150 | `Stop__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x8027FB70 | 0x90 | `Pause__Q44nw4r3snd6detail9SeqPlayerFb` | SeqPlayer |
| 0x8027FC00 | 0xC0 | `Skip__Q44nw4r3snd6detail9SeqPlayerFQ54nw4r3snd6detail9SeqPlayer10OffsetTypei` | SeqPlayer |
| 0x8027FCC0 | 0x8 | `SetChannelPriority__Q44nw4r3snd6detail9SeqPlayerFi` | SeqPlayer |
| 0x8027FCD0 | 0x8 | `SetReleasePriorityFix__Q44nw4r3snd6detail9SeqPlayerFb` | SeqPlayer |
| 0x8027FCE0 | 0xC | `SetSeqUserprocCallback__Q44nw4r3snd6detail9SeqPlayerFPFUsPQ34nw4r3snd24SeqUserprocCallbackParamPv_vPv` | SeqPlayer |
| 0x8027FCF0 | 0x98 | `CallSeqUserprocCallback__Q44nw4r3snd6detail9SeqPlayerFUsPQ44nw4r3snd6detail8SeqTrack` | SeqPlayer |
| 0x8027FD90 | 0x1BC | `InvalidateData__Q44nw4r3snd6detail9SeqPlayerFPCvPCv` | SeqPlayer |
| 0x8027FF50 | 0x20 | `GetPlayerTrack__Q44nw4r3snd6detail9SeqPlayerFi` | SeqPlayer |
| 0x8027FF70 | 0x12C | `ParseNextTick__Q44nw4r3snd6detail9SeqPlayerFb` | SeqPlayer |
| 0x802800A0 | 0x3C | `GetVariablePtr__Q44nw4r3snd6detail9SeqPlayerFi` | SeqPlayer |
| 0x802800E0 | 0x10C | `Update__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x802801F0 | 0x264 | `UpdateTick__Q44nw4r3snd6detail9SeqPlayerFi` | SeqPlayer |
| 0x80280460 | 0x294 | `SkipTick__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x80280700 | 0x28 | `NoteOn__Q44nw4r3snd6detail9SeqPlayerFiRCQ44nw4r3snd6detail10NoteOnInfo` | SeqPlayer |
| 0x80280730 | 0x4 | `ChannelCallback__Q44nw4r3snd6detail9SeqPlayerFPQ44nw4r3snd6detail7Channel` | SeqPlayer |
| 0x80280740 | 0x8 | `@180@__dt__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |
| 0x80280750 | 0x8 | `@192@__dt__Q44nw4r3snd6detail9SeqPlayerFv` | SeqPlayer |

## Independent pipeline evidence

A fresh default output directory was built through configure.py --map and plain Ninja; both exited 0. Explicit frozen verification passed on the actual build/wpse-v1/main.dol, and direct immutable DOL equality passed. SHA-256 remains ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. Scope is the main executable only.

The freshly retrieved claimed issue supplies the authoritative inventory. Each row passed checks for native metadata size, exactly one owning split, compiled source function and extent, final ELF native address and extent, actual linker-map source contribution, and actual Ninja source input without the corresponding original fallback. All four whole source objects have 100% text comparisons against their original split objects. No function is accepted from a neighboring address or partial unit score.

[F-794-1](794-snd-remotespeakermanager-seqfile-seqplayer.md) and [F-426-1](426-snd-mml-playerheap-remote.md) document boundaries, fidelity and original source provenance. Existing unit ledger rows retain all implementation credit. The audit row records coverage only.

## Traps

A complete scratch audit did not authorize closing an assigned issue. The helper initially refused #794 while assigned. Its later legitimate helper claim, independent worktree, fresh complete inventory, actual source-selected build and final consolidated committed-head gate are required for closure. Frozen executable identity alone permits original fallbacks, so every inventory function separately requires the real source input and map contribution.
