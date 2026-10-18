# F-351-2: Original FA volume-label interface names

Target: `wpse-v1`, issue #351. This prerequisite corrects two provisional names before reconstructing the driver unit. It changes no ABI, layout, compiler option, original file, expected hash, split or source selection, and adds no reconstructed function credit.

## Primary name evidence

The separate FA portion of the CC0 Wii IPL primary map at revision `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, [config/43U/symbols.txt](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/config/43U/symbols.txt), names these consecutive functions with exactly the target's sizes. The earlier VF portion of the same map describes another interface; use the FA entries without the `VFi` prefix. No reference implementation is imported by this prerequisite.

| Target | Primary FA spelling | Native size | Previous provisional spelling |
| --- | --- | --- | --- |
| `0x801FBBE0` | `PFDRV_LoadVolumeLabelFromBuf` | `0x3C` | `PFVOL_LoadVolumeLabelFromBuf` |
| `0x801FBC20` | `PFDRV_StoreVolumeLabelToBuf` | `0x3C` | `PFDRV_LoadVolumeLabelFromBuf` |

Native direction independently confirms the names: the first function copies eleven bytes into the volume's label at `0x1648` and terminates byte `0x1653`; the second copies eleven bytes out of that member into its incoming buffer and terminates buffer byte eleven. Signatures and argument order remain unchanged. `pf_entry.c` calls the first and `pf_volume.c` calls the second. Their existing declarations in `pf_volume.h` and the two owning symbols are updated together; no alias or duplicate interface is introduced.

## Baseline and candidate validation

A clean committed source baseline at `61f8b4b10899228549fafc501e995a24c23c1985` completed configure/map, default Ninja and explicit frozen verification, all exit 0. The seeded worktree contains 680 actual configured source objects. Their SHA-256 fingerprints and the two affected objects plus actual ELF were captured outside the repository in `351-interface-baseline` before this edit.

After the correction, configure/map and default Ninja complete with exit 0. The prior DOL was removed before rebuilding. Explicit frozen verification exits 0 and the source-selected full DOL remains identical. The complete fingerprint comparison establishes 678 whole source objects byte-identical. Only `pf_entry.o` and `pf_volume.o` change, as expected.

For each affected source object and the real final ELF, an independent ELF32 big-endian parser checks every non-name section payload, raw relocation payload, section type/flags/address/link/info/alignment/entry-size metadata, symbol ordering and every symbol's value, size, type, binding, visibility and section. Those all remain identical. Symbol-name string offsets and file section offsets can move with string lengths. The only semantic symbol-name changes are the one expected rename per source object and both expected renames in the final ELF. These objects are not claimed wholly byte-identical. The runtime bytes and target identities are unchanged.

Commands:

```
python configure.py --map
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

All exit 0 for both baseline and candidate. The outside `351-interface-audit.py` also exits 0. The exact latest-main acceptance is repeated with `tools/collab/land.py` before merging this prerequisite. The driver implementation, native unit splits and whole #351 acceptance remain pending.

## Traps

- **An earlier provisional library name can reverse a copy direction.** Match the FA part of a reference map by sequence and size, then check native source/destination accesses. The VF spelling elsewhere in the same map belongs to another interface revision.
- **Correcting a shared external name changes object string tables.** Preserve the failed whole-file comparison, prove the exact bounded symbol-name changes and unchanged metadata/relocations/runtime bytes, and do not call the affected files whole-file identical.
