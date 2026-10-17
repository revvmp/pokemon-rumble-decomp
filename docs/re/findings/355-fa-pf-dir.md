# F-355-3: FA `pf_dir.c`, first functions

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issues: #355 and #356 (partial). Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

`pf_dir.c` follows `pf_cluster.c` (F-355-1). Its split is `.text` `0x80201200`–`0x80205CC8`, `.sdata` `0x804B8858`–`0x804B8874` and `.sdata2` `0x804BA5B8`–`0x804BA5C0`. The unit is registered `NonMatching`, so the build links the original object. Twelve of its 38 functions are written; ten are identical. `python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` pass.

## Boundaries

- The start is the end of `PFCLUSTER_DeleteChain`.
- The `.sdata` range holds the literals `"\\"`, `"/"`, `""`, `":"`, `"*"`, `".."` and `"."`, in order of first use. The two `.sdata2` words are the byte constant `0xE5` (deleted entry).
- The end is tentative. The last API wrapper ends at `0x80205CC8`. The routine at `0x80205CD0` starts the entry code, which follows alphabetically (`pf_entry`). If later work shows that `0x80205CD0` and `0x80205EE0` belong here, extend the split.

## Status

| Function | Address | Status |
| --- | --- | --- |
| `PFDIR_IsEmptyDirectory` | `0x80201200` | 99.6%: the registers of the first name byte and the attribute load are swapped |
| `PFDIR_GetSDD` | `0x80201320` | 99.7%: `stat` is in `r0` instead of `r5` |
| `PFDIR_OpenDirEntry` | `0x80201430` | identical |
| `PFDIR_p_opendir` | `0x80201530` | identical |
| `PFDIR_p_readdir` | `0x80201820` | identical |
| `PFDIR_p_chdir` | `0x80202220` | identical |
| `PFDIR_p_chmod` | `0x802024A0` | identical |
| `PFDIR_p_rmdir` | `0x802026E0` | identical |
| `PFDIR_p_fstat` | `0x80202A10` | identical |
| `PFDIR_p_fsnext` | `0x80203F30` | 96.7%: the build hoists the constant 999999 into `r29`; the target reloads it |
| `PFDIR_p_fsfirst` | `0x80204240` | identical |
| `PFDIR_ChmodEntry` | `0x80204350` | identical |

Not yet written:

- `0x80201950` (directory creation)
- `0x80202BA0` and `0x80203570` (rename or move)
- the per-entry operations `0x80204550` (remove), `0x80204810` (open file) and `0x802048F0` (open directory)
- the wildcard dispatcher `0x802049C0`
- the eighteen API wrappers from `0x80204E20` to `0x80205BE0`

The dispatcher `0x802049C0` takes a `PF_DTA`, a mode and an attribute. Mode bits 2 and 4 select `PFDIR_ChmodEntry`, bit 1 the remove operation, bit 0x10 the open-file operation and bit 0x20 the open-directory operation. The API wrappers resolve the volume with `PFPATH_GetVolumeFromPath`, call `PFVOL_CheckForRead`, call the internal routine, and store any error in both `pf_vol_set.last_error` and `p_vol->last_error`.

## Names

VF names without `VFi`, medium confidence: `PFDIR_IsEmptyDirectory`, `PFDIR_GetSDD`, `PFDIR_p_opendir`, `PFDIR_p_readdir`, `PFDIR_p_chdir`, `PFDIR_p_chmod`, `PFDIR_p_rmdir`, `PFDIR_p_fstat`, `PFDIR_p_fsnext` and `PFDIR_p_fsfirst`.

Descriptive, low confidence: `PFDIR_OpenDirEntry` and `PFDIR_ChmodEntry`.

`PFDIR_GetFreeUDD` and `PFDIR_IsOpened` are static inline helpers; their code is inlined in every caller.

The callees named for the link are listed in `config/wpse-v1/symbols.txt` and [SYMBOLS.md](../SYMBOLS.md). They are VF names from the `PFENT`, `PFENT_ITER`, `PFFAT`, `PFPATH`, `PFVOL` and `PFFILE` modules.

## Layouts

These are in `include/revolution/fa/pf_volume.h`, `pf_dir.h` and `pf_str.h`.

- `PF_SDD` is 0x280 bytes: `stat` at 0, `num_handlers` at 4, `ffd` at 8, `dir_entry` at 0x40. `PF_VOLUME.sdds` starts at 0xE3C.
- `PF_DIR` is 0x20 bytes. `PF_VOLUME.udds` starts at 0x15BC.
- `PF_VOLUME`:

  | Offset | Field |
  | --- | --- |
  | 0x161C | `num_opened_files` |
  | 0x1620 | `num_opened_directories` |
  | 0x1F80 | `last_error` |
  | 0x1F84 | `last_driver_error` |
  | 0x1F88 | `file_config` |
  | 0x1F8C | `flags` |
  | 0x1F8E | `drv_char` |

- `PF_VOLUME_SET`: `last_error` at 0x40, `last_driver_error` at 0x44, `codeset` at 0x48, `setting` at 0x60. Bits 1 and 2 of `setting` are the code mode.
- `PF_DTA` keeps the VF member order:

  | Offset | Field |
  | --- | --- |
  | 0x8 | `p_vol` |
  | 0xC | `start_cluster` |
  | 0x10 | `index` |
  | 0x14 | `status` |
  | 0x1C | `pattern` |
  | 0x224 | `modify_time` |
  | 0x228 | `file_size` |
  | 0x22C | `attr` |
  | 0x22D | `short_name` |
  | 0x23A | `long_name` |
  | 0x444 | `w_pattern` |
  | 0x64C | `w_short_name` |
  | 0x666 | `w_long_name` |

## Traps

- **An operator-precedence bug in the original is load-bearing.** Trigger: `PFDIR_GetSDD` emits `cntlzw` followed by `rlwinm 27,30,30` for the opened test. Only `!stat & PF_SDD_STAT_OPENED` reproduces it. Evidence: F-355-3.
- **Loops over descriptor tables need pointer iteration.** Trigger: the inlined `PFDIR_IsOpened` loop in `PFDIR_p_rmdir` and `PFDIR_ChmodEntry` matched only with `p_sdd++` in the increment list. Indexing `sdds[i]` produced different address arithmetic. Evidence: F-355-3.
- **The error tail `neg; or; srawi; and` comes from two returns.** Trigger: it appears after `PFENT_RemoveEntry` calls. `if (err != 0) return err; return 0;` reproduces it. Evidence: F-355-3.
- **Bash heredocs that contain Python with quotes fail to parse.** Trigger: here-documents holding multi-line Python strings ended with `unexpected EOF`. Write the script to a file first, then run it.
