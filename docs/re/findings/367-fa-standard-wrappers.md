# F-367-1: Complete standard FA wrapper inventory

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #367 owns 25 functions and 1,608 native code bytes at `0x8021B190–0x8021B884`. Every original issue row is covered exactly once by the following 25 one-function translation units. Inter-unit alignment remains outside each source extent. No neighboring issue function, data range or storage layout is reconstructed here.

## Source identity and boundaries

The separate FA section of CC0 Wii IPL `config/43U/splits.txt` and `config/43U/symbols.txt`, pinned to `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, names these standard wrapper files and their original public functions. Its FA source files are not reconstructed. Its related VF sources under `libs/RevoEX/src/vf/standard/` provide readable precedents for path initialization, error recording and return conversion. CC0 Petari at `e0ab141914921726a595573dc48d3f13d0922efe` supplies the narrow mode-parser precedent used to reconstruct the analogous wide parser.

Target call graphs independently distinguish otherwise identical wrappers. In particular, `PFDIR_mkdir` at `0x802057A0` calls `PFDIR_p_mkdir`, while `PFDIR_rmdir` at `0x802056F0` calls `PFDIR_p_rmdir`. The move and rename wrappers likewise call their respective lower operations. Each lower callee's proven original FA name is supplied in symbols metadata without changing that neighboring unit's split, extent or type.

| Source unit | Original function | Target address | Native extent |
| --- | --- | --- | --- |
| `pf_mkdir.c` | `pf2_mkdir` | `0x8021B190` | `0x50` |
| `pf_mount.c` | `pf2_mount` | `0x8021B1E0` | `0x28` |
| `pf_move.c` | `pf2_move` | `0x8021B210` | `0x7C` |
| `pf_opendir.c` | `pf2_opendir` | `0x8021B290` | `0x58` |
| `pf_readdir.c` | `pf2_readdir` | `0x8021B2F0` | `0x24` |
| `pf_regctx.c` | `pf2_regctx` | `0x8021B320` | `0x24` |
| `pf_remove.c` | `pf2_remove` | `0x8021B350` | `0x50` |
| `pf_rename.c` | `pf2_rename` | `0x8021B3A0` | `0x7C` |
| `pf_rewinddir.c` | `pf2_rewinddir` | `0x8021B420` | `0x28` |
| `pf_rmdir.c` | `pf2_rmdir` | `0x8021B450` | `0x50` |
| `pf_rmvvol.c` | `pf2_rmvvol` | `0x8021B4A0` | `0x28` |
| `pf_seekdir.c` | `pf2_seekdir` | `0x8021B4D0` | `0x24` |
| `pf_setclstlink.c` | `pf2_setclstlink` | `0x8021B500` | `0x28` |
| `pf_setcode.c` | `pf2_setcode` | `0x8021B530` | `0x24` |
| `pf_setencode.c` | `pf2_setencode` | `0x8021B560` | `0x24` |
| `pf_settailbuf.c` | `pf2_settailbuf` | `0x8021B590` | `0x28` |
| `pf_setupfsi.c` | `pf2_setupfsi` | `0x8021B5C0` | `0x2C` |
| `pf_setvol.c` | `pf2_setvol` | `0x8021B5F0` | `0x5C` |
| `pf_setvolcfg.c` | `pf2_setvolcfg` | `0x8021B650` | `0x28` |
| `pf_sync.c` | `pf2_sync` | `0x8021B680` | `0x28` |
| `pf_telldir.c` | `pf2_telldir` | `0x8021B6B0` | `0x24` |
| `pf_unmount.c` | `pf2_unmount` | `0x8021B6E0` | `0x28` |
| `pf_unregctx.c` | `pf2_unregctx` | `0x8021B710` | `0x24` |
| `pf_xdivide.c` | `pf2_xdivide` | `0x8021B740` | `0x90` |
| `pf_w_api_util.c` | `PFWAPI_ParseOpenModeString` | `0x8021B7D0` | `0xB4` |

The final wide parser is `PFWAPI_ParseOpenModeString` in `pf_w_api_util.c`, not a narrow public wrapper. The reference's separate next file and this target's next aligned entry place the boundary before `0x8021B890`, owned by #368. The earlier #366 wrapper inventory ends before this issue's first file.

## Interfaces and source fidelity

All source is ordinary C, built with established Wii/1.0 `cflags_sdk` and default 16-byte function alignment. Public wrappers preserve the reference's `pf2_` names, record failed string initialization in `pf_vol_set.last_error`, and call the existing PFAPI converters. Two-path operations initialize both local strings before testing the combined error. The directory opener preserves the reference's output-pointer convention, including an uninitialized local output on the initialization-error path; the converter selects NULL when the error is nonzero.

Headers declare only the forwarded contracts used here. `PF_CLUSTER_LINK_CONFIG`, `PF_TAIL_BUF` and `PF_VOLUME_CONFIG` remain incomplete pointer-only types because these wrappers neither allocate nor inspect their storage. No size is asserted for these types. The cluster-link pointer uses the same canonical tag as #366's file-level interface: both lower native consumers read two words at offsets 0 and 4 and a halfword at offset 8 in mode 1. No `PF_VOLUME`, `PF_VOLUME_SET`, file, directory or cache member changes. The `PF_STR` and `PF_CHARCODE` declarations are imported by the volume header so its new prototypes compile under direct inclusion by cache and cluster consumers.

The original utility interface is owned by #365 and supplied by prerequisite PR #882. The initial bounded declarations in `8692a6f` were held after review; corrective header commits `ab3656a` and `918d53c` supply the revised contracts and original shared divide callee for integration alongside #366's independent declarations. The rejected header revision must not be published alone. The final source merge must use the merged canonical headers and repeat its latest-main gates; an approved scratch header copy is not a substitute for that dependency.

## Lower callee identity

Each referenced lower function or data symbol below exists under this exact original name in the pinned FA map. Native target addresses are independently checked through the source object's relocations and the complete frozen native instruction stream. The FA reference address and extent are evidence from the reference revision, not replacement target metadata. Outside extents and types remain unchanged.

| Original lower name | Native target | Pinned FA reference | Reference extent |
| --- | --- | --- | --- |
| `PFSTR_InitStr` | `0x801fb790` | `0x815c6670` | `0x88` |
| `PFDIR_mkdir` | `0x802057a0` | `0x815d034c` | `0xe0` |
| `PFDIR_rmdir` | `0x802056f0` | `0x815d029c` | `0xb0` |
| `PFAPI_convertReturnValue` | `0x8021a3f0` | `0x815e4bb0` | `0x10` |
| `pf_vol_set` | `0x803eb970` | `0x811781e0` | `0x33800` |
| `PFAPI_convertReturnValue2NULL` | `0x8021a420` | `0x815e4be0` | `0x18` |
| `PFAPI_convertReturnValue4unmount` | `0x8021a440` | `0x815e4bf8` | `0x24` |
| `PFDIR_move` | `0x80205230` | `0x815cfdf0` | `0xe8` |
| `PFDIR_opendir` | `0x80205320` | `0x815cfed8` | `0x9c` |
| `PFDIR_readdir` | `0x802054a0` | `0x815d0054` | `0xd0` |
| `PFDIR_rename` | `0x80205140` | `0x815cfd08` | `0xe8` |
| `PFDIR_seekdir` | `0x80205640` | `0x815d01ec` | `0xb0` |
| `PFDIR_telldir` | `0x80205570` | `0x815d0124` | `0xc8` |
| `PFFILE_remove` | `0x80210960` | `0x815db310` | `0xa8` |
| `PFFILE_divide` | `0x80211be0` | `0x815dc538` | `0x124` |
| `PFVOL_mount` | `0x80218260` | `0x815e2a80` | `0x2d0` |
| `PFVOL_regctx` | `0x80219a30` | `0x815e4220` | `0x12c` |
| `PFVOL_rmvvol` | `0x80217260` | `0x815e1a9c` | `0x2fc` |
| `PFVOL_setclstlink` | `0x80218b30` | `0x815e3344` | `0x320` |
| `PFVOL_setcode` | `0x802199d0` | `0x815e41c4` | `0x5c` |
| `PFVOL_setencode` | `0x80219ce0` | `0x815e44c4` | `0x20` |
| `PFVOL_settailbuf` | `0x80219170` | `0x815e397c` | `0x264` |
| `PFVOL_setupfsi` | `0x80218830` | `0x815e304c` | `0x2f8` |
| `PFVOL_setvol` | `0x80216c70` | `0x815e14b8` | `0x330` |
| `PFVOL_setvolcfg` | `0x802193e0` | `0x815e3be0` | `0x368` |
| `PFVOL_sync` | `0x80218e50` | `0x815e3664` | `0x318` |
| `PFVOL_unmount` | `0x80218530` | `0x815e2d50` | `0x2fc` |
| `PFVOL_unregctx` | `0x80219b60` | `0x815e434c` | `0x178` |

Both public `pf2_divide` and `pf2_xdivide` delegate to the same original `PFFILE_divide` at `0x80211BE0`. A draft initially invented a lower `PFFILE_xdivide` name by copying the public prefix. Strict paired-relocation comparison during the wide-wrapper audit rejected that alias; it is absent from the original FA map. The corrected source and canonical header use `PFFILE_divide`. This is another case where equal native addresses do not establish an original name.

## Validation

On main base `9fdce911`, the first split-only full build and then the complete 25-unit split-only build both passed plain Ninja and the explicit frozen verifier. Initial compiled objects matched native bytes but review rejected five mistaken identities and forwarding contracts, as documented below. Those byte checks were insufficient source acceptance. After correcting the complete name and ABI inventory, all actual Ninja objects again reproduce complete native streams, all 75 paired relocations and every allocated objdiff section. No allocated data section is present in these units.

The corrected source selection passed `python configure.py --map`, plain `.venv/Scripts/ninja` and the explicit frozen verifier, all with exit 0. The complete original 25-row inventory audit checked each native extent, all 1,608 bytes after independent relocation resolution, all 75 relocation offsets/kinds/targets/addends, the actual source entries in `build.ninja`, each function's final ELF address/size and source contribution in `main.elf.MAP`, every allocated section at 100%, and the entire rebuilt DOL against the unchanged original. It also asserts all 25 original file/function identities against the complete pinned FA sequence, audits each original caller's target and public arity, and checks the exact argument-loading instruction sequences for all five corrected wrappers. The external reproducible audit is `367-audit-complete.py --source-selected`; its evidence is in `367-reference/complete-evidence.json`. Exact committed-head verification after canonical-header integration and the final latest-main rebase remain required before merge.

Canonical prerequisite #882 merged as `2fe0771d7205feddc60e27cfeec5791099355c1d`. The branch was restacked onto the merged header tree, dropping every duplicate lower/PFAPI header declaration commit and preserving the corrected single-argument detach contract. A fresh default build completed with configure/map, plain Ninja, explicit frozen verification and the complete original-inventory audit all exiting 0. The only dtk split rewrite reorders the owned corrected units by native address; it is retained in `d4e2f0c`. The audit again proves all 25 original source identities, 1,608 native bytes, 75 paired relocations, 28 exact original lower names, actual source inputs/map/final ELF and every allocated section at 100%. The stale unrelated directory ledger row reintroduced by union merge was removed while preserving main's current matching row. Only final exact latest-main verification and the coordinated merge remain.

## Traps

- Public API spelling is not proof of a lower helper name. The original FA namespace and strict relocation target-name pairing must agree; aliases at the same address cannot substitute for that identity check.

- Byte equality does not prove a forwarding contract. Review rejected revision `70618bcdcbae57339d68ada57938db6ed7e47c77` because five native-exact wrappers had incorrect API identities. The complete pinned FA inventory and actual dispatcher/lower-consumer argument uses establish the following corrections. In particular, the old two-argument bodies left an incoming third register untouched, while the old three-argument body forwarded a register its callee ignored. Neither error necessarily changed generated bytes.

| Address | Rejected identity | Correct original identity | Native caller | Forwarded arguments |
| --- | --- | --- | --- | --- |
| `0x8021B4A0` | `pf2_detach` | `pf2_rmvvol` | `0x8021CB70` | drive |
| `0x8021B500` | `pf2_buffering` | `pf2_setclstlink` | `0x8021CC3C` | drive, mode, configuration pointer |
| `0x8021B590` | `pf2_sync` | `pf2_settailbuf` | `0x8021CCD0` | drive, size, buffer pointer |
| `0x8021B650` | `pf2_setclstlink` | `pf2_setvolcfg` | `0x8021CD98` | drive, configuration pointer |
| `0x8021B680` | `pf2_settailbuf` | `pf2_sync` | `0x8021CC6C` | drive, mode |

- The lower cluster-link consumer at `0x80218B30` saves mode from `r4` and configuration from `r5`; the tail-buffer consumer at `0x80219170` saves size from `r4` and the buffer pointer from `r5`. The volume-configuration consumer at `0x802193E0` reads fields through `r4`, while sync at `0x80218E50` saves only its mode argument. The dispatcher independently loads the matching one-, two- or three-argument contracts at the call sites above.
- Standard wrappers often have identical instruction shapes. Following the actual lower callee is necessary to distinguish mkdir from rmdir, move from rename, and directory read from directory close. A matching wrapper skeleton alone does not establish its name.
- The native `pf2_setupfsi` entry explicitly truncates its flags argument before calling `PFVOL_setupfsi`. A public `u16` parameter removed that instruction and shortened the function by four bytes. The evidenced public `u32` to lower `u16` contract reproduces the native `clrlwi` and complete `0x2C` extent.
- Wide mode-parser switch case order changes layout even when all results are equivalent. Placing the default return before the plus case, as in the narrow reference source, reproduces the target's complete `0xB4` body.
- Adding PFAPI-dependent declarations to the volume header requires its own include of the string interface. Relying on a wrapper's include order failed the actual cache and cluster compile; adding the canonical include fixed those direct consumers without changing storage.
