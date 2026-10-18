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
| `fa/msc/puh_msc_cmd.c` | `0x80223250–0x8022362C` | one function | First-owned #373; complete external readable switch exact |
| `fa/msc/puh_msc_blk.c` | `0x80223630–0x80223E64` | first three functions in #373; eight further functions in #374 | Same agent legitimately claimed #374; complete external object exact; private unconsumed record types remain under review |

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

# F-373-2: Readable SCSI sense translator and shared block-unit proof

The independently authored SCSI sense translator is a plain C switch over the signed 24-bit combination of three input bytes. Native branch execution establishes 38 explicit code/result pairs, with a separate key `4`, additional-sense code `0x40` fallback returning 24 for every qualifier; all other codes return 39. Cases appear in result order. No reference implementation or decompiler output supplies the body. `373-probe-scsi-sense.py` checks the native-derived cases, compiles the candidate with unchanged Wii/1.0 SDK flags, and compares every byte of its complete 988-byte `.text`, function extent and absence of other allocated sections or relocations. All comparisons pass against the unchanged frozen DOL. External `373-reference/scsi-sense-evidence.json` retains the full mapping and proof.

The complete cooperating block-unit candidate contains 11 functions, 2,040 instruction bytes, 60 internal padding bytes, 32 initialized data bytes, 384 BSS bytes and four small-data BSS bytes. All section payloads, starts, extents and 66 independently resolved relocation targets match. Its first three functions contribute exactly 308 instruction bytes to #373; the remaining eight contribute 1,732 bytes to the separately claimed #374. No duplicate byte credit is assigned. Detailed consumed member offsets, whole-unit proof and source-fidelity limitations are recorded in [F-374-5](374-fa-sdi-usb.md#f-374-5-complete-original-inventory-text-proof).

The private block-device record's unconsumed ranges are still diagnostic byte views; their original types have not been recovered. The block candidate therefore remains outside tracked implementation pending interface review. The sense translator also remains external until its split-only baseline and actual source registration gates can run. Neither external result establishes linked source coverage or whole-issue completion. The original #373 inventory still includes externally owned SD tails and other unresolved units.

## Traps

- The translator's final comparison is unsigned. A signed literal in the otherwise identical readable switch changes exactly the native `cmplwi` to `cmpwi`. The unsigned literal reflects the observed comparison; its complete object then matches without compiler-flag changes.
- Constant propagation across a linear listing is insufficient for branch-derived switch facts: one branch can replace a register used as a different constant base on another path. Collect comparison facts through concrete native branch execution and iterate newly discovered boundary values before asserting an exhaustive case table.
