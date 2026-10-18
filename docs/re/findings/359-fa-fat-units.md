# F-359-1: FA FAT16 translation unit

The first-owned FAT16 routine starts at `0x8020CFC0` in #359. Three subsequent routines belong to #360; that owner approved whole-file coordination while retaining those three original inventory rows. The unit ends at `0x8020D420`, where FAT32 begins. The earlier core FAT unit begins in assigned #358 and is not taken over here.

The pinned reference IPL split inventory independently lists distinct `fa/pf_fat.c`, `pf_fat12.c`, `pf_fat16.c` and `pf_fat32.c` translation units. The four FAT16 names and native sizes match its ordered functions: ReadFATEntry276, WriteFATEntry184, WriteFATEntryWithBuf264, ReadFATEntryWithBuf368. Their target placements include28 alignment bytes, for1120 total text bytes. The source emits no other allocated sections.

The outside ordinary C candidate reproduces all four instruction streams, all14 resolved call targets and the complete1120-byte text including alignment. A strict audit preserves every non-relocated branch bit and pairs both sector operations and the exact register-save/restore helper entry points. This is a native diagnostic, not source selection or issue completion. The canonical sector function names are independently supported by their actual call sites; no target data is copied into source.

The CC0 primary reference is [TFP2 revision b62dfe78759133a85bf5ce451bcf496b87439b0c](https://github.com/koopthekoopa/TFP2/tree/b62dfe78759133a85bf5ce451bcf496b87439b0c). Its VF FAT16 implementation is a related revision, rather than a source match: target functions omit validation and use a direct PF_CACHE_PAGE pointer, with explicit mirrored FAT writes. The native read path retries through the evidenced error callback. A buffered write preserves its observed final read-error assignment after storing the two value bytes; buffered read deliberately ignores the earlier flush results, as the native flow does. Reference source/header blob IDs and SHA256 values are retained in the outside provenance manifest; no restricted MSL implementation was involved.

The complete volume callback field at0x1FA4 uses the approved pending #912 volume/character headers, copied byte-identically into an outside include overlay. Those shared declarations have not been duplicated here. This source remains outside until the proper split-only verification, NonMatching registration, source diff and Matching full executable gates pass. The rest of the23 original #359 functions remains pending; there is no closing reference.

## Commands and results

The outside candidate uses the actual Wii/1.0 pf_dir Ninja compile command, with the approved header overlay and outside output paths only. `359-probe-fat16.py` compiles successfully, and `359-strict-fat16.py` exits0 for all4 functions/1092 code bytes/1120 text bytes/14 actual targets. The split-only metadata checkpoint currently has no source registration and no completed full-build gate.

## Traps

- Casting the sixteen-bit input before applying the existing PDM_SWAP16 macro preserves native byte-reversed stores. A generic32-bit swap expression emitted an extra instruction. No new intrinsic, flag or macro was introduced.
- Related VF page routines use a pointer-to-pointer cache interface. This target directly accesses PF_CACHE_PAGE at the incoming fourth argument; importing the VF interface would change the ABI.
