# F-371-1: Native PrFILE2 Wii kernel adapter

Issue #371, target `wpse-v1`. The diagnostics identify ten functions by their original `pfk_` names, from `pfk_create_task` at `0x8021DFE0` through `pfk_release_semaphore` ending at `0x8021E414`. They adapt PrFILE2 operations to existing Wii SDK threads, message queues, and semaphores. The preceding function ends at `0x8021DFDC`; the following routine at `0x8021E420` begins NAND file-backed disk handling and calls `NANDChangeDir` and `NANDOpen`.

The descriptive source file `RVL_SDK/fa/pf_kernel.c` owns `.text` `0x8021DFE0` to `0x8021E414` and `.data` `0x803C4520` to `0x803C47F0`. It has no global state or other sections. The fourteen diagnostic strings are emitted in native function order, including original spelling and capitalization. The following string at `0x803C47F0` names `pfd_st_inter_callback` and belongs to a different driver.

| Address | Size | Original function name |
| --- | --- | --- |
| `0x8021DFE0` | `0x90` | `pfk_create_task` |
| `0x8021E070` | `0x60` | `pfk_start_task` |
| `0x8021E0D0` | `0x7C` | `pfk_get_task_id` |
| `0x8021E150` | `0x70` | `pfk_create_mailbox` |
| `0x8021E1C0` | `0x68` | `pfk_receive_message` |
| `0x8021E230` | `0x70` | `pfk_send_message` |
| `0x8021E2A0` | `0x9C` | `pfk_create_semaphore` |
| `0x8021E340` | `0x40` | `pfk_delete_semaphore` |
| `0x8021E380` | `0x44` | `pfk_get_semaphore` |
| `0x8021E3D0` | `0x44` | `pfk_release_semaphore` |

All ten functions, all 1,048 instruction bytes, the padded 1,076-byte `.text` section, and all 720 `.data` bytes compare at 100% under Wii/1.0 and `cflags_sdk`, with default sixteen-byte function alignment. The first C reconstruction matches without compiler overrides. The complete source unit is registered `Matching` and linked from source.

The header uses existing `OSThread`, `OSMessageQueue`, `OSMessage`, `OSPriority`, and `OSSemaphore` declarations. Parameter structures have descriptive names; their native offsets are proven by the loads and the call signatures:

| Type and size | Fields in offset order |
| --- | --- |
| `PFK_TASK_PARAMS`, `0x18` | `entry` `0x00`, `argument` `0x04`, `stack_size` `0x08`, `stack` `0x0C`, `priority` `0x10`, `thread` `0x14` |
| `PFK_MAILBOX_PARAMS`, `0x0C` | `queue` `0x00`, `messages` `0x04`, `capacity` `0x08` |
| `PFK_SEMAPHORE_PARAMS`, `0x0C` | unsigned `initial_count` `0x00`, unsigned `maximum_count` `0x04`, `semaphore` `0x08` |

The task adapter passes stack base plus size as the thread stack top, creates an attached thread, and resumes it only when `OSIsThreadSuspended` returns one. Message receive blocks; message send does not block and rejects a null message. Semaphore creation initializes to the maximum count and waits `maximum_count - initial_count` times. Semaphore deletion validates the handle and otherwise performs no OS operation. These behaviors preserve the original; no runtime API redesign is included.

Names: high confidence for function spelling from native diagnostics and behavior from SDK call signatures; descriptive file and parameter/member names, medium spelling confidence. Source was written for this target directly from native instructions; no external implementation was copied.

Validation: split-only configure/build/frozen-manifest verification exit 0. Reconstructed source objdiff is 100% for every function and both sections. Configure, source-linked build, and frozen-manifest verification all exit 0. Verification checks the unchanged original hash and compares the rebuilt executable directly.

## Remaining issue scope

This completes ten of #371's 28 functions, not the issue. Eight filesystem wrappers at `0x8021DDD0` to `0x8021DFDC` precede the adapter; ten NAND-backed disk functions at `0x8021E420` to `0x8021F3A4` follow it. They remain automatic fallbacks and receive no reconstruction credit from this unit. Shared native source boundaries must be coordinated with #370 and #372 before their splits are accepted.

## Traps

No new trap. The original diagnostic typo `Faile to OSReceiveMessage()` is preserved as native data.
