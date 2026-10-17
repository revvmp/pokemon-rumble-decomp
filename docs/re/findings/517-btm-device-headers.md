# F-517-1: BTM device-controller header prerequisite

Issue #517 needs the complete controller reset states and interfaces before its two device source units can be reconstructed. This prerequisite changes canonical headers only and adds no source reconstruction credit. It must merge before the dependent source delivery. The branch follows pending PR #696, which includes the merged BTM inquiry/main/power deliveries. The pull request records its current parent and exact verified head.

## Target-evidenced reset states

The prior canonical enum had wait-for-reset value 0, wait-after-reset value 1, and ready value 2. The target `BTM_IsDeviceUp` at `0x8031C824` loads `btm_cb.devcb.state` from offset 0x64E and subtracts 5 before its boolean equality sequence (`addi r0,r3,-5` at `0x8031C830`). Ready is therefore 5. The pinned controller source and the exact target instruction streams establish the intermediate query states:

| Value | Name | Proven operation |
| --- | --- | --- |
| 0 | `BTM_DEV_STATE_WAIT_RESET_CMPLT` | Reset retry / completion |
| 1 | `BTM_DEV_STATE_WAIT_AFTER_RESET` | Post-reset delay |
| 2 | `BTM_DEV_STATE_WAIT_BUF_SIZE` | Query HCI buffer size |
| 3 | `BTM_DEV_STATE_WAIT_LOCAL_VERSION` | Query local version |
| 4 | `BTM_DEV_STATE_WAIT_LOCAL_FEATURES` | Query local features |
| 5 | `BTM_DEV_STATE_READY` | Device usable |

These descriptive names replace Petari's numeric `BTM_DEV_STATE_2`, `_3`, and `_4` names. They are behavioral names, not claimed original spellings. Main already carries the correct complete enum through PR #698 / F-519-2, including `WAIT_BUF_SIZE` and ready value 5. The refreshed prerequisite preserves that definition and changes no existing state value or storage type.

## Other interfaces

Additions recover the `UINT8*` device-class pointer alias, new vendor/stored-link-key callback result structures, missing controller/security prototypes, controller timer identifiers, scan defaults and feature-bit macros, SCO packet capability aliases, and `BTM_DEV_RESET` status value 12. Existing shared structure layouts and declarations are unchanged. `tBTM_VSC_CMPL` contains opcode at 0, parameter length at 2, and a 255-byte parameter buffer at 4, natural total size 0x104. Stored-key read/write/delete result sizes are 6, 3, and 4 bytes respectively; the existing two-byte return-key event remains unchanged.

Reference declarations and constants come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/btm_api.h`, `btm_int.h`, `hcidefs.h`, and `bt_target.h`, each Copyright (C) 1999-2012 Broadcom Corporation, including changes by muff1n1634. The [file-specific notice](../licenses/517-bte-NOTICE.md) preserves provenance and the full license. Dependencies are narrowed and source comments are documented here. Scratch compilation of the forthcoming `btm_dev.c` and `btm_devctl.c` uses the candidate declarations and measured GC/3.0a3/SDK flags; retained instruction candidates and full data payloads match the target. This is preliminary source evidence, not acceptance of those units.

## Header acceptance

The initial default-directory baseline captured all 301 existing source objects and preserved their code/data/relocations. After reconciling with the newer main deliveries and rebasing onto the refreshed #696 base, a fresh `build/517-header-refresh-1` probe compiles the exact parent headers, restores the proposed headers, and repeats all-source compilation. All 322 then-current source objects retain identical code/data section sizes and bytes plus canonical relocation targets. Configure and both all-source passes exit 0. The full Ninja link/CHECK and explicit verification of `build/517-header-refresh-1/wpse-v1` then also exit 0. Main game DOL SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; scope excludes boot content 6, data contents, and WAD packaging. Final rebased exact-head verification is required after the dependency merges; local verification is the merge gate under the updated take-issue procedure. No frozen manifests, expected hashes, progress inputs, or source-unit selections change.

## Traps

Adding a public `BTM_VendorSpecificCommand(UINT16, ...)` prototype changed the existing `L2CA_SetAclPriority` caller outside this claim: CodeWarrior added an opcode truncation instruction and rescheduled the call setup, breaking its previously matched bytes. The prototype is excluded from this prerequisite; the forthcoming function definition retains the measured SDK signature in its own source unit. With that declaration absent, both the initial 301-object baseline and refreshed 322-object baseline retain identical bytes and canonical relocations. A coordinated caller change belongs to its owner; do not alter that caller or widen the callee's evidenced signature to make this header addition pass.

A raw four-byte `blr` match suggested the unlinked reference `BTM_ContinueReset` body at `0x8031E048`. Existing evidence already identifies that address as `btm_discovery_timeout`, and it is outside the controller unit's contiguous retained range. Reject the instruction-only candidate and leave the existing symbol unchanged. A unique tiny match inside a limited search window does not establish function identity.
