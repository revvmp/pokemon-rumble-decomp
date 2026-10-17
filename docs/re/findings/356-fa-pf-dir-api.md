# F-356-1: FA `pf_dir.c` completed

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issues: #356, and the last two #355 differences in this unit. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

All 38 functions of `pf_dir.c` (`.text` `0x80201200`–`0x80205CC8`) are now identical to the target, and the unit is `Matching` and linked. This adds the 23 functions from `0x80204550` and fixes `PFDIR_IsEmptyDirectory` and `PFDIR_GetSDD` (F-355-3). The unit boundaries of F-355-3 hold: the last API wrapper ends at `0x80205CC8`, and `0x80205CD0` starts `pf_entry.c` (F-356-2).

## Functions

| Address | Name | Confidence | Role |
| --- | --- | --- | --- |
| `0x80204550` | `PFDIR_RemoveEntry` | low, descriptive | `fsexec` remove operation |
| `0x80204810` | `PFDIR_OpenFileEntry` | low, descriptive | `fsexec` open-file operation; stores the handle in `PF_DTA.p_file` |
| `0x802048F0` | `PFDIR_OpenDirEntryForDTA` | low, descriptive | `fsexec` open-directory operation; stores the handle in `PF_DTA.p_dir` |
| `0x802049C0` | `PFDIR_p_fsexec` | low | the dispatcher F-355-3 described |
| `0x80204E20` | `PFDIR_FinalizeAllDirs` | high | VF `VFiPFDIR_FinalizeAllDirs` (Petari `src/RVL_SDK/vf/pf_dir.c`); called from the volume checks |
| `0x80204EB0`–`0x80205BE0` | `PFDIR_fsfirst`, `PFDIR_fsnext`, `PFDIR_fsexec`, `PFDIR_fstat`, `PFDIR_rename`, `PFDIR_move`, `PFDIR_opendir`, `PFDIR_closedir`, `PFDIR_readdir`, `PFDIR_telldir`, `PFDIR_seekdir`, `PFDIR_rmdir`, `PFDIR_mkdir`, `PFDIR_createdir`, `PFDIR_chdir`, `PFDIR_fchdir`, `PFDIR_chmod`, `PFDIR_chdmod` | low | the eighteen API wrappers, in address order, after the PrFILE2 API they wrap |

Each wrapper resolves the volume (`PFPATH_GetVolumeFromPath`, `p_dta->p_vol` or `p_dir->p_sdd->dir_entry.p_vol`), checks it, and stores any error in both `pf_vol_set.last_error` and `p_vol->last_error`. Writers clear `p_vol->cache.signature` before and after the call and flush the FAT cache, and all but `PFDIR_rmdir` and `PFDIR_fsexec` also flush the data cache. `PFDIR_mkdir` passes mode 0 and no DTA; `PFDIR_createdir` passes both. `PFDIR_chdmod` adds the directory attribute. `PFDIR_chdir` and `PFDIR_fchdir` call `PFVOL_SetCurrentVolume` after a successful change. `PFDIR_fchdir` calls a static `PFDIR_p_fchdir`, which is inlined.

`PFDIR_p_fsexec(p_dta, mode, param)` returns 10 unless `mode` selects an operation. Mode bits 2 and 4 select chmod, bit 1 remove, 0x10 open file and 0x20 open directory. Chmod, remove, and open file with `param != 2` need `PFVOL_CheckForWrite`. Bit `0x80000000` reuses the entry at `p_dta->index - 1`; otherwise the routine searches again from the first LFN entry and compares the short and long names. It refuses `.` and `..` with 28.

Callees named for the link: `PFVOL_CheckForWrite` (`0x802161F0`, VF name, follows `PFVOL_CheckForRead`, medium) and `PFVOL_SetCurrentVolume` (`0x80216890`, registers the volume for the current context, low). `PFFILE_OpenEntry` (`0x80210580`), `PFENT_ITER_FindEntry` (`0x802093D0`), `PFENT_ITER_GetEntry` (`0x80209440`), `PFENT_ITER_LoadLongName` (`0x802094B0`), `PFPATH_CmpLongName` (`0x80212D90`) and `PFPATH_CmpShortName` (`0x80212DC0`) are descriptive, low. `VFiPFPATH_getLongNameformShortName` takes a third argument, the entry's small-letter flags.

## Layouts

`PF_DTA` offsets 0 and 4 are `p_file` and `p_dir`. F-355-1 called them `reserved`: `PFDIR_p_mkdir` clears them, and the open operations store the new handles there.

## Evidence

`python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` (exit 0) pass with `pf_dir.c` linked from source. Every function's instruction stream equals the target's.

# F-356-2: `pf_entry.c` boundaries and first functions

`pf_entry.c` is split as `.text` `0x80205CD0`–`0x802095D4`, `.data` `0x803C4258`–`0x803C4264`, `.sdata` `0x804B8878`–`0x804B8890` and `.sdata2` `0x804BA5C0`–`0x804BA5C8`. It is registered `NonMatching`, so the build links the original object.

- The `PFENT_ITER_*` routines belong to this unit. `PFENT_LookupPath` and `PFENT_ITER_IsInDirectory`, `PFENT_ITER_FindEntryOfPath` and `PFENT_ITER_Retreat` read the same `.sdata` strings (`..`, `:`, `\` and others at `0x804B8878`–`0x804B888C`), and CodeWarrior does not share literals across units. Petari's VF keeps them in a separate `pf_entry_iterator.c`.
- The `.text` end is the end of `PFENT_ITER_LoadLongName`. The next routine, `0x802095E0`, reads the FAT tables at `0x80368188` and starts `pf_fat.c`.
- The only `.data` object is the 12-byte string at `0x803C4258`, read at `0x80207290`. The `.sdata2` object is the deleted-entry byte array read by `PFENT_RemoveEntry`.

Three functions are written, and all three are identical:

| Address | Name | Confidence |
| --- | --- | --- |
| `0x80205CD0` | `PFENT_IsNotMatchedEntry` | low, descriptive. Returns 1 to skip an entry: LFN slots, attribute mismatches and name mismatches; counts name mismatches |
| `0x80205EE0` | `PFENT_LoadMatchedEntry` | low, descriptive. Returns -1 for a skipped entry, otherwise loads the short entry as VF's `VFiPFENT_loadEntryNumericFieldsFromBuf` does |
| `0x802065B0` | `PFENT_CalcCheckSum` | VF name. Belongs to #357, written here because both functions above inline it |

Callees named: `PFENT_LoadLFNEntryFieldsFromBuf` (`0x802067C0`, VF name, two arguments here), `VFiPFPATH_putShortName` (`0x802136A0`) and `VFiPFPATH_getShortName` (`0x80213830`), VF names, medium. The rest of the unit belongs to #357 and later block issues.

# F-356-3: Remaining #355 differences

- `PFDIR_IsEmptyDirectory` is identical once the attribute byte is a local read inside a nested test. See the first trap.
- `PFDIR_GetSDD` is identical with the redundant allocation test that Petari's `VFiPFFILE_GetSFD` uses. See the second trap.
- `PFCLUSTER_DeleteChain` still swaps `r24` and `r25` (offset and size). An exhaustive search over the order of its nine declarations (3,024 interleavings of the four register locals with the stack locals) found no order below the current 24-instruction difference. Removing `size` or `offset` shifts every register.

## Traps

- **A byte read twice can need its own local.** Trigger: `PFDIR_IsEmptyDirectory` had `buf[0]` and `buf[11]` in swapped registers. `attr = p_iter->buf[11];` inside `if (p_iter->buf[0] != PF_ENT_DELETED)` reproduces it; the local outside the nested test hoists the load. Evidence: F-356-3.
- **A redundant test in a condition changes register coloring.** Trigger: `PFDIR_GetSDD` loaded `stat` into `r5` instead of `r0`. `!(stat & 1) || ((stat & 1) && !stat & 2)` matches; the compiler removes the second `stat & 1`, but not its effect on allocation. Petari's `VFiPFFILE_GetSFD` has the same form. Evidence: F-356-3.
- **A loaded byte that seems unused may be a call argument.** Trigger: the open operations loaded `buf[12]` into `r5`, not `r0`. `VFiPFPATH_getLongNameformShortName` takes the flags as its third argument. Evidence: F-356-1.
- **Single-exit result variables keep the early `li` in a saved register.** Trigger: `PFENT_IsNotMatchedEntry` sets `r30` to 1 before each branch to the epilogue. Assigning `result = 1` in each early arm and returning `result` once reproduces it. Evidence: F-356-2.
- **A split ending at an unaligned object leaves a gap symbol across the boundary.** Trigger: after splitting `pf_entry.c`, the report step failed: `gap_09_804B8872_sdata size out of section bounds (32 > 28)`. Adding `align:8` to `lbl_804B8878`, the first `pf_entry.c` object, fixed it. Evidence: F-356-2.
- **Git Bash `sed -i` drops CRLF.** Trigger: `sed -i` on `pf_entry.c` and `pf_entry.h` rewrote them with LF endings. Use a script that writes CRLF, or check with `file` afterwards.
