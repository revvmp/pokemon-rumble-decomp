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
| `pf_detach.c` | `pf2_detach` | `0x8021B4A0` | `0x28` |
| `pf_seekdir.c` | `pf2_seekdir` | `0x8021B4D0` | `0x24` |
| `pf_buffering.c` | `pf2_buffering` | `0x8021B500` | `0x28` |
| `pf_setcode.c` | `pf2_setcode` | `0x8021B530` | `0x24` |
| `pf_setencode.c` | `pf2_setencode` | `0x8021B560` | `0x24` |
| `pf_sync.c` | `pf2_sync` | `0x8021B590` | `0x28` |
| `pf_setupfsi.c` | `pf2_setupfsi` | `0x8021B5C0` | `0x2C` |
| `pf_setvol.c` | `pf2_setvol` | `0x8021B5F0` | `0x5C` |
| `pf_setclstlink.c` | `pf2_setclstlink` | `0x8021B650` | `0x28` |
| `pf_settailbuf.c` | `pf2_settailbuf` | `0x8021B680` | `0x28` |
| `pf_telldir.c` | `pf2_telldir` | `0x8021B6B0` | `0x24` |
| `pf_unmount.c` | `pf2_unmount` | `0x8021B6E0` | `0x28` |
| `pf_unregctx.c` | `pf2_unregctx` | `0x8021B710` | `0x24` |
| `pf_xdivide.c` | `pf2_xdivide` | `0x8021B740` | `0x90` |
| `pf_w_api_util.c` | `PFWAPI_ParseOpenModeString` | `0x8021B7D0` | `0xB4` |

The final wide parser is `PFWAPI_ParseOpenModeString` in `pf_w_api_util.c`, not a narrow public wrapper. The reference's separate next file and this target's next aligned entry place the boundary before `0x8021B890`, owned by #368. The earlier #366 wrapper inventory ends before this issue's first file.

## Interfaces and source fidelity

All source is ordinary C, built with established Wii/1.0 `cflags_sdk` and default 16-byte function alignment. Public wrappers preserve the reference's `pf2_` names, record failed string initialization in `pf_vol_set.last_error`, and call the existing PFAPI converters. Two-path operations initialize both local strings before testing the combined error. The directory opener preserves the reference's output-pointer convention, including an uninitialized local output on the initialization-error path; the converter selects NULL when the error is nonzero.

Headers declare only the forwarded contracts used here. `PF_CLUSTER` and `PF_TAIL_BUF` remain incomplete pointer-only types because these wrappers neither allocate nor inspect their storage. No size is asserted for either type. No `PF_VOLUME`, `PF_VOLUME_SET`, file, directory or cache member changes. The `PF_STR` and `PF_CHARCODE` declarations are imported by the volume header so its new prototypes compile under direct inclusion by cache and cluster consumers.

The original utility interface is owned by #365 and supplied by prerequisite PR #882. The additive lower-call declarations were handed off as bounded commit `8692a6f` for integration into that prerequisite, together with #366's independent declarations. The final source merge must use the merged canonical headers and repeat its latest-main gates; an approved scratch header copy is not a substitute for that dependency.

## Validation

On main base `9fdce911`, the first split-only full build and then the complete 25-unit split-only build both passed plain Ninja and the explicit frozen verifier. Registering all units as `NonMatching` produced actual Ninja objects whose complete native streams, all 75 paired relocations and every allocated objdiff section matched. No allocated data section is present in these units.

Selecting all 25 units as `Matching` then passed `python configure.py --map`, plain `.venv/Scripts/ninja` and the explicit frozen verifier, all with exit 0. The complete original 25-row inventory audit checked each native extent, all 1,608 bytes after independent relocation resolution, all 75 relocation offsets/kinds/targets/addends, the actual source entries in `build.ninja`, each function's final ELF address/size and source contribution in `main.elf.MAP`, every allocated section at 100%, and the entire rebuilt DOL against the unchanged original. The external reproducible audit is `367-audit-complete.py --source-selected`; its evidence is in `367-reference/complete-evidence.json`. This is complete source proof on the recorded base; exact committed-head verification after canonical-header integration and the final latest-main rebase remain required before merge.

## Traps

- Standard wrappers often have identical instruction shapes. Following the actual lower callee is necessary to distinguish mkdir from rmdir, move from rename, and directory read from directory close. A matching wrapper skeleton alone does not establish its name.
- The native `pf2_setupfsi` entry explicitly truncates its flags argument before calling `PFVOL_setupfsi`. A public `u16` parameter removed that instruction and shortened the function by four bytes. The evidenced public `u32` to lower `u16` contract reproduces the native `clrlwi` and complete `0x2C` extent.
- Wide mode-parser switch case order changes layout even when all results are equivalent. Placing the default return before the plus case, as in the narrow reference source, reproduces the target's complete `0xB4` body.
- Adding string-dependent declarations to the volume header requires its own include of the string interface. Relying on a wrapper's include order failed the actual cache and cluster compile; adding the canonical include fixed those direct consumers without changing storage.
