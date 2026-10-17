# F-426-1: Player heap and remote speaker native units

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`, original `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Issue #426 owns 29 functions and 1,524 code bytes at `0x8027E060–0x8027E6F4`.

## Player heap boundary and baseline

`snd_PlayerHeap.cpp` owns `.text` `0x8027E2B0–0x8027E4CC` and `.data` `0x803C6598–0x803C65A8`. Its eight native functions are the constructor, destructor, `Create`, `Alloc`, `Clear`, `GetFreeSize`, `AttachSound`, and `DetachSound`. The vtable at `0x803C6598` points to the destructor at `0x8027E2E0` and allocator at `0x8027E3E0`.

Split-only validation used `python configure.py --no-progress --build-dir build/426-playerheap-baseline` (0), `.venv/Scripts/ninja build/426-playerheap-baseline/wpse-v1/main.dol` (0), and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/426-playerheap-baseline/wpse-v1` (0). The first unrestricted Ninja invocation linked the actual custom DOL successfully but exited 1 at the hardcoded default-path SHA-1 check; the explicit custom-DOL target and independent frozen verifier completed successfully. No expected hash was changed.

The existing canonical `snd_PlayerHeap.h` expresses the observed complete 32-byte layout: vtable at 0, sound at 4, player at 8, heap start at 12, heap end at 16, allocation cursor at 20, and list node at 24. Native `Create`, `Alloc`, `Clear`, attachment functions, and constructor stores establish these offsets. The native `Clear` mutex access agrees with existing `SoundThread` at offset `0x354`.

Wii/1.1 and existing `cflags_nw4r` reproduce all eight functions and the complete vtable: objdiff reports `.text` and `.data` 100%. The source-selected custom-DOL Ninja target exited 0 and the explicit frozen verifier exited 0. The final ELF retains every native function address and extent. `Destroy` is inlined into the destructor and has no separately retained native body. Unused weak helper bodies are discarded by the linker.

## MML sequence track baseline

The complete `snd_MmlSeqTrack.cpp` owns `.text` `0x8027E000–0x8027E0B8` and `.data` `0x803C6570–0x803C6580`: constructor, parser dispatch, and destructor. The first two functions belong to #425; the destructor belongs to #426. The user's explicit whole-file approval is recorded in [F-426-2](426-snd-shared-interfaces.md), whose canonical interfaces merged in PR #754. A fresh default-directory `python configure.py`, plain `.venv/Scripts/ninja`, and explicit frozen verification all exited 0 with this split and the previously accepted player heap source. The vtable pairs the destructor and parser dispatch with their native addresses; constructor and destructor calls identify the sequence-track base interfaces.

The source is now `Matching`: complete object `.text` and `.data` are 100% in objdiff. The full source-selected default Ninja build exited 0 and its frozen verification exited 0. The final ELF has all three native addresses and extents, totaling 168 function bytes, including #426's 88-byte destructor. The constructor and parser dispatch from #425 total 80 bytes and are explicitly credited to that neighboring issue. No other #425 unit is claimed complete.

## MML track allocator acceptance

The complete allocator owns `.text` `0x8027E0C0–0x8027E210` and `.data` `0x803C6580–0x803C6598`. Its six retained functions total 300 instruction bytes: allocation, release, pool creation, pool destruction, available-count accessor, and virtual destructor. The original vtable pairs all four virtual slots with those native addresses. `Create` passes the proven `0xD0` track stride; allocation initializes the player and parser through the canonical track interface. `Destroy` and available-count accessors have paired native tail-branch destinations, distinguishing their otherwise similar tiny bodies.

The committed split-only baseline passed default configure, plain Ninja, and explicit frozen verification. The source was registered `NonMatching` and compiled with the established Wii/1.1 `cflags_nw4r`; complete object `.text` and `.data` compare at 100%. Selecting that source passed default Ninja, its actual SHA-1 check, and explicit frozen verification. All six functions retain their native final ELF addresses and extents. This accepts the complete unit as `Matching`, with no original-object dependency inside its split.

## Retained NAND stream method cluster

The `.text` split `0x8027E210–0x8027E2A8` contains ten eight-byte `ut::NandFileStream` methods and their natural function alignment. Their native class identity, vtable slots, RTTI reference, original reference order, and the remaining host-unit hypothesis are established in [F-426-3](426-snd-archive-interfaces.md). The original host name `snd_NandSoundArchive.cpp` remains a hypothesis; the retained method identities and native source coverage are independently proven.

The committed original-object baseline passed the actual default DOL target and explicit frozen verification. A faithful pinned archive source context naturally emits all ten weak bodies without forcing helpers or copied instructions. The two unretained nested stream constructor definitions are omitted as documented in F-426-3; no invented SDK overload is added. Other archive and nested stream methods remain faithful reference context and receive no source credit.

The `NonMatching` candidate compiled with the canonical headers and Wii/1.1 `cflags_nw4r`. Complete original-object `.text` and all ten function comparisons are 100%. Selecting the source and generating an actual linker map passed the default DOL target and explicit frozen verification. The map attributes each of the ten native address/extent entries to `snd_NandSoundArchive.o`, with source selection independently established by the Ninja link inputs. It marks the archive's strong functions and its other nested stream functions `UNUSED`. All ten final ELF addresses and eight-byte extents are exact, totaling 80 credited bytes. This accepts the retained split as `Matching`; no original-object definition remains inside that split.

## Remote speaker acceptance

The complete `snd_RemoteSpeaker.cpp` owns `.text` `0x8027E4D0–0x8027EE60`, with eleven retained functions / 2,384 instruction bytes. The constructor and the setup, shutdown, and update routines belong to #426 and total 544 bytes. The seven remaining functions total 1,840 bytes; they formerly belonged to #427 and now appear in the successor issue #794. First-function ownership keeps the complete source file and split in #426. This work supplies those seven overlapping functions. Main PR836 separately supplies the manager, sequence-file, and sequence-player units; their owner retains issue #794 and its closure assessment. A complete read-only inventory audit can supplement the seven supplied tails without claiming those other files. The complete `0xA8` speaker interface and the native alarm, command, codec, and callback accesses are established in F-426-2.

The committed split-only baseline passed the actual default DOL target and explicit frozen verification. The `NonMatching` candidate compiled against the canonical headers, and complete object `.text` plus all eleven retained functions compare at 100%. Selecting the source passed the actual DOL target and explicit frozen verification. The final ELF and actual linker map place all eleven functions at their original addresses and extents in the source object. Additional inlined or unretained helper definitions receive no separate credit.

## Complete issue inventory and source selection

The independent audit checks all five complete original/source object pairs at 100%, all 38 retained native addresses and extents, actual Ninja source inputs, and every source-object contribution in the actual linker map. The whole source-selected DOL equals the immutable original. The original issue contains exactly 29 functions / 1,524 bytes; the five complete units contain 38 functions / 3,444 bytes including the explicitly identified neighboring coverage. The final latest-main default configure, plain Ninja/report, frozen verification, and exact verified head are recorded in the pull request.

| Address | Size | Source unit | Original issue function |
| --- | --- | --- | --- |
| `0x8027E060` | `0x58` | `MmlSeqTrack` | `__dt__Q44nw4r3snd6detail11MmlSeqTrackFv` |
| `0x8027E0C0` | `0x68` | `MmlSeqTrackAllocator` | `AllocTrack__Q44nw4r3snd6detail20MmlSeqTrackAllocatorFPQ44nw4r3snd6detail9SeqPlayer` |
| `0x8027E130` | `0x68` | `MmlSeqTrackAllocator` | `FreeTrack__Q44nw4r3snd6detail20MmlSeqTrackAllocatorFPQ44nw4r3snd6detail8SeqTrack` |
| `0x8027E1A0` | `0xC` | `MmlSeqTrackAllocator` | `Create__Q44nw4r3snd6detail20MmlSeqTrackAllocatorFPvUl` |
| `0x8027E1B0` | `0x8` | `MmlSeqTrackAllocator` | `Destroy__Q44nw4r3snd6detail20MmlSeqTrackAllocatorFPvUl` |
| `0x8027E1C0` | `0x8` | `MmlSeqTrackAllocator` | `GetAllocatableTrackCount__Q44nw4r3snd6detail20MmlSeqTrackAllocatorCFv` |
| `0x8027E1D0` | `0x40` | `MmlSeqTrackAllocator` | `__dt__Q44nw4r3snd6detail20MmlSeqTrackAllocatorFv` |
| `0x8027E210` | `0x8` | `NandSoundArchive` | `GetBufferAlign__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E220` | `0x8` | `NandSoundArchive` | `GetSizeAlign__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E230` | `0x8` | `NandSoundArchive` | `GetOffsetAlign__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E240` | `0x8` | `NandSoundArchive` | `CanCancel__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E250` | `0x8` | `NandSoundArchive` | `CanWrite__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E260` | `0x8` | `NandSoundArchive` | `CanRead__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E270` | `0x8` | `NandSoundArchive` | `CanSeek__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E280` | `0x8` | `NandSoundArchive` | `CanAsync__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E290` | `0x8` | `NandSoundArchive` | `IsBusy__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E2A0` | `0x8` | `NandSoundArchive` | `GetRuntimeTypeInfo__Q34nw4r2ut14NandFileStreamCFv` |
| `0x8027E2B0` | `0x30` | `PlayerHeap` | `__ct__Q44nw4r3snd6detail10PlayerHeapFv` |
| `0x8027E2E0` | `0xC4` | `PlayerHeap` | `__dt__Q44nw4r3snd6detail10PlayerHeapFv` |
| `0x8027E3B0` | `0x30` | `PlayerHeap` | `Create__Q44nw4r3snd6detail10PlayerHeapFPvUl` |
| `0x8027E3E0` | `0x30` | `PlayerHeap` | `Alloc__Q44nw4r3snd6detail10PlayerHeapFUl` |
| `0x8027E410` | `0x88` | `PlayerHeap` | `Clear__Q44nw4r3snd6detail10PlayerHeapFv` |
| `0x8027E4A0` | `0x10` | `PlayerHeap` | `GetFreeSize__Q44nw4r3snd6detail10PlayerHeapCFv` |
| `0x8027E4B0` | `0x8` | `PlayerHeap` | `AttachSound__Q44nw4r3snd6detail10PlayerHeapFPQ44nw4r3snd6detail10BasicSound` |
| `0x8027E4C0` | `0xC` | `PlayerHeap` | `DetachSound__Q44nw4r3snd6detail10PlayerHeapFPQ44nw4r3snd6detail10BasicSound` |
| `0x8027E4D0` | `0x80` | `RemoteSpeaker` | `__ct__Q34nw4r3snd13RemoteSpeakerFv` |
| `0x8027E550` | `0xC0` | `RemoteSpeaker` | `Setup__Q34nw4r3snd13RemoteSpeakerFPFll_v` |
| `0x8027E610` | `0xAC` | `RemoteSpeaker` | `Shutdown__Q34nw4r3snd13RemoteSpeakerFPFll_v` |
| `0x8027E6C0` | `0x34` | `RemoteSpeaker` | `Update__Q34nw4r3snd13RemoteSpeakerFv` |

## Reference provenance

The player heap, MML allocator, NAND archive context, and remote-speaker source follow CC0 `zeldaret/ss` at `57a709b2128eb163a18e2969d561c9abb7d1fcee`, the corresponding `src/nw4r/snd/` units, adapted to existing canonical headers and the project's source conventions. The source and referenced header have no file-specific license notice overriding the repository CC0 dedication. Reference material is retained outside the checkout. No commented reference source is imported into tracked code.

Remote speaker probes use the same pinned CC0 project. Its six initial state flags, absent old force-resume flag, and resulting member offsets reproduce this target's constructor and every linked remote-speaker function. The old CC0 `kiwi515/ogws` revision `27fa94a593096eed7442efc4e70981fd52025b9c` has an additional flag and does not describe this target's layout.

## Entry-aware restack verification

After the entry-aware metadata restack, a fresh build/426-source-entry-driver directory was configured with a linker map. The actual all_source and DOL targets exited 0. Explicit frozen verification and the independent source audit exited 0: all five complete objects retain 100% in every original section, all 29 owned functions / 1,524 bytes and 38 complete-unit functions have their native ELF addresses and extents, actual Ninja and linker-map inputs select these source objects, and the complete immutable DOL is byte-identical.

A separate read-only neighboring-inventory audit uses the forty address/size rows from #794. Each row has the corresponding source object, exact native ELF function address/extent, 100% object .text, and an actual source contribution in the linker map, with whole-DOL identity. This does not claim the other source files, transfer that issue's ownership, or close it.

| #794 coverage | Functions | Code bytes | Contribution |
| --- | --- | --- | --- |
| RemoteSpeaker tails | 7 | 1,840 | Supplied by this complete first-owned file |
| RemoteSpeakerManager | 5 | 640 | Already supplied by main PR836 |
| SeqFileReader | 3 | 308 | Already supplied by main PR836 |
| SeqPlayer | 25 | 5,268 | Already supplied by main PR836 |
| Total | 40 | 8,056 | Read-only coverage audit; issue remains with its owner |

Header PR817 is now merged. The earlier custom-directory proof verifies the reviewed source restack without reusing an old default output. Final consolidated acceptance repeats the complete default-directory pipeline and all three original inventories after rebasing onto actual main.

## Consolidated issue inventories

The helper later legitimately claimed #794 after it became unassigned, and its independent isolated build and complete forty-function audit are recorded in [F-794-2](794-snd-complete-inventory.md). The separate #425 owner supplied the independent original inventory and data evidence in [F-425-2](425-complete-inventory.md). Both owners agreed to retain these bounded audits in PR #829 and close the three issues only after the same final committed tree passes every inventory.

The combined original scope is ninety-nine survey rows / 14,588 bytes through ninety-eight real native functions: #425 has thirty rows / 5,008 bytes through twenty-nine functions, #426 has twenty-nine functions / 1,524 bytes, and #794 has forty functions / 8,056 bytes. The #425 survey row at 0x8027CCA4 is the trailing unreachable instruction of Seek at 0x8027CC60 and is checked as its own subextent without counting another native function. New source-byte credit remains the five independently reconstructed complete units; the audits add no duplicate implementation credit.

## Traps

- A custom build directory does not change `config/wpse-v1/build.sha1`, whose input path remains `build/wpse-v1/main.dol`. Use the actual custom-DOL Ninja target and explicit frozen verifier for the split-only baseline; final acceptance will also execute the full default build. Never accept a stale default-path DOL as evidence for a custom output.
- The public old MML allocator headers forward-declare `SeqPlayer` in `detail`, while their sequence-track header uses `snd::SeqPlayer`. This reference inconsistency prevents compiling the combined headers without adaptation. It is not evidence that the target uses either interface; validate native calls and complete layouts before importing them.

The entry-aware metadata merge driver correctly combined all ten source commits without conflicts, but its object insertion point placed the five new units before the newly merged MemorySoundArchive/MidiSeqPlayer/MmlParser entries. Review address order even after an automatic entry merge; only the five owned Object lines were relocated after MmlParser, preserving every main entry. This is a concrete insertion-context finding, not evidence that all automatic merges misorder entries.

## Shared-reference review

Worktrees share origin/main. During the final gate, another fetch advanced
that reference from the pinned 2c68fcf9 rebase base to 76b5be1b, which newly
named HBM startTrigEvent at 0x80183E50 and HomeButtonEventHandler::onEvent at
0x80185B00. Comparing the candidate against the moving reference initially
misidentified those upstream additions as old-commit name reversions.
Two candidate builds were stopped and acceptance restarted; no failed or
interrupted build was accepted. Rebasing onto 76b5be1b inherited both names
naturally, leaving exactly six owned sound entries different from main.
Pin the actual base used for a rebase when reviewing preservation, then
perform the separate fresh-fetch ancestor gate for newly arrived main.
This avoids confusing upstream additions with changes in the issue stack.
