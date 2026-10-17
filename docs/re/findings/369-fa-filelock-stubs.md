# F-369-1: Shared file-lock native proof and remaining stub inventory

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #369 owns 22 original survey functions and 3,624 native code bytes. This initial result proves two of those functions in an external compiler probe; it does not claim source selection, full issue completion or a passing whole-tree build after the header change.

## Ownership and inventory

The issue was open and unassigned when claimed through `tools/collab/claim_issue.py`. Its earlier assignment had been removed; the helper accepted the fresh claim. The first source file, `pf_filelock.c`, begins with #368's `PF_InitLockFile`, so #368 remains the split, object and header owner. The same worker owns both claims and can supply the two subsequent bodies without taking an assigned contributor's work. Earlier findings and failed revisions remain preserved.

| Target | Native extent | Current evidence |
| --- | --- | --- |
| `0x8021C350` | `0xBC` | complete `PF_LockFile` scratch instructions and resolved relocations exact |
| `0x8021C410` | `0x90` | complete `PF_UnLockFile` scratch instructions and resolved relocations exact |
| `0x8021C4A0` | `0xB0` | original `pfstub_init_stub` identity and typed work-area consumers established |
| `0x8021C550` | `0x164` | worker entry, further reconstruction pending |
| `0x8021C6C0` | `0xE4` | command submission, further reconstruction pending |
| `0x8021C7B0` | `0xC4` | result storage, further reconstruction pending |
| `0x8021C880` | `0x4` | system initializer tail-call, reconstruction pending |
| `0x8021C890` | `0x18` | current-context getter, reconstruction pending |
| `0x8021C8B0` | `0x7C` | timestamp, reconstruction pending |
| `0x8021C930` | `0x584` | standard command dispatcher, reconstruction pending |
| `0x8021CEC0` | `0x44` | command 27, identity and record ABI audit pending |
| `0x8021CF10` | `0x50` | command 16, identity and record ABI audit pending |
| `0x8021CF60` | `0x4C` | command 11, identity and record ABI audit pending |
| `0x8021CFB0` | `0x3C` | command 0, identity and record ABI audit pending |
| `0x8021CFF0` | `0x4C` | command 41, identity and record ABI audit pending |
| `0x8021D040` | `0x48` | command 19, identity and record ABI audit pending |
| `0x8021D090` | `0x4C` | command 2, identity and record ABI audit pending |
| `0x8021D0E0` | `0x3C` | command 28, identity and record ABI audit pending |
| `0x8021D120` | `0x3C` | command 29, identity and record ABI audit pending |
| `0x8021D160` | `0x3C` | command 4, identity and record ABI audit pending |
| `0x8021D1A0` | `0x50` | command 20, identity and record ABI audit pending |
| `0x8021D1F0` | `0x4` | narrow initialization tail-call, reconstruction pending |

## Typed shared-file lock

The previous canonical `PF_SFD` stored its final 32 bytes as `u8 lock[0x20]`. The bounded declaration replaces that area with the ordinary typed lock record below and a trailing handler count. The complete surrounding `PF_SFD` stays `0x29C`; no volume stride, storage definition or object extent changes.

| Member | Offset within `PF_SFD` | Native evidence |
| --- | --- | --- |
| `lock.mode`, `u16` | `0x27C` | `0x8020FA38` exports its low two bits as file-info lock mode |
| `lock.count`, `u16` | `0x27E` | both lock functions read it; file-info exports it at `0x8020FA5C` |
| `lock.wcount`, `u32` | `0x280` | unlock tests zero; waiting lock path updates it at `0x80210440–0x80210448` |
| `lock.owner`, `PF_FILE*` | `0x284` | `0x8020FA48` exports lock owner; `0x802103E4` compares it to the incoming file handle |
| `lock.resource`, `OSSemaphore*` | `0x288` | create result is stored here, then passed to get, release and delete wrappers |
| `lock.semaphore`, `OSSemaphore` | `0x28C` | its address is supplied to the actual kernel create wrapper |
| `num_handlers`, `u16` | `0x298` | initialization clears it at `0x8020EBC4`; file open increments it at `0x80210814–0x80210820` |

The lock record has size `0x1C`. The handler count follows it, and natural trailing alignment preserves the original 32-byte area. Existing canonical `OSSemaphore` in `OSThread.h` is 12 bytes. Native `pfk_create_semaphore` at `0x8021E2A0` passes the supplied storage directly to `OSInitSemaphore` at `0x802BB8E0`, then waits the difference between maximum and initial counts. This establishes the storage type independently of its pointer alias. The delete wrapper at `0x8021E340` validates the pointer without destroying storage; get and release at `0x8021E380` and `0x8021E3D0` call the actual SDK wait and signal functions.

Member names `mode`, `count`, `wcount`, `owner` and `resource` also agree with CC0 Petari's VF `PF_LOCK` in `vf_struct.h`, pinned to `e0ab141914921726a595573dc48d3f13d0922efe`. This target extends that related record with embedded semaphore storage; the VF layout is not copied wholesale. Both lock function names and all referenced kernel helper names occur verbatim in the independent FA map from CC0 Wii IPL `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`.

## External compiler validation

The actual committed canonical header was used first in the include search, with only #368's approved typed system interface supplied as a temporary fallback. All three file-lock functions, including #368's initializer, have their exact native extents and all 352 native instruction bytes after independently resolving every relocation to its actual target. The object has no allocated data. The two #369 functions account for 332 of the original 3,624 issue bytes in this scratch result. They are not linked source credit.

`369-audit-filelock.py` compares the entire instructions and resolved targets, rejecting differences rather than normalizing or masking them. `369-lock-layout.c` measures the actual canonical type sizes and offsets, producing these 12 words: `29C,27C,C,30,2C,1C,27E,280,284,288,28C,298`. These establish surrounding `PF_SFD`, `PF_FILE` and semaphore sizes as well as every proposed lock field. Temporary sources, object files and evidence JSON remain outside the repository under issue-numbered paths.

The full configure, plain Ninja and explicit frozen verifier gate remains pending while the active shared merge queue finishes. The header owner for #363 will integrate the bounded declaration with its own independent additions and check existing consumers before publication. No source, split or object registration has been added by #369 at this stage.

## Traps

- A body that ignores a register cannot establish its public arity. Actual callers at `0x80210430` and `0x80210480` explicitly pass `NULL` and the file handle, respectively, as a second owner argument to `PF_LockFile`. Its target contract is `s32 PF_LockFile(PF_FILE* p_file, PF_FILE* p_owner)` even though the body never reads the second parameter. The related VF one-argument header is another revision. Recompiling the ordinary two-argument source preserves all 352 file-lock bytes and independently resolved relocations. Native `PF_UnLockFile` callers at `0x8021053C`, `0x80210CCC` and `0x80210D10` supply only the file handle. The bounded native caller audit is `369-filelock-callers.py`; no tracked lock-function prototype or body preceded this correction.
- Target asynchronous wrapper additions change the standard-stub sequence relative to the pinned FA map. A size-order import would incorrectly name `0x8021CFB0`, which submits command 0 from a supplied command record. Map each command through the actual dispatcher to the narrow callee and verify its public argument contract before assigning an original name.
- The related VF lock record lacks this target's embedded semaphore. Preserve target `PF_SFD` size `0x29C` and all surrounding offsets; native kernel storage calls, rather than a matching pointer value, establish the semaphore type.

# F-369-2: Complete system-unit scratch match

The complete system unit now compiles externally with exact native extents and independently resolved instruction bytes: `PFSYS_initializeSYS` at `0x8021C880`, 4 bytes; `PFSYS_GetCurrentContextID` at `0x8021C890`, 24 bytes; and `PFSYS_TimeStamp` at `0x8021C8B0`, 124 bytes. The same object emits `pf_sys_set` as exactly `0x23A0` NOBITS bytes aligned to 8, using the typed layout independently proven in F-368-2. No repository storage definition has been added.

Initialization tail-calls `pfstub_init_stub`. The getter copies `pf_vol_set.current_context_id` from offset `0x64` through its output pointer and returns zero. This field is independently stored by the actual worker at `0x8021C5B4`; #363's volume-set investigation also confirms its relation to the context table and volume array. That one proven member is added only in the external probe's temporary interface, pending canonical owner integration.

The timestamp directly calls `OSGetTime` and `OSTicksToCalendarTime`, then copies year, month plus one, day, hour, minute and second, and sets `sys_ms` to 1. Native stores establish 32-bit fields in both `PF_SYS_DATE` and `PF_SYS_TIME`. The related VF source uses the same member names but 16-bit fields and a separate time helper; copying that header would change both the ABI and instructions. The external source instead uses the native-proven fields and actual canonical SDK calendar type.

`369-audit-system.py` checks every instruction, all independently resolved relocation targets, absence of additional allocated sections, and exact BSS size/alignment. With the two lock functions, this gives five of the original 22 issue functions and 484 of 3,624 native bytes in external exact probes. The preceding #368 initializer adds another 20 bytes to the complete shared file-lock proof. None of these probes counts as linked source coverage; source materialization, canonical integration, actual Ninja objects and full frozen verification remain pending.

## Dispatcher identity audit

`369-dispatch-map.py` reads all 81 entries of the actual jump table at `0x803C4318` and resolves each case's direct call to the verified narrow-wrapper inventory. This independently distinguishes command identity from public-stub order: command 0 calls `pf2_create`, 11 calls `pf2_chdir`, 20 calls `pf2_devinf`, 28 calls `pf2_fsfirst`, 29 calls `pf2_fsnext`, and 4 calls `pf2_fwrite`. Consequently the wrappers at `0x8021CF60`, `0x8021CFB0`, `0x8021D0E0`, `0x8021D120` and `0x8021D160` cannot receive the names suggested by blindly following the older FA map's stub sequence. The saved complete table is external `369-reference/standard-dispatch-map.json`; public record ABI and name confidence must be established before source publication.

# F-369-5: Split baseline and independent system source

The 15 first-owned units are now split at their actual retained text ends: the complete stub unit, system unit, standard dispatcher and twelve public stubs. The dispatcher owns exactly 324 bytes of initialized jump-table data; the system owns the unchanged `pf_sys_set` extent `0x8041F170–0x80421510`. This issue does not take the first-owned #368 file-lock split. Function names retain every original size and attribute. Split-only configure, plain Ninja and explicit frozen verification all exited 0 on `3ef0795206aa30f17395ca712dab4f8135afbf8b`; dtk's sole rewrite normalizes whitespace on the new dispatcher data row, retained in `410eef5`.

Only the independently proven `pf_system.c` is initially registered and materialized `NonMatching`. Its complete date/time types live in the canonical system header. The header temporarily includes the exact approved typed system-set declarations from pending #368; those duplicates must disappear after canonical integration. The owner of #363 explicitly approved the additive signed `current_context_id` member after `setting` in the existing volume-set prefix, at offset `0x64`, retaining every preceding member and defining no volume storage or array. Both the native getter and worker independently establish that field. The system source defines only its own native-proven `0x23A0` system object.

The uncertain operation qualifier and asynchronous outer record are absent from tracked source and headers. The complete stub and other newly split units still use original fallback; no special inlining flags are introduced. On committed `288f006c`, the actual system source built as `NonMatching` with configure, plain Ninja and explicit frozen verification all exiting 0. `369-audit-system-actual.py` checks all three actual Ninja functions, 152 native instruction bytes, all 172 text bytes including padding, every relocation offset/kind/name/addend, every allocated objdiff section at 100%, exact `0x23A0` BSS/alignment 8, and full unchanged DOL identity. The system unit is eligible for source selection; its following source-selected complete build gate remains pending. This is partial #369 work; no whole-issue completion is claimed.
