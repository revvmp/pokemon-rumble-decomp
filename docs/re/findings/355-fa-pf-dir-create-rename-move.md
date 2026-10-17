# F-355-4: FA `pf_dir.c` directory creation, rename and move

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #355 (partial). Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

The three largest functions in #355 are now written, and each is identical to the target. `PFDIR_p_fsnext` (F-355-3) is identical too. `pf_dir.c` stays `NonMatching` because 23 of the #356 functions, from `0x80204550` on, are not written. `python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` (exit 0) pass.

| Function | Address | Size | Status |
| --- | --- | --- | --- |
| `PFDIR_p_mkdir` | `0x80201950` | `0x8C8` | identical |
| `PFDIR_p_rename` | `0x80202BA0` | `0x9C8` | identical |
| `PFDIR_p_move` | `0x80203570` | `0x9C0` | identical |
| `PFDIR_p_fsnext` | `0x80203F30` | `0x304` | identical |

objdiff reports `pf_dir.c` at 13 of 38 functions matched. In #355, only `PFDIR_IsEmptyDirectory` (99.6%) and `PFDIR_GetSDD` (99.7%) in this unit, and `PFCLUSTER_DeleteChain` (99.4%, F-355-2), still differ.

## Behaviour

- `PFDIR_p_mkdir(p_vol, p_path, mode, p_dta)` creates a directory. Both API wrappers pass a third argument, but the function never reads it. When `p_dta` is not null, it fills the search record as `PFDIR_p_fsfirst` does. It writes the dot and dot-dot entries itself; VF's `FillVoidEntryToSectors` does this in a separate function. Dot-dot gets cluster 0 only when the parent's `entry_sector` is `0xFFFFFFFF`. VF also tests for cluster 1. Error codes: 20 (parent is not a directory), 2 (path longer than 259), 1 (name longer than 255 or invalid), 8 (exists), 6 (no cluster) and 17 (short write).
- `PFDIR_p_rename(p_vol, p_old, p_new)` renames an entry in its own directory. It returns 31 when the new path names another drive and 32 when the new parent differs. When neither name needs LFN entries, it rewrites only the short entry. Otherwise it marks the old entries `0xE5`, allocates new ones, and restores the old entries if allocation fails.
- `PFDIR_p_move(p_vol, p_src, p_dst)` moves an entry to another directory. It returns 0 when the destination is the source entry itself. It returns 10 when it finds the source cluster while walking the destination's `..` chain, which means the move would put a directory inside itself. After a move, it rewrites the moved directory's `..` entry. It writes cluster 0 there when the new parent's cluster is at most 1, or is 2 on FAT32.

## Names

Low confidence, descriptive: `PFDIR_p_mkdir`, `PFDIR_p_rename` and `PFDIR_p_move`, after PrFILE2's `pf_mkdir`, `pf_rename` and `pf_move`. API wrappers call them at `0x802057F8`, `0x802058E8`, `0x8020519C` and `0x8020528C`.

Callees named for the link:

| Name | Address | Basis | Confidence |
| --- | --- | --- | --- |
| `PFENT_CalcCheckSum` | `0x802065B0` | VF name; its result is stored as the entry checksum | medium |
| `PFENT_StoreEntryNumericFieldsToBuf` | `0x80206720` | VF name; called after each dot entry, as in VF | medium |
| `PFENT_storeLFNEntryFieldsToBuf` | `0x802069A0` | VF name; `(buf, p_ent, ord, sum, is_last)` | medium |
| `PFENT_SearchEntry` | `0x80206DD0` | Wraps `PFENT_findEntry` and discards both positions | low |
| `PFENT_allocateEntry` | `0x80206E00` | VF name; FA has no `attr_required` argument | medium |
| `PFENT_AdjustSFN` | `0x80207420` | VF name; follows `PFENT_UpdateSFNEntry`, as in VF | medium |
| `PFENT_ITER_IsInDirectory` | `0x80208930` | Called for each open file to test whether it lies under the directory | low |
| `PFENT_ITER_Retreat` | `0x80208CC0` | VF name; steps back over each LFN entry | medium |
| `PFENT_ITER_FindCluster` | `0x80208F20` | VF name and argument order | medium |
| `PFENT_ITER_FindEntryOfPath` | `0x80209260` | Looks a name up in the iterator's directory; handles drive and root forms first | low |
| `PFFAT_GetSectorSpecified` | `0x8020B290` | VF name; converts `may_allocate` to 0 or 1, as VF does | medium |
| `PFPATH_AdjustExtShortName` | `0x80214E50` | VF name; same digit loop from index 7 | medium |

`PFENT_SetDotEntry` and `PFENT_SetDotDotEntry` are VF's names. Here they are static and inlined into `PFDIR_p_mkdir`. Two static inline helpers, `PFDIR_IsOpened` (F-355-3) and the new `PFDIR_IsCurrentDir`, are inlined into both rename and move.

## Layouts

- `PF_FFD` offset 4 is `start_cluster`, as in VF, where it is at 0. `PFDIR_p_rename` points the FFD at this slot and stores the parent cluster in it. F-355-1 called it `reserved`.
- `PF_VOLUME.current_dir` is `PF_CUR_DIR[4]` at `0x1654`: `stat` at 0, `context_id` at 4 and `dir_entry` at 8, 0x248 bytes each. `PFVOL_IsCurrentDirCluster` (`0x802163F0`) uses the same layout. The 12 bytes after it, `tail_entry`, are VF's `PF_DIR_TAIL` slot and are opaque.
- `PF_VOLUME_SET.current_vol` is `PF_CUR_VOLUME[4]` at 0: `stat`, `context_id` and `p_vol`, 12 bytes each. `PFDIR_IsCurrentDir` scans it. The next 0x10 bytes stay opaque.
- `PF_DTA.reserved[0]` and `[1]` are cleared by `PFDIR_p_mkdir`; nothing reads them.

## Traps

- **Register coloring follows declaration order, not use order.** Trigger: all three functions had the right instructions and the wrong callee-saved registers. A random search over the order of the scalar declarations (`tune355.py`, kept outside the repository) found an order with zero differences each time. Stack offsets depend only on the order of the arrays and structures, which is the reverse of their offsets. Splitting a reused loop counter into separate variables (`i`, `j`, `ord`, `dir_sector`) was needed before any order worked. Evidence: F-355-4.
- **A `switch` produces the dead branch after an early return.** Trigger: after `PFENT_SearchEntry`, the target tests 0, then 3, and has an unreachable `b` after `li r3,8`. `switch (err) { case 0: return 8; case 3: break; default: return err; }` reproduces it; `if`/`else if` does not. Evidence: F-355-4.
- **`bne` straight to the epilogue comes from a nested `if` with `else return err`.** Trigger: inside the `..` walk in `PFDIR_p_move`. `if (err == 0) { if (...) return 10; } else { return err; }` produces it; `if (err != 0) return err;` adds a `beq; b` pair. Evidence: F-355-4.
- **A `while` that the target leaves unrotated is a `for (;;)` with a `break` test first.** Trigger: the `..` walk in `PFDIR_p_move`. Evidence: F-355-4.
- **`PFDIR_p_fsnext` runs its loop body at most once.** Trigger: after a successful search, the target leaves the loop. A `break;` after the `ppos == 999999` test reproduces it. Its constant then stays in `r0` instead of being hoisted into `r29`. The zeros for `lpos` and `ppos` must come before `i = 0`. Evidence: F-355-4.
- **Inlined dot-entry helpers set the order of constant loads.** Trigger: with the eleven stores written out inline, the target loaded `' '` before `'.'`. Only VF's loop helpers, inlined, reproduce the order. Evidence: F-355-4.
- **The LFN restore step masks with `0x1FE0`.** Trigger: both rename and move emit `clrlslwi r3,r3,24,5` for `num_entry_LFNs * 32`. A plain multiply gives `slwi`, and a `(u16)` cast gives mask `16..26`. Only an explicit mask, `(ent.num_entry_LFNs * 32) & 0x1FFF`, reproduces it. The original expression is unknown. Evidence: F-355-4.
- **`int` versus `u16` length variables change the compare and the division.** Trigger: rename and move compare the length with `cmpwi` and divide by 13 with signed code, so `len` is `s32` there. `PFDIR_p_mkdir` uses `cmplwi` and an unsigned divide, so its `len` is `u16`, and it divides by `13u`. Evidence: F-355-4.
