# F-366-1: FA standard wrappers and shared interfaces

## Scope and identity

Issue #366 owns twenty-six functions / 1,540 code bytes at `0x8021AAA0–0x8021B184`. The complete retained sequence covers file adjustment, append, change directory, close, concatenate, EOF/error/info/lock/open, format, read/seek/cluster-link/search/stat/sync/write, device/volume/config queries, initialization, insertion and write protection. Exact original spellings, one-function filenames, sizes and order agree independently with the FA symbol map and splits of CC0 Wii IPL `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, `config/43U`. The sequence begins `pf_fadjust.c` and ends `pf_iswriteprotected.c`; `pf_getdev.c` exports the original name `pf2_devinf`. The adjacent owners cover different one-function units, so no neighboring file split is taken.

| Address | Size | Original exported name | Original file |
| --- | --- | --- | --- |
| `0x8021AAA0` | `0x24` | `pf2_fadjust` | `pf_fadjust.c` |
| `0x8021AAD0` | `0x28` | `pf2_fappend` | `pf_fappend.c` |
| `0x8021AB00` | `0x24` | `pf2_fchdir` | `pf_fchdir.c` |
| `0x8021AB30` | `0x24` | `pf2_fclose` | `pf_fclose.c` |
| `0x8021AB60` | `0x7C` | `pf2_fconcat` | `pf_fconcat.c` |
| `0x8021ABE0` | `0x24` | `pf2_feof` | `pf_feof.c` |
| `0x8021AC10` | `0x24` | `pf2_ferror` | `pf_ferror.c` |
| `0x8021AC40` | `0x24` | `pf2_finfo` | `pf_finfo.c` |
| `0x8021AC70` | `0x28` | `pf2_flock` | `pf_flock.c` |
| `0x8021ACA0` | `0x9C` | `pf2_fopen` | `pf_fopen.c` |
| `0x8021AD40` | `0x28` | `pf2_format` | `pf_format.c` |
| `0x8021AD70` | `0x28` | `pf2_fread` | `pf_fread.c` |
| `0x8021ADA0` | `0x24` | `pf2_fseek` | `pf_fseek.c` |
| `0x8021ADD0` | `0x24` | `pf2_fsetclstlink` | `pf_fsetclstlink.c` |
| `0x8021AE00` | `0x24` | `pf2_fsexec` | `pf_fsexec.c` |
| `0x8021AE30` | `0x74` | `pf2_fsfirst` | `pf_fsfirst.c` |
| `0x8021AEB0` | `0x24` | `pf2_fsnext` | `pf_fsnext.c` |
| `0x8021AEE0` | `0x60` | `pf2_fstat` | `pf_fstat.c` |
| `0x8021AF40` | `0x24` | `pf2_fsync` | `pf_fsync.c` |
| `0x8021AF70` | `0x28` | `pf2_fwrite` | `pf_fwrite.c` |
| `0x8021AFA0` | `0x28` | `pf2_devinf` | `pf_getdev.c` |
| `0x8021AFD0` | `0x28` | `pf2_getvol` | `pf_getvol.c` |
| `0x8021B000` | `0x28` | `pf2_getvolcfg` | `pf_getvolcfg.c` |
| `0x8021B030` | `0x40` | `pf2_init_prfile2` | `pf_init_prfile2.c` |
| `0x8021B070` | `0xC8` | `pf2_insert` | `pf_insert.c` |
| `0x8021B140` | `0x44` | `pf2_iswriteprotected` | `pf_iswriteprotected.c` |

Ordinary C candidates for all twenty-six functions reproduce every native instruction word, all 71 separately identified call/data relocation targets, and each native size using established Wii/1.0 `cflags_sdk`. The source has no assembly, storage aliases or extra flags. This local candidate checkpoint is diagnostic: actual split/source-selected full-link acceptance remains required before completing #366. VF source structure and headers are compared with CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe`; its root and file-level licenses were inspected. Original FA source is not asserted available in IPL.

## Bounded shared declarations

Five canonical FA headers expose only native-proven forwarding interfaces: existing `pf_dir.h`, `pf_file.h` and `pf_volume.h`, plus bounded Petari-derived `pf_fatfs.h` and `pf_system.h`. Every pre-existing declaration and storage record remains unchanged. The additions support the complete #366 candidates, the corrected twenty-five #367 wrappers and the fifteen remaining standard #365 wrappers. The PFAPI utility declaration comes only from its owner's approved canonical #882 header, with original PFAPI_convertDriverError and PFAPI_convertReturnValue4feof names; no competing copy is published here.

PF_INFO and PF_DEV_INF retain reference type names as opaque forwarding types. PF_VOL_INF, PF_VOLUME_CONFIG and PF_CLUSTER_LINK_CONFIG are forwarding-only tags; their original spelling is not established. No fields, size, alignment or storage are invented for them. Both native file cluster-link routine `0x80211380` and volume routine `0x80218B30`, in mode one, read the same widths at offsets zero/four/eight: u32/u32/u16. That independently supports sharing the descriptive opaque PF_CLUSTER_LINK_CONFIG tag. It is distinct from the existing internal PF_CLUSTER_LINK traversal state. The volume configuration output at `0x80219750` writes four words and is not conflated with the attach record.

PF_TAIL_BUF remains an opaque pointer type from the #367 interface. PF_DRV_TBL is forward-declared only for the typed lower attach call. The complete native-proven NAND path/size record belongs to #365's own pf_attach.h and is not imported into a volume storage definition. Character arguments remain signed where native extsb requires it; the lower buffering mode is u8, which reproduces the explicit mask in its #365 public wrapper.

## Consumer and baseline evidence

The final combined declarations compile all twenty-six #366 candidates with 1,540 bytes / 71 paired relocation targets exact, all corrected twenty-five #367 actual repository consumers with 1,608 bytes / 75 targets exact, and all fifteen #365 actual repository consumers with 1,424 bytes / 61 targets exact. These sixty-six consumers total 4,572 function bytes and 207 independently resolved relocation sites. Their source/header storage is unchanged by the declarations.

The original worktree baseline at `6965907bd8c2285c5f9096aed579af63a836c376` passed configure, plain Ninja and explicit frozen verification. An initial header checkpoint passed a fresh 513-target plain Ninja and explicit verification of actual `build/366-headers/wpse-v1`, all exit zero. All 506 existing compiled objects preserve every allocated section's type/flags/size/alignment/bytes and every canonical relocation target/addend; 505 are whole-file identical. The remaining pf_dir.o differs only in its string table, and a separate original-header control compilation reproduces its original whole-file SHA-256 exactly. The complete DOL is unchanged. This initial checkpoint predates the final corrected/expanded declarations: the owning #882 integration repeats its full current committed-head build and baseline gates before merge.

The final #882 integration on ea4ffb4742 repeats the fresh default build,
actual report, explicit all_source and immutable-DOL verification. Its
534-object inventory contains the two owned utility/chdir units and all
532 existing objects. Existing allocated sections and relocations are exact;
531 whole files are identical and the sole numeric-local pf_dir string-table
difference reproduces the original hash under an original-header control.
The final native dispatcher audit also refines detach to one signed-drive
argument before publication, as recorded in F-365-1's traps.

## Traps

- A matching instruction stream does not prove a borrowed API name. The VF subset initially mislabeled two #367 wrappers as detach and buffering, colliding with distinct #365 functions. The full pinned FA sequence identifies them as rmvvol and setclstlink; corresponding arities also differ. That header proposal was rejected and reverted before publication. Preserve the full original FA name/file/size order and validate actual forwarded argument registers rather than transferring a nearby VF name solely because bytes match.
- New volume prototypes use PF_STR and PF_CHARCODE even when a consumer first reaches volume through cache/cluster headers. The volume header must directly include pf_str.h; relying on its caller's later include fails standalone compilation.
- Do not reuse the existing internal PF_CLUSTER_LINK record for the public cluster-link configuration. Matching access widths at zero/four/eight support a shared opaque configuration pointer, not a claim that either public record has the internal traversal object's layout.

## Actual source-selected checkpoint

Each of the twenty-six original one-function units was split at its actual final instruction, retaining intervening alignment outside the source extent. The split-only fresh 513-target Ninja and explicit verification of actual `build/366-split/wpse-v1/main.dol` pass. The next NonMatching registration compiled all twenty-six ordinary C sources; all 1,540 bytes, 71 independently paired relocation targets and every allocated section match. Only then were the units selected as Matching. Plain Ninja and explicit actual-output frozen verification again pass with all twenty-six source inputs selected and their original fallback objects excluded. All twenty-six sections score 100%, every final function address/extent matches, and the complete 4,094,880-byte DOL has unchanged SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The complete original issue inventory is covered. All 506 pre-existing objects preserve every allocated section and canonical relocation, with 505 whole-file identical as explained above.

The canonical prerequisite #882 is now merged. All temporary PFAPI and shared-header commits were dropped during the source rebase onto main1d16cf44e80e9219de742c53e0969e1298c4982a; the final source diff adds only its own twenty-six API headers and no shared type/declaration changes. A fresh567-target full Ninja build of actual build/366-main-882/wpse-v1 passes, followed by explicit frozen verification and complete original-inventory/native/paired-relocation/source-selection/allocated-section/final-address audits. All534 canonical pre-existing source objects were independently copied from the verified prerequisite build and every whole-file SHA-256 is unchanged in the new build, including all allocated bytes and canonical relocations. The exact DOL hash remains the frozen value above. Final merge still requires the fresh latest-main ancestry and exact-head guard; #366 is counted only after confirmed merge and issue closure.
