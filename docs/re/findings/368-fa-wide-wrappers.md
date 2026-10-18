# F-368-1: Wide standard FA wrappers and remaining file-lock entry

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. This is partial work on #368. Its original inventory has 25 functions and 2,636 native code bytes. The 24 complete wide-wrapper units below cover 2,616 bytes; `PF_InitLockFile` at `0x8021C330`, 20 bytes, remains original fallback. The issue is not complete.

## Original identity and source boundaries

All 24 original file/function names, order and extents agree with the separate FA entries in CC0 Wii IPL `config/43U/splits.txt` and `config/43U/symbols.txt`, pinned to `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`. Their target call graphs agree with the narrow wrappers. The source uses ordinary C and the established Wii/1.0 `cflags_sdk` with default 16-byte function alignment. Each function is a separate translation unit; its following alignment belongs outside the source split.

| Original file | Original function | Target address | Native extent |
| --- | --- | --- | --- |
| `pf_w_cdelete.c` | `pf2_w_cdelete` | `0x8021B890` | `0x7C` |
| `pf_w_chdir.c` | `pf2_w_chdir` | `0x8021B910` | `0x50` |
| `pf_w_chdmod.c` | `pf2_w_chdmod` | `0x8021B960` | `0x60` |
| `pf_w_chmod.c` | `pf2_w_chmod` | `0x8021B9C0` | `0x60` |
| `pf_w_cinsert.c` | `pf2_w_cinsert` | `0x8021BA20` | `0x7C` |
| `pf_w_combine.c` | `pf2_w_combine` | `0x8021BAA0` | `0x7C` |
| `pf_w_create.c` | `pf2_w_create` | `0x8021BB20` | `0x5C` |
| `pf_w_createdir.c` | `pf2_w_createdir` | `0x8021BB80` | `0x74` |
| `pf_w_cut.c` | `pf2_w_cut` | `0x8021BC00` | `0x88` |
| `pf_w_divide.c` | `pf2_w_divide` | `0x8021BC90` | `0x90` |
| `pf_w_fconcat.c` | `pf2_w_fconcat` | `0x8021BD20` | `0x7C` |
| `pf_w_fopen.c` | `pf2_w_fopen` | `0x8021BDA0` | `0x9C` |
| `pf_w_fsexec.c` | `pf2_w_fsexec` | `0x8021BE40` | `0x24` |
| `pf_w_fsfirst.c` | `pf2_w_fsfirst` | `0x8021BE70` | `0x74` |
| `pf_w_fsnext.c` | `pf2_w_fsnext` | `0x8021BEF0` | `0x24` |
| `pf_w_fstat.c` | `pf2_w_fstat` | `0x8021BF20` | `0x60` |
| `pf_w_insert.c` | `pf2_w_insert` | `0x8021BF80` | `0xC8` |
| `pf_w_mkdir.c` | `pf2_w_mkdir` | `0x8021C050` | `0x50` |
| `pf_w_move.c` | `pf2_w_move` | `0x8021C0A0` | `0x7C` |
| `pf_w_opendir.c` | `pf2_w_opendir` | `0x8021C120` | `0x58` |
| `pf_w_remove.c` | `pf2_w_remove` | `0x8021C180` | `0x50` |
| `pf_w_rename.c` | `pf2_w_rename` | `0x8021C1D0` | `0x7C` |
| `pf_w_rmdir.c` | `pf2_w_rmdir` | `0x8021C250` | `0x50` |
| `pf_w_xdivide.c` | `pf2_w_xdivide` | `0x8021C2A0` | `0x90` |

Readable bodies follow the independently reconstructed narrow wrappers from #365, #366 and #367, with original reference provenance recorded there. Public path and mode-string pointers become `const u16*`; string initialization uses `PF_CODE_MODE_UNICODE`; the file opener calls original `PFWAPI_ParseOpenModeString`. Lower operation contracts remain the same. In particular, `pf2_w_xdivide` calls original `PFFILE_divide`, shared with the public divide wrappers. There is no invented lower xdivide alias.

## Canonical interfaces

The temporary canonical header snapshot is the owner-approved #366 five-header commit `c5678d7f84e342cac073c8c0e168f92c9fbf049a` with correction `a38bf24cfdfb001be1eb3a54700105ca520a2784`; original PFAPI declarations come from #365's approved `107ccacc` interface, and the wide parser declaration comes from #367. These prerequisites are being integrated through PR #882 and PR #887. Their duplicate header changes must disappear when the branch rebases onto the merged canonical interfaces, followed by the required clean latest-main gate. No file, directory, volume, cache or disk storage layout changes.

## Remaining first-owned file

The original `pf_filelock.c` begins with #368's `PF_InitLockFile` and continues with #369's `PF_LockFile` at `0x8021C350` and `PF_UnLockFile` at `0x8021C410`, ending at `0x8021C4A0`. Issue #369 is already assigned to another contributor. Its bodies and source contribution are untouched here, and no claim override was attempted. The first-owned file still needs complete shared-source cooperation before accepting its last #368 function.

The native initializer stores zero to the existing system object's `flock_count` at offset `0x2398`, using base `0x8041F170`. Both target metadata and the pinned original FA map report `pf_sys_set` size `0x23A0`; the related VF structure is only four bytes and cannot supply this target's complete layout. No system-object extent, storage definition or guessed offset alias is introduced. The later F-368-2 proof supplies the target-proven canonical layout; cooperation on the complete file remains necessary for source acceptance.

## Validation

On main base `efc0c5c7`, the initial 24-unit split-only `python configure.py --map`, plain Ninja and explicit frozen verifier all exited 0. Actual Ninja-built `NonMatching` objects then passed all 24 original row/name/file/extent checks, 2,616 native bytes after independent relocation resolution, all 123 paired relocation offsets/kinds/names/addends, no extra allocated data, and every allocated objdiff section at 100%. The source-selected build repeats those checks and additionally verifies all 24 actual source linker inputs, their final ELF addresses and sizes, source map contributions and the complete DOL against the unchanged frozen original. The bounded audit is `368-audit-wide.py --source-selected`; it deliberately reports 24/25 issue rows, never full issue completion.

## Traps

- Historical shared-document updates can reappear during a conflict-free union rebase. The combined branch's prebuild consolidation audit caught 24 obsolete duplicate owned wide ledger rows, one stale directory-status row, 26 missing current-main #366 rows and omitted unrelated symbol-record sections. Reconciliation preserves the complete actual-main ledger and symbol records verbatim, appends exactly 65 unique owned ledger rows in native order and retains all five owned symbol sections once. All 51 previously accepted source files remain byte-identical to their preserved snapshots; a successful textual rebase alone is insufficient shared-document validation.
- Preserving source text alone does not preserve source selection. The broader prebuild audit also caught 27 missing current-main object registrations and splits, plus 73 reverted outside symbol names, inherited from the consolidated branch's historical changes. The bounded correction retains every one of actual main's 583 object registrations, all existing split blocks and 33,058 symbol addresses, with no symbol-attribute changes, then adds only the 65 owned units and 91 owned or required placeholder-to-proven-name updates. Current-main metadata, ledger and symbol records are explicitly checked before acceptance. A full DOL comparison would not by itself reject a regression from matching source to original fallback.

- A relocation alias at the same address can conceal a wrong lower name. Strict pairing rejected the temporary lower `PFFILE_xdivide` alias inherited from #367; the exact original name is `PFFILE_divide`. The full audit checks every lower name against the original FA namespace.
- The narrow public mode/attribute arguments retain their 32-bit type in wide counterparts. Only path and mode-string pointers change character width. For example, lower `u8` attribute truncation must remain visible in the generated code.
- `pf2_w_create` preserves the native uninitialized output local on the string-initialization error path. Adding a defensive initializer changes the native body; the reference string initializer currently returns success, but acceptance still requires reproducing the actual instructions.
- The four-byte VF system-set record cannot establish this FA system object's leading `0x2398` bytes. Native counter-offset evidence does not justify changing an assigned neighbor's storage or publishing an invented complete record.

# F-368-2: Typed system-set layout from kernel consumers

This later result strengthens the earlier offset-only observation and establishes the bounded canonical interface described below, without defining repository storage. The native stub initializer at `0x8021C4A0` supplies enough independent kernel-call evidence to reconstruct the leading work area with existing canonical SDK types. No raw offset alias, fabricated padding field or opaque prefix is needed.

| Field or object | Native offset | Evidence |
| --- | --- | --- |
| initialization flag | `0x0` | initializer tests the word, then stores 1 |
| `OSThread*` task handle | `0x4` | result from task creation is stored here, then passed to task start |
| `OSMessageQueue*` mailbox handle | `0x8` | mailbox creation result is stored here and later passed to receive/send wrappers |
| byte stack, `0x2000` bytes | `0xC` | task-creation record supplies this base and exact size |
| embedded `OSThread` | `0x2010` | task-creation record supplies this storage; kernel wrapper calls `OSCreateThread` and returns the same pointer |
| embedded `OSMessageQueue` | `0x2328` | mailbox-creation record supplies this object; kernel wrapper calls `OSInitMessageQueue` |
| `OSMessage messages[20]` | `0x2348` | mailbox-creation record supplies this array and count 20 |
| lock counter | `0x2398` | `PF_InitLockFile` zeroes this word; native lock consumers update it |

The native task kernel wrapper at `0x8021DFE0` passes the supplied thread object directly to `OSCreateThread` and returns it on success. The mailbox wrapper at `0x8021E150` passes its supplied queue, message array and count to `OSInitMessageQueue`, then returns that queue pointer. Their original names, `pfk_create_task` and `pfk_create_mailbox`, are separately corroborated by the pinned FA map; no bodies from their neighboring issues were implemented.

The actual Wii/1.0 compiler reports canonical `OSThread` size `0x318` and `OSMessageQueue` size `0x20`. A normal record containing the first seven rows has size `0x2398`. Thread alignment naturally inserts four bytes after the byte stack, placing the thread at `0x2010`. A containing system-set record with that stub-control record followed by `u32 flock_count` has size `0x23A0` and alignment 8, naturally including its trailing alignment. Those values agree with both the existing target object and the original FA map. The next native BSS object starts at `0x80421510`, immediately after `0x8041F170 + 0x23A0`.

An external C probe using exactly those typed members compiles `PF_InitLockFile` to all 20 native bytes after independently resolving its two HA/LO relocations to the actual system-set base. The same scratch object defines `pf_sys_set` and emits exactly `0x23A0` NOBITS bytes with alignment 8. Its temporary 36-byte diagnostic offset array is solely a layout measurement, not proposed production data. The probe is `368-system-layout-probe.c`; `368-audit-system-layout.py` checks all nine reported offsets/sizes, the complete initializer instructions, both relocation targets and the BSS extent. Results are retained in `368-reference/system-layout-evidence.json`.

The canonical `pf_system.h` now contains the two typed records and an external declaration of `pf_sys_set`; it preserves `PFSYS_initializeSYS` and introduces no storage definition. `pf_filelock.h` declares only `PF_InitLockFile`, and its C source implements only that initializer. The full original `pf_filelock.c` text range, `0x8021C330–0x8021C4A0`, is registered `NonMatching`. Consequently, the entire unit remains original fallback and these changes add zero linked source bytes. Neither assigned #369 body is implemented or selected. Existing object metadata is renamed to original `pf_sys_set` without changing its extent or attributes.

The actual repository initializer was compiled separately with the actual canonical header. Its complete text and both relocation records agree with the independently resolved 20-byte probe, and it has no allocated data. The layout diagnostic was recompiled using that same canonical header, rather than a repeated local type definition. Both checks exit 0 through `368-audit-system-layout.py`.

At committed head `5ac363cf90b43a1bd3af53f14b2d404fa791261a`, `python configure.py --map`, plain `.venv/Scripts/ninja -j4` and the explicit frozen verifier all exited 0. `368-audit-wide.py --source-selected` repeated the complete 24-wrapper proof: 2,616 native bytes, 123 paired relocations, actual source linker inputs and map contributions, every allocated section at 100%, and frozen DOL identity. `368-audit-ninja-init.py` separately checked the actual Ninja-built initializer's 20 native bytes, both resolved relocations and absence of allocated data; the actual linker rule still selects the entire original file-lock object. This is partial evidence, with zero additional linked bytes from that file. Canonical prerequisite integration and latest-main verification remain necessary before merge.

Issue #369 subsequently became unassigned and was legitimately claimed through the helper by this worker. Its two file-lock bodies now have independent external native proof, but have not been added here; the earlier assigned-owner restriction and partial-unit status above describe the state when this bounded initializer was introduced.

# F-368-3: Complete shared file-lock source

After #369 was legitimately claimed, both remaining bodies were added to the first-owned `pf_filelock.c` in native order. Canonical `PFK_SEMAPHORE_PARAMS` and the actual merged kernel declarations replace the external probe's temporary configuration types. The existing `PF_LOCK` members, embedded SDK semaphore, canonical typed system-set declaration and complete original object extents remain unchanged. No system storage is defined here.

The public lock function has two parameters: the file handle and an unused owner handle. Actual callers at `0x80210430` and `0x80210480` explicitly supply `NULL` and the file handle as the second argument. The related VF one-argument declaration was rejected before tracked publication. The unlock function takes only the file handle. The native caller audit is `369-filelock-callers.py`, independently retained with the claimed #369 investigation.

On committed `11c268e8`, the actual Ninja-built `NonMatching` object reproduces all three functions: initializer 20 bytes, lock 188 bytes and unlock 144 bytes. Every relocation offset, kind, target name and addend pairs with the original object. All allocated objdiff sections are 100%, including all 368 text bytes with 16 internal padding bytes, and there is no allocated data. Configure, plain Ninja and the explicit frozen verifier exit 0. `368-audit-filelock.py` checks these actual objects independently of the external probe.

At source-selected committed head `37b6f373da1f27a0d7e2aa1b5cf44229c67a1f73`, configure, plain Ninja, explicit frozen verifier and `368-audit-complete.py` all exit 0. The complete #368 inventory is the 24 wide wrappers plus the initializer: 25 functions and 2,636 native code bytes, all source selected at their exact final addresses and source-map contributions. The complete shared file-lock object is also source selected, with the same all-section proof. The two additional shared-file bodies contribute 332 bytes to #369 once; they do not complete that separate issue and receive no duplicate credit. No special compiler settings or uncertain operation-field qualifier enter this work. Final merge still requires the canonical #887 parser prerequisite and exact latest-main gates.

# F-368-4: Consolidated narrow, wide and system delivery

The separately verified owner branches are consolidated into PR #895 to avoid repeated source-metadata rebuilds. Exact preserved snapshots are #367 `1a5afdbf3ec712691b70139695fcced48197b4c7`, #368 `2dcb5f7f2f8319c4a46ad04872d77a091ca10333`, and partial #369 `cba3839451ed1f44e0b6fa46bb5f770407f6f9ea`. The original individual PRs stay open until the combined merge and both complete-issue closures are confirmed. The final tree now includes the wide parser itself, eliminating the separate parser-header dependency; typed system-set declarations have one canonical definition.

`368-audit-consolidation.py` verifies that all 51 actual source files are byte-identical to those recorded snapshots, each has exactly one matching object registration and ledger row, and the stale unrelated directory ledger row is absent. The complete original inventories remain separate: #367 has 25 functions/1,608 bytes and #368 has 25 functions/2,636 bytes. Partial #369 contributes five functions/484 bytes: the two shared lock bodies plus three system routines. Total distinct source coverage is 55 functions/4,728 native instruction bytes, 51 source units and the unchanged `0x23A0` system BSS. These counts do not include any remaining #369 fallback or duplicate either shared function.

The combined tree still needs a fresh actual build after final latest-main integration. `368-audit-combined-issues.py` will repeat the complete #367 and #368 original-inventory audits, the actual system source/object/data/relocation/map audit, and a strict #369 audit asserting five accepted entries/484 bytes versus 17 remaining fallback entries/3,140 bytes. Historical individual gates are retained as evidence and do not substitute for that final combined proof. The uncertain internal operation qualifier and special inlining flags remain absent from tracked source.

## Traps

- A union merge can resurrect an obsolete ledger row again while consolidating a later independently rebased branch, even after each earlier branch was cleaned. The combined-tree audit checks the unrelated directory row and every owned source row after all cherry-picks; retain main's current matching row and remove only the resurrected obsolete duplicate.
