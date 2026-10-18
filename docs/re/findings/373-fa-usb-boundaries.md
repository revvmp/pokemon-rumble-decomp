# F-373-1: USB source boundaries and native callback bridge

Issue #373 owns all 28 original rows from `0x80220890` through `0x80223774`, totaling 11,868 retained instruction bytes. This preparation is partial: two callback-bridge functions have an exact external source candidate, while the original objects still supply every function to the executable. No source registration, source selection or complete-issue acceptance is claimed.

## Independent file evidence

The complete symbol map and `config/43U/splits.txt` from CC0 Wii IPL, pinned to `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, distinguish these original files. Their physical target boundaries also follow native function ends and default SDK alignment. Related file spans support unit identity; the target's actual section sizes still require independent proof.

| Original source suffix | Target text range | Issue contribution | Status |
| --- | --- | --- | --- |
| `fa/driver/sd_drv.c` | starts `0x8021FFC0`; last SD function ends `0x80222E84` | 17 body rows owned by #373; first function belongs to external #372 | Preserve #372's earlier bodies and split/header ownership; coordinate owned tail additions through PR #931 |
| `fa/driver/pfd_cmn.c` | `0x80222E90–0x80222FA8` | one function, `pfd_get_media_drv_char` | First-owned #373; complete ordinary-C external candidate exact; neighboring split correction requested |
| `fa/driver/msc_drv.c` | `0x80222FB0–0x80222FC4` | two functions, 20 bytes | First-owned #373; split prepared; complete external candidate exact |
| `fa/msc/puh_msc.c` | `0x80222FD0–0x80223248` | four functions | First-owned #373; complete external object exact; diagnostic command-payload view remains under review |
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

The private block-device record's unconsumed ranges are still diagnostic byte views; their original types have not been recovered. The block candidate therefore remains outside tracked implementation pending interface review. The sense translator also remains external until its split-only baseline and actual source registration gates can run. Neither external result establishes linked source coverage or whole-issue completion. The original #373 inventory still includes its 17 unresolved SD-tail bodies in the shared file first owned by #372.

## Traps

- The translator's final comparison is unsigned. A signed literal in the otherwise identical readable switch changes exactly the native `cmplwi` to `cmpwi`. The unsigned literal reflects the observed comparison; its complete object then matches without compiler-flag changes.
- Constant propagation across a linear listing is insufficient for branch-derived switch facts: one branch can replace a register used as a different constant base on another path. Collect comparison facts through concrete native branch execution and iterate newly discovered boundary values before asserting an exhaustive case table.

# F-373-3: Complete USB command API diagnostic object

`373-probe-api-whole.py` compiles all four retained command API functions together under unchanged Wii/1.0 SDK flags. The complete object matches 632 `.text` bytes, including four internal padding bytes, 12 `.sbss` bytes, every function extent and all 18 independently resolved relocation targets. `usbh_msc_read10` and `usbh_msc_write10` each retain 128 instruction bytes; local `_uhf_msc_api_send_message` retains 364 and `uhf_msc_get_message_id` retains eight. The complete unit ends at `0x80223248`, before the next unit's alignment. The original reference's thread-priority API is absent from this target.

Actual field accesses and the pinned original map establish `st_uhs_msc_status` at `0x804B9270`, `st_uhs_msc_msgid` at `0x804B9274`, and `uhg_msc_memid` at `0x804B9278`, each four bytes. This ends the unit's small-data storage at `0x804B927C`; the following four alignment bytes do not belong to the heap handle. The compiler emits the three declarations in reverse storage order in this candidate, and the audit checks each emitted offset rather than accepting only a zero-filled total extent.

Both transfer APIs clear a 40-byte command payload, require status 1, place the device, sense buffer, data buffer, block and 16-bit transfer count at the native positions, and submit the original read/write operation value. The diagnostic source represents the payload explicitly as ten words and its count slot as a halfword view. This is a native protocol view, not a claim that all original private record members and types are recovered. The sender allocates 60 bytes, consumes the proven 52-byte prefix consisting of operation, semaphore, result pointer and copied payload, then performs the native send/wait/delete paths. The unconsumed trailing allocation bytes are not assigned invented members. Its error result is `-12`, whereas the related #374 block sender returns `-1`.

External `373-reference/api-whole-evidence.json` preserves the complete proof. Across the callback bridge, command API, sense translator and first three shared block functions, 10 original #373 functions and 1,944 native instruction bytes now have exact external candidates. The first-owned common helper and owned shared-file SD tails remain outside that count. No new source registration, linked source credit or whole-issue completion is claimed.

## Traps

- The actual command and block senders both pass a third zero argument to `uhf_ker_send_message`; a related reference declares only two arguments because its retained callee does not use the extra incoming register. Preserve the actual caller contract rather than deleting an unused argument to match the reference declaration.
- Distinguish original retained objects from alignment: the four-byte heap handle's inferred eight-byte metadata extent includes the next unit's padding. Whole-unit storage and each emitted symbol offset must agree with actual accesses and the following unit boundary.

# F-373-4: Independent common media-driver helper

`pfd_get_media_drv_char` is a separate original `pfd_cmn.c` unit at `0x80222E90–0x80222FA8`, corroborated by the pinned original FA map and source-file spans. Its ordinary C candidate checks the output pointer and capacity, scans all 26 PDM partitions initialized for the input disk, then finds matching partitions in the 26 PF volumes and copies their drive characters until the requested capacity is reached. It includes the actual canonical `pdm.h` and #363's explicitly approved complete volume header checkpoint `dc3bf19e45e37260bef03251219a837f837ee16c`; no private replica of those records is declared.

`373-probe-common-driver.py` compiles the candidate with unchanged Wii/1.0 SDK flags, verifies the original DOL's frozen SHA-256, compares the complete 280-byte text against that DOL, checks the exact function extent and absence of other allocated sections, and independently resolves every global and call target. All checks pass. The actual global bases are `pdm_disk_set = 0x803EAFA8` and `pf_vol_set = 0x803EB970`; the partition-array offset is `0x4E4`, the volume-array offset `0x88`, and the canonical volume stride `0x1FAC`. The native partition initialization call resolves to `pdm_part_is_initialized` at `0x801FDC30`. External `373-reference/common-driver-evidence.json` records the complete proof and consumed canonical-header digest.

All 11 original first-owned #373 functions now have exact external candidates, totaling 2,224 native instruction bytes. The remaining 17 original rows and 9,644 bytes are also owned by #373, within the shared SD file first owned by external #372. The issue body assigns every listed function to #373; file ownership assigns the split, object registration and header to #372, while later body additions remain with their respective block owners. No neighboring bodies are taken and no duplicate source credit is claimed. [The existing PR #931 coordination comment](https://github.com/revvmp/pokemon-rumble-decomp/pull/931#issuecomment-6243828295) now includes the concrete common-helper proof and requests ending the SD split at its actual final function end `0x80222E84`, leaving the separately owned helper and intervening alignment for integration. Its source, header and split remain untouched here. Owned tail bodies can be investigated externally and contributed through the shared-file workflow after owner coordination; they are not credited as someone else's completed work. Actual split-only baseline, source registration and final complete pipeline gates remain pending.

## Traps

- The helper's inner loop index is eight bits. A wider index changes the compiler's unrolled induction updates and removes one native instruction; the ordinary byte-sized loop reproduces the complete function without flag changes.
- PowerPC signed low halves matter when resolving address pairs. `lis 0x803F` followed by signed `addi 0xAFA8` resolves to `0x803EAFA8`, not `0x803FAFA8`. Compare resolved addresses with canonical symbol metadata before diagnosing a remaining instruction difference as source scheduling.

# F-373-5: First three owned shared-SD tail probes

Three of the 17 owned tail functions in the shared SD file now have independent external native matches: `pfd_sddrv_init_drv_tbl` at `0x80220A20`, 24 bytes; `pfd_sddrv_registar_callback` at `0x80220A40`, 16 bytes; and `pfd_sddrv_is_media_insert` at `0x80220A50`, 152 bytes. The first writes the eight-entry driver-function-table pointer and extension, the second saves the two callbacks with original `registar` spelling, and the third refreshes the drive character and reports inserted media through the canonical PDM notification. The latter preserves the conventional disk-pointer argument even though this retained implementation reads only the established shared driver state.

`373-probe-owned-sd-small.py` checks every instruction and independently resolves the actual function-table, state, callbacks, common helper and notification targets. Its diagnostic driver-record view repeats the exact field facts of external PR #931's proposed private record; it introduces no competing tracked header or storage definition. That owner's first nine source bodies, file registration, split and header are unchanged. Appending these owned bodies to the canonical shared file remains contingent on owner coordination and actual baseline/source acceptance.

`373-audit-first-owned-external.py` reruns all owned external proofs and checks the unchanged complete original issue inventory. It now covers 14 original rows and 2,416 native instruction bytes: 11 first-owned functions and the three shared-file tails. Fourteen shared-file tail rows and 9,452 bytes remain unresolved. All shared #374 bytes are excluded from this total. No source is selected and no newly linked coverage is claimed. The selected CC0 Wii IPL VF SD source and header were inspected as a related revision, pinned at `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`; they have no additional file notices, and their surrounding device-management implementation differs from this target. Their root license, exact bytes and hashes are preserved in external `373-reference/ipl-sd/manifest.json`; native target behavior remains authoritative.

The common helper's actual three callers at `0x80220058`, `0x80220180` and `0x80220A8C` each explicitly mask its returned count to eight bits before comparing it. The external helper prototype keeps the byte-sized capacity used by the pending owner's declaration, with the original function name replacing its provisional alias. No unsupported wider public interface is published.
