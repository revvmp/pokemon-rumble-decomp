# F-374-1: SDI and USB kernel reference coverage

## Scope and provenance

Issue #374 surveys 35 retained rows and 8,172 instruction bytes. Its first eight functions are the tail of `puh_msc_blk.c`, whose first function belongs to the same contributor's #373 claim. The remaining rows comprise twenty functions in `sdi_api.c`, three in `puh_ker_mem.c`, one in `puh_ker_msg.c`, and three in `puh_ker_sem.c`. No unit is source-selected by this checkpoint and no new production source bytes are claimed.

The reference is [wii-ipl at f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c](https://github.com/koopthekoopa/wii-ipl/tree/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c). Its root license is CC0; the selected SDI and USB kernel files have no overriding file-level notices. The local reference manifest records the commit, blob identifiers and SHA-256 values for each selected file. Reference comments are removed before generating candidates with the project's vendor stripping tool. Native function identities and unit order are checked independently against the pinned IPL symbol and split maps.

## Native comparison

External Wii/1.0 SDK compilations resolve every relocation to its actual target before comparing complete instruction streams with the frozen DOL. The original executable SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

| Function | Native address | Bytes | External comparison |
| --- | --- | --- | --- |
| `__sdCb` | `0x80223E70` | 276 | Exact |
| `ISD_GetHCRegister` | `0x80223F90` | 160 | Exact |
| `ISD_GetDeviceStatus` | `0x80224030` | 220 | Exact |
| `sduCommand` | `0x802242D0` | 448 | Exact |
| `ISD_ResetDevice` | `0x80224490` | 212 | Exact |
| `ISD_MountCard` | `0x80224A90` | 212 | Exact |
| `ISD_UnmountCard` | `0x80224B70` | 64 | Exact |
| `ISD_ReadCardRegister` | `0x80224D30` | 748 | Exact |
| `uhf_ker_get_memory_block` | `0x80225420` | 32 | Exact |
| `uhf_ker_release_memory_block` | `0x80225440` | 60 | Exact |
| `uhf_ker_check_cacheable` | `0x80225480` | 8 | Exact |
| `uhf_ker_send_message` | `0x80225490` | 132 | Exact |
| `uhf_ker_create_sem` | `0x80225520` | 408 | Exact |
| `uhf_ker_delete_sem` | `0x802256C0` | 256 | Exact |
| `uhf_ker_get_sem` | `0x802257C0` | 96 | Exact |

These fifteen functions cover 3,332 native instruction bytes. The memory unit also reproduces its four internal padding bytes, for 104 text bytes total, with no allocated data and both actual MEM call targets exact. The message unit's 132 text bytes and semaphore unit's 768 text bytes, including eight internal padding bytes, compare exactly after actual relocation resolution. Their retained static storage still needs the full native layout and whole-unit acceptance checks. These are external candidate results, not source-selected acceptance or a full issue completion.

Reproduction scripts outside the checkout are `374-probe-sdi-reference.py`, `374-audit-sdi-partial.py`, `374-probe-memory-kernel.py`, and `374-probe-usb-kernel-reference.py`. All completed with exit 0. The SD audit deliberately reports partial coverage and retains mismatches rather than treating its exit status as whole-unit acceptance. No configure, Ninja or frozen build verification has run for this checkpoint.

## Header coordination

The externally owned #372/#931 `include/revolution/fa/isd.h` already proposes a forty-byte `ISD_HANDLE` with descriptor at zero, RCA at `0x10`, capacity words at `0x18`, and open state at `0x24`. Those offsets agree with the reference SD record and these native consumers. Preserve that owner's canonical record and adapt source member accesses to it; do not publish a second SD record under another include tree. Original API names established by the pinned map and reference include `ISD_InitCard`, `ISD_MountCard`, `ISD_UnmountCard`, `ISD_GetDeviceStatus`, `ISD_ResetDevice`, and `ISD_ReadCardRegister`. Coordinate any correction to provisional descriptive API names with the owning contributor before publication.

## Remaining work

The SD reference is partial: several retained SD routines require reconstruction. Static `sduCommandv` and data-width helpers are currently unretained because their missing callers have not been added, so their absence from the candidate object does not prove a boundary. An extra reference `ISD_ProbeCard` body has no corresponding retained target entry and must be handled before whole-unit comparison. The eight shared USB block tails remain part of this issue's original acceptance inventory.

`ISD_GetCardSize` has the correct 336-byte extent but still differs. Introducing ordinary named CSD size and multiplier locals before the read-length clamp reduced differences from 33 words to 26; a bounded declaration-order diagnostic reaches 23. None is accepted. Preserve the controls and pursue plausible readable data flow rather than register-forcing source changes.

## Traps

- The reference callback orders cases 25, 38, 18; this target's exact retained body orders 25, 18, 38. Preserve actual target control flow and check all resolved instruction bytes.
- The high-capacity size calculation in the reference multiplies by 512, while the native target shifts by ten and therefore multiplies by 1,024. Reference revision agreement cannot replace the native semantic check.
- The reference's SD command allocation uses a rounding macro absent from current headers. Its actual requested extent is 64 bytes; preserve that evidenced extent with an ordinary equivalent expression rather than adding an unsupported shared macro.
- A partial audit returning zero reports that it completed its comparison. Its explicit per-function mismatch and coverage records remain authoritative; it does not establish whole-unit matching.
- The three routines after message sending are semaphore operations in the distinct `puh_ker_sem.c` unit. Their actual `OSWaitSemaphore` calls and pinned original names independently confirm the boundary. Do not infer a four-function message unit from adjacency or from a partial reference file.
- The reference's semaphore creation checks `count != 0 && count != 1`; this target uses the ordinary unsigned `count > 1` equivalent. That bounded source change reproduces the target's 408-byte function rather than the reference's 416-byte version.

# F-374-2: Retained SD transfers and initialization

## Additional external proof

The next external checkpoint proves nineteen of twenty retained SDI functions, including the original eight in F-374-1. Only `ISD_GetCardSize` remains unaccepted. Together with the seven USB kernel functions, issue coverage is now 26 original functions and 6,104 native instruction bytes. This supersedes the earlier fifteen-function coverage count without changing its historical evidence.

| Additional SDI function | Native address | Bytes |
| --- | --- | --- |
| `sduCommandv` | `0x80224110` | 448 |
| `ISD_ReadBlock` | `0x80224570` | 12 |
| `ISD_WriteBlock` | `0x80224580` | 12 |
| `ISD_ReadMultiBlock` | `0x80224590` | 12 |
| `ISD_WriteMultiBlock` | `0x802245A0` | 12 |
| `ISD_ReadMultiBlockAsync` | `0x802245B0` | 616 |
| `ISD_WriteMultiBlockAsync` | `0x80224820` | 616 |
| `ISD_InitCard` | `0x80224BB0` | 372 |
| `sduDatabuswidth` | `0x80225020` | 380 |
| `ISD_RegisterDeviceIntrHandler` | `0x802252F0` | 168 |
| `ISD_UnregisterDeviceIntrHandler` | `0x802253A0` | 124 |

`374-probe-sdi-transfers.py`, `374-probe-sdi-interrupts.py`, and `374-probe-sdi-init.py` compile ordinary C candidates and resolve every relocation before comparing the complete retained native extents. Each completed with exit 0; the final audit explicitly reports nineteen exact bodies and the remaining geometry mismatch. No build/source-selection acceptance is inferred from these external compilations.

The transfer functions use the second response buffer, `__sdResp2`, at `0x804B9298`. A small inline clock helper takes the descriptor and requested enabled value, writes the command buffer, and maps a failed IOS request to the fatal SD error. This captures the ordinary repeated operation and reproduces both complete 616-byte transfers. Separate response arrays preserve their actual native stack extents. The read path invalidates the data cache and uses `sduCommandv`; the write path flushes it and uses `sduCommand`.

Initialization independently proves the original function-static `lo` pointer at `0x804B92A4`, IPC heap extent 2,048 bytes, four 32-byte buffer allocations, a 96-byte vector allocation, and the three device paths. The original pinned symbol map names the local pointer `lo$3187`; the compiler-generated spelling differs with source revision, while the ordinary function-static declaration and resolved address agree.

The canonical-header owner was asked in [PR #931](https://github.com/revvmp/pokemon-rumble-decomp/pull/931#issuecomment-6243828295) to replace six provisional API names with the pinned original names, preserving its existing record. No header change or assigned neighboring source edit was made by this contributor.

## Traps

- Retaining a shared initialized error variable across early fatal branches emitted twenty extra bytes in each async transfer. Ordinary early returns for invalid heap/state restore the actual control flow; this is a source-structure finding, not a compiler flag change.
- Initialization pointer checks written as `pointer == NULL` caused the compiler to retain a shared zero register and use register comparisons. Ordinary `!pointer` checks reproduce all actual instructions under the same unchanged SDK flags. Both controls have the same 372-byte extent, so extent agreement alone does not establish a match.
- `ISD_UnregisterDeviceIntrHandler` has a genuinely 32-byte-aligned local response buffer, confirmed by its dynamic stack alignment and actual buffer address. Preserve that local alignment rather than changing the whole translation unit's function alignment.

# F-374-3: Complete external SDI object comparison

The final geometry candidate closes the remaining instruction differences with ordinary CSD evaluation: calculate sector size before total capacity, then combine the masked lower sector-size bits with the upper bit. All twenty retained functions now compare exactly under unchanged SDK Wii/1.0 flags. The earlier declaration-order and mask controls remain failed diagnostic evidence, not acceptance claims.

Removing the reference-only unretained `ISD_ProbeCard` and ordering the retained data-width helper after card-register reading restores every original function start in a single complete object. `374-probe-sdi-whole.py` then `374-audit-sdi-whole-allocated.py` both exit 0. The latter independently resolves all 231 text relocations, compares complete text including padding, compares literal and initialized data directly, and checks the exact NOBITS storage extents.

| Section | Native start | Actual source extent | Result |
| --- | --- | --- | --- |
| `.text` | `0x80223E70` | `0x15AC` | Exact 5,548 bytes, including 5,448 instruction bytes and 100 internal padding bytes |
| `.data` | `0x803C5280` | `0x50` | Exact reset diagnostic and three device-path strings |
| `.bss` | `0x80422FC0` | `0x80` | Mutex at zero, paths at `0x20`, exact allocation/alignment |
| `.sdata` | `0x804B88E0` | `0x4` | Exact signed heap identifier initialized to -1 |
| `.sbss` | `0x804B9288` | `0x20` | Exact seven globals and function-static heap-base pointer |

Whole issue external coverage is now 27 of 35 original rows and 6,440 of 8,172 instruction bytes; the eight USB block tails remain. The complete SDI candidate still uses the isolated reference-shaped header overlay. Actual canonical-header adaptation, split-only baseline, real Ninja-built objects, source-input/MAP checks, objdiff and frozen full-DOL gates remain required before source acceptance. No source is registered or linked by this documentation checkpoint.

## Traps

- The inferred existing `__sdHeapId` symbol extent of eight bytes includes four trailing alignment bytes. The actual source object is a four-byte signed heap identifier with eight-byte section alignment. Preserve the following object's alignment rather than adding storage to the variable.
- The final SDI function ends at `0x8022541C`; the next memory function starts at `0x80225420`. Those four external alignment bytes do not belong to the SDI text split.
