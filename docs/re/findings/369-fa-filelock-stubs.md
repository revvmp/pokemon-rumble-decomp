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
| `0x8021D1A0` | `0x50` | command identity and record ABI audit pending |
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

- Target asynchronous wrapper additions change the standard-stub sequence relative to the pinned FA map. A size-order import would incorrectly name `0x8021CFB0`, which submits command 0 from a supplied command record. Map each command through the actual dispatcher to the narrow callee and verify its public argument contract before assigning an original name.
- The related VF lock record lacks this target's embedded semaphore. Preserve target `PF_SFD` size `0x29C` and all surrounding offsets; native kernel storage calls, rather than a matching pointer value, establish the semaphore type.
