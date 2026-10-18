# F-374-1: SDI and USB kernel reference coverage

## Scope and provenance

Issue #374 surveys 35 retained rows and 8,172 instruction bytes. Its first eight functions are the tail of `puh_msc_blk.c`, whose first function belongs to the same contributor's #373 claim. The remaining rows comprise twenty functions in `sdi_api.c`, three in `puh_ker_mem.c`, and four in `puh_ker_msg.c`. No unit is source-selected by this checkpoint and no new production source bytes are claimed.

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

These eleven functions cover 2,440 native instruction bytes. The memory unit also reproduces its four internal padding bytes, for 104 text bytes total, with no allocated data and both actual MEM call targets exact. These are external candidate results, not source-selected acceptance or a full issue completion.

Reproduction scripts outside the checkout are `374-probe-sdi-reference.py`, `374-audit-sdi-partial.py`, and `374-probe-memory-kernel.py`. All completed with exit 0. The SD audit deliberately reports partial coverage and retains mismatches rather than treating its exit status as whole-unit acceptance. No configure, Ninja or frozen build verification has run for this checkpoint.

## Header coordination

The externally owned #372/#931 `include/revolution/fa/isd.h` already proposes a forty-byte `ISD_HANDLE` with descriptor at zero, RCA at `0x10`, capacity words at `0x18`, and open state at `0x24`. Those offsets agree with the reference SD record and these native consumers. Preserve that owner's canonical record and adapt source member accesses to it; do not publish a second SD record under another include tree. Original API names established by the pinned map and reference include `ISD_InitCard`, `ISD_MountCard`, `ISD_UnmountCard`, `ISD_GetDeviceStatus`, `ISD_ResetDevice`, and `ISD_ReadCardRegister`. Coordinate any correction to provisional descriptive API names with the owning contributor before publication.

## Remaining work

The reference is partial: several retained SD routines and three message-kernel routines require reconstruction. Static `sduCommandv` and data-width helpers are currently unretained because their missing callers have not been added, so their absence from the candidate object does not prove a boundary. An extra reference `ISD_ProbeCard` body has no corresponding retained target entry and must be handled before whole-unit comparison. The eight shared USB block tails remain part of this issue's original acceptance inventory.

`ISD_GetCardSize` has the correct 336-byte extent but still differs. Introducing ordinary named CSD size and multiplier locals before the read-length clamp reduced differences from 33 words to 26; a bounded declaration-order diagnostic reaches 23. None is accepted. Preserve the controls and pursue plausible readable data flow rather than register-forcing source changes.

## Traps

- The reference callback orders cases 25, 38, 18; this target's exact retained body orders 25, 18, 38. Preserve actual target control flow and check all resolved instruction bytes.
- The high-capacity size calculation in the reference multiplies by 512, while the native target shifts by ten and therefore multiplies by 1,024. Reference revision agreement cannot replace the native semantic check.
- The reference's SD command allocation uses a rounding macro absent from current headers. Its actual requested extent is 64 bytes; preserve that evidenced extent with an ordinary equivalent expression rather than adding an unsupported shared macro.
- A partial audit returning zero reports that it completed its comparison. Its explicit per-function mismatch and coverage records remain authoritative; it does not establish whole-unit matching.
