# F-514-3: Canonical API and power-management declarations

The earlier `dm_api.h` independently declared inquiry structures, messages, events, address arrays and an untyped search callback. Those types duplicate the canonical BTA headers completed for #512. This prerequisite makes the legacy path forward to `bta_dm_int.h`, which includes the canonical public declarations, and adds the original public API prototypes to `bta_api.h` with C linkage for C++ callers.

The existing partial API source switches from native `u8`/`u16`/`u32` aliases and `HC_BT_HDR` to SDK `UINT8`/`UINT16`/`UINT32` and `BT_HDR`, imports its declarations instead of redeclaring them locally, and expresses its existing 0x110-byte visibility allocation as `sizeof(tBTA_DM_MSG)`. Its ten existing bodies retain the same statements and control flow. No initializer or registration record is added in this prerequisite; the unit remains `NonMatching`.

The shared message union gains power-status and power-timer variants of sizes 0x14 and 0x0E. Both fit inside its existing 0x110-byte extent, and existing members retain their types and offsets. New power configuration types have their original layouts: `tBTA_DM_PM_CFG` is three bytes, `tBTA_DM_PM_ACTN` is four bytes with its timeout at offset two, and `tBTA_DM_PM_SPEC` is 0x4A bytes with nine pairs of actions starting at offset two. The existing 0x104-byte device control block, 0x7C-byte search control block and timer callback layouts are unchanged.

The pinned reference is Petari `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/{bta_api.h,bta_dm_int.h,btm_api.h,hcidefs.h,gki.h}`. Newly added enums, function declarations and layouts follow that reference. Scratch compilation of the complete API, state dispatchers and power-management sources against these declarations reproduces all 21 retained instruction streams with GC/3.0a3. That is diagnostic evidence; source-link acceptance belongs to the subsequent implementation change.

## Validation

Before changing the headers, capture every code/data section, BSS size and relocation target of the existing partial API, complete matching action unit and matching connection unit. Recompile the actual registered sources through `ninja all_source` and compare those records. All section bytes, sizes and resolved relocation targets remain identical, including the partial API after its type consolidation. Compiler-assigned anonymous literal identifiers are compared by their real section, offset and size rather than their unstable numeric spelling. This leaves the comparison of their bytes and destinations exact.

The pull request records `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, the normal full build, frozen-manifest verification, and the final rebased commit. No split, symbol, compiler setting, source selection or frozen fingerprint changes. This prerequisite adds no reconstructed code coverage.

## Provenance

The imported declarations adapt the pinned reference with comments removed, project include paths and only the needed interfaces. `bta_api.h` and `bta_dm_int.h` carry Copyright (C) 2003-2012 Broadcom Corporation; `btm_api.h`, `hcidefs.h` and `gki.h` carry Copyright (C) 1999-2012 Broadcom Corporation. They include changes by muff1n1634 and are licensed under Apache-2.0. The [full license](../licenses/510-apache-2.0.txt), [BTE notice](../licenses/510-bte-NOTICE.md), and [earlier API notice](../licenses/514-bte-NOTICE.md) are preserved. The API source's mechanical signature updates retain its existing file-specific provenance.

## Traps

- **A compatibility header can duplicate canonical types without sharing their C identity.** The old search callback used `void*`, while the canonical callback names the actual result union; its service mask used a native unsigned-int alias rather than the reference's unsigned-long SDK alias. Forward the legacy include to the canonical definitions and use those declarations in the source. For this target, rebuilding proves identical code, data and relocation destinations after consolidation; do not infer that every same-width type substitution is harmless.
