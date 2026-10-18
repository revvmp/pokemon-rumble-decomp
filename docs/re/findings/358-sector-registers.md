# F-358-3: Exact FAT core prefix and remaining full-unit dependency

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. This checkpoint is partial work for #358. It does not close the issue or enable `pf_fat.c` as `Matching`.

## Source correction

`PFFAT_GetSector` at `0x8020B130` is 352 bytes. Expressing its first unsigned remainder as `size - num_divide * p_vol->bpb.bytes_per_sector`, after the existing quotient calculation, produces the native register assignment. The native cached sectors-per-cluster value occupies `r7` and the quotient occupies `r8`; the prior `%` expression exchanged these registers in seven instruction words. For a valid nonzero sector size the quotient-derived remainder has the same unsigned value. No compiler flags, types, interfaces, data, helpers or split boundaries change.

All 17 retained core functions now compare at 100%, totaling 7,344 instruction bytes. An independent ELF32 big-endian comparison of the actual original and compiled objects proves the entire 7,412-byte text prefix, including internal padding, byte-identical. It separately resolves all 113 prefix relocations using the native section bases and configured external symbol addresses and compares their offset, relocation type and final target. All match. The 60-byte `fat_special_values` rodata table is byte-identical.

The complete native unit has 35 functions / 12,428 instruction bytes and a 12,620-byte text extent. Its 18 remaining functions / 5,084 instruction bytes belong to active #359, whose draft #958 and ownership were checked before this change. Coordination is recorded in [the issue thread](https://github.com/revvmp/pokemon-rumble-decomp/issues/359#issuecomment-6245371559). The existing full split and header remain intact. The original `pf_fat.o` is still selected by the real link; the exact prefix earns no source-selected code credit. Complete tail reconstruction, all retained functions and sections, source-only link inputs, and a fresh exact full DOL are required before enabling this unit and closing #358.

## Original issue inventory

The original 29 address/size rows total 11,292 instruction bytes. Twelve iterator functions / 3,948 bytes are already source selected. The seventeen core functions / 7,344 bytes are exact object-prefix work, with the original complete core unit still linked. No existing iterator bytes receive new credit.

| Address | Size | Native function | Acceptance state |
| --- | --- | --- | --- |
| `0x80208620` | `0x108` | `PFENT_ITER_IsAtLogicalEnd` | Matching, source selected |
| `0x80208730` | `0xF8` | `PFENT_ITER_MoveTo` | Matching, source selected |
| `0x80208830` | `0xF8` | `PFENT_ITER_Advance` | Matching, source selected |
| `0x80208930` | `0x38C` | `PFENT_ITER_IsInDirectory` | Matching, source selected |
| `0x80208CC0` | `0x25C` | `PFENT_ITER_Retreat` | Matching, source selected |
| `0x80208F20` | `0x33C` | `PFENT_ITER_FindCluster` | Matching, source selected |
| `0x80209260` | `0x164` | `PFENT_ITER_FindEntryOfPath` | Matching, source selected |
| `0x802093D0` | `0x48` | `PFENT_ITER_FindEntry` | Matching, source selected |
| `0x80209420` | `0xC` | `PFENT_GetEntryOfPath` | Matching, source selected |
| `0x80209430` | `0xC` | `PFENT_GetParentEntryOfPath` | Matching, source selected |
| `0x80209440` | `0x68` | `PFENT_ITER_GetEntry` | Matching, source selected |
| `0x802094B0` | `0x124` | `PFENT_ITER_LoadLongName` | Matching, source selected |
| `0x802095E0` | `0x14C` | `PFFAT_ReadFATSector` | Exact prefix, original unit selected |
| `0x80209730` | `0x240` | `PFFAT_SearchForNumFreeClusters` | Exact prefix, original unit selected |
| `0x80209970` | `0x128` | `PFFAT_ClearCluster` | Exact prefix, original unit selected |
| `0x80209AA0` | `0x188` | `PFFAT_FindClusterLink` | Exact prefix, original unit selected |
| `0x80209C30` | `0x194` | `PFFAT_FindClusterLinkPage` | Exact prefix, original unit selected |
| `0x80209DD0` | `0x1B8` | `PFFAT_ReadCluster` | Exact prefix, original unit selected |
| `0x80209F90` | `0x1C4` | `PFFAT_ReadClusterPage` | Exact prefix, original unit selected |
| `0x8020A160` | `0x21C` | `PFFAT_WriteCluster` | Exact prefix, original unit selected |
| `0x8020A380` | `0x220` | `PFFAT_WriteClusterPage` | Exact prefix, original unit selected |
| `0x8020A5A0` | `0x360` | `PFFAT_DoAllocateChain` | Exact prefix, original unit selected |
| `0x8020A900` | `0x37C` | `PFFAT_GetClusterInChain` | Exact prefix, original unit selected |
| `0x8020AC80` | `0x110` | `PFFAT_GetClusterContinuousSectorInChain` | Exact prefix, original unit selected |
| `0x8020AD90` | `0xF0` | `PFFAT_GetClusterAllocatedInChain` | Exact prefix, original unit selected |
| `0x8020AE80` | `0x190` | `PFFAT_GetClusterSpecified` | Exact prefix, original unit selected |
| `0x8020B010` | `0x118` | `PFFAT_GetClusterAllocated` | Exact prefix, original unit selected |
| `0x8020B130` | `0x160` | `PFFAT_GetSector` | Exact prefix, original unit selected |
| `0x8020B290` | `0x44` | `PFFAT_GetSectorSpecified` | Exact prefix, original unit selected |

## Validation and iterator padding

`python configure.py`, the real Ninja entry point after removing only the previous `main.dol`, and frozen-manifest `verify.py` all exited zero. The rebuilt DOL is byte-identical to the frozen original. The actual Ninja link query selects `src/RVL_SDK/fa/pf_entry_iterator.o` and excludes its original object; for `pf_fat.o` it selects the original and excludes the incomplete source object.

The containing iterator unit has sixteen exact functions / 7,836 instruction bytes and a 7,940-byte text section. Its current prelink `.sdata` score is 95.652176%, rather than the earlier blanket claim of 100% for data. Direct object inspection shows 22 matching source bytes plus two native zero padding bytes:

- Target, 24 bytes: `2e2e00002e0000003a0000005c0000002f00000000000000`.
- Source, 22 bytes: `2e2e00002e0000003a0000005c0000002f0000000000`.

The source payload is exact and the two extra target bytes are zero alignment padding. The already source-selected, freshly rebuilt full DOL proves those bytes remain correct after linking. No artificial source padding or iterator split change is introduced. F-358-1 and the ledger now state this distinction explicitly.

## Traps

- **Equivalent remainder expressions can change register allocation.** The trigger was an otherwise exact function with seven words exchanging `r7` and `r8`. Reusing the existing first quotient in the unsigned remainder expression recovers the native allocation; changing only the second remainder does not. Caching the divisor in locals of several types and declaration positions, or restructuring the rounded-division helper, did not recover this function. This finding applies to the observed Wii/1.0 SDK configuration and this expression, not to a general remainder-style rule.
- **A data score includes alignment padding.** The iterator's 22 exact payload bytes still produce a prelink score below 100% against a 24-byte target section. Inspect the actual payload and linked bytes before changing source data or repeating a blanket data-match claim.
