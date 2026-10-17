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

## Reference provenance

Player heap behavior follows CC0 `zeldaret/ss` at `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/snd/snd_PlayerHeap.cpp`, adapted to existing canonical headers and the project's source conventions. The source and referenced header have no file-specific license notice overriding the repository CC0 dedication. Reference material is retained outside the checkout. No commented reference source is imported into tracked code.

Remote speaker probes use the same pinned CC0 project. Its six initial state flags, absent old force-resume flag, and resulting member offsets reproduce this target's constructor and every linked remote-speaker function. The old CC0 `kiwi515/ogws` revision `27fa94a593096eed7442efc4e70981fd52025b9c` has an additional flag and does not describe this target's layout.

## Traps

- A custom build directory does not change `config/wpse-v1/build.sha1`, whose input path remains `build/wpse-v1/main.dol`. Use the actual custom-DOL Ninja target and explicit frozen verifier for the split-only baseline; final acceptance will also execute the full default build. Never accept a stale default-path DOL as evidence for a custom output.
- The public old MML allocator headers forward-declare `SeqPlayer` in `detail`, while their sequence-track header uses `snd::SeqPlayer`. This reference inconsistency prevents compiling the combined headers without adaptation. It is not evidence that the target uses either interface; validate native calls and complete layouts before importing them.
