# F-373-1: USB source boundaries and native callback bridge

Issue #373 owns all 28 original rows from `0x80220890` through `0x80223774`, totaling 11,868 retained instruction bytes. This preparation is partial: two callback-bridge functions have an exact external source candidate, while the original objects still supply every function to the executable. No source registration, source selection or complete-issue acceptance is claimed.

## Independent file evidence

The complete symbol map and `config/43U/splits.txt` from CC0 Wii IPL, pinned to `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, distinguish these original files. Their physical target boundaries also follow native function ends and default SDK alignment. Related file spans support unit identity; the target's actual section sizes still require independent proof.

| Original source suffix | Target text range | Issue contribution | Status |
| --- | --- | --- | --- |
| `fa/driver/sd_drv.c` | starts `0x8021FFC0`; last SD function ends `0x80222E84` | 17 functions in #373; first function belongs to external #372 | Preserve #372 ownership and pending PR #931; no shared split/header edits here |
| `fa/driver/pfd_cmn.c` | `0x80222E90–0x80222FA8` | one function, `pfd_get_media_drv_char` | First-owned #373; external investigation pending |
| `fa/driver/msc_drv.c` | `0x80222FB0–0x80222FC4` | two functions, 20 bytes | First-owned #373; split prepared; complete external candidate exact |
| `fa/msc/puh_msc.c` | `0x80222FD0–0x80223248` | four functions | First-owned #373; native interfaces under investigation |
| `fa/msc/puh_msc_cmd.c` | `0x80223250–0x8022362C` | one function | First-owned #373; complete SCSI sense switch under investigation |
| `fa/msc/puh_msc_blk.c` | `0x80223630–0x80223E64` | first three functions in #373; eight further functions in #374 | First-owned #373; preserve unclaimed #374 body scope until legitimate ownership or cooperation |

The old reference includes `usbh_msc_set_thread_priority` between write and command submission; this target does not. Native code and actual issue rows establish the target inventory, rather than importing the old sequence wholesale. The separate SD common helper is not part of the older `sd_drv.c` span. Pending PR #931 currently includes it in the proposed SD split; that difference is recorded for coordination before any shared-boundary edit.

## Exact two-function candidate

`pfd_mscdrv_registar_callback` at `0x80222FB0`, 16 bytes, saves the insertion and ejection callbacks and returns zero. Preserve the original `registar` spelling. `pfd_mscdrv_is_media_insert` at `0x80222FC0`, four bytes, forwards its disk pointer to `uhf_msc_is_media_insert` at `0x80223C70`. The latter body belongs to #374; only its independently evidenced name is added here, preserving its size, type and split.

The callback type is `void (*)(s8 drive)`, supported by the actual FA registration and notification callers. The two local pointer objects occupy exactly eight `.sbss` bytes: `g_attach_func` at `0x804B9268` and `g_detach_func` at `0x804B926C`. Their original names and local scope are corroborated by the reference map. The proposed split stops at `0x80222FC4`, before the following twelve alignment bytes, and at `.sbss` `0x804B9270` without taking the next unit's state.

`373-probe-msc-driver.py` compiles the complete independently authored C candidate outside the repository, using the actual canonical `PDM_DISK` declaration, Wii/1.0 and unchanged SDK flags. It compares all 20 native text bytes after independently resolving both SDA stores and the real tail-branch destination; it also checks the complete eight-byte small-data section and each local symbol offset. The immutable original DOL hash is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Evidence is retained outside the repository in `373-reference/msc-driver-evidence.json`.

This commit prepares only the split and names. The split-only actual configure/plain Ninja/explicit frozen verifier gate is pending the existing build queue. The candidate remains outside tracked source until that baseline passes. Subsequent actual source objects, paired relocations, complete sections, source-input/MAP/final-ELF proof and the full frozen build are required before source selection.

## Traps

- Text and zero-filled section sizes can hide swapped local objects. Declaring insertion before ejection emitted their small-data symbols in reversed order under this compiler. The ordinary reversed declaration order restores insertion at zero and ejection at four; the audit checks both real offsets and the resolved SDA targets. This is an observed property of this candidate, not a universal declaration-order rule.
- Original source spans separate the common SD helper and the two-function MSC bridge from the neighboring driver units. A block's contiguous addresses or an earlier broad proposed split do not establish one original source file. Preserve the first owner's work and coordinate a bounded correction with file-map and native evidence.
- Preserve recovered misspellings: both SD and MSC registration names use `registar_callback`. Earlier descriptive `set_callback` spellings in external #370 probes were rejected before tracked implementation publication; corrected names retain all native instructions.
