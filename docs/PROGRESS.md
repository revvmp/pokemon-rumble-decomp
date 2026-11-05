# Progress

## Current status

Roadmap Phases 1 to 3 are complete and Phase 4 is active. The latest verified build matches the frozen main.dol exactly. It links 2,751,440 of 3,082,252 code bytes (89.2672%) from 884 Matching units; 12 real functions remain below 100%. Original split fallbacks still supply the remaining code and data. See the latest session below and [matching requirements](MATCHING.md).

| Milestone | State | Evidence or prerequisite |
| --- | --- | --- |
| Repository architecture and workflow | Initialized | `ARCHITECTURE.md`, `PROJECT_RULES.md` |
| Artifact verifier | Implemented | `tools/verify/verify.py` |
| Release identification | Identified | WPSE, USA, TMD version 1; F-0001; retail revision label and acquisition provenance unverified |
| Frozen reference manifests | Frozen | Package `config/targets/wpse-v1-package.json`; executable `config/targets/wpse-v1.json` (`main.dol`) |
| Artifact and section inventory | Complete for current scope | F-0005 extraction; both executables' sections in `re/INVENTORY.md`; boot loader role F-0009 |
| Toolchain identification | Identified per library | RVL_SDK Wii/1.0; MSL and Runtime Wii/1.1 or 1.3 with `-func_align 4`; NW4R Wii/1.0 or 1.1; F-0007 |
| Reproducible game build | Achieved | Two clean builds byte-identical; F-0010 |
| Exact executable match | Achieved for `main.dol` | Verified; remaining original split fallbacks are tracked separately |
| Source reconstruction | Started | 884 Matching units of 1,389 current units; see ledger |
| Full release match | Pending | Boot loader, data contents, and WAD packaging outside current scope |

## Current continuation: source-link acceptance

CFileData and CDlgMsgWin now pass fresh ninja and frozen main.dol verification from source. This adds 77,940 linked code bytes without changing the 71-function tail. CDlgMsgWin required correcting its Dismiss literal from `off` to the evidenced `end`, which function scoring alone did not reject. The whole executable verifier caught the difference.

Main and CBattleRoyalBox subsequently passed full source-link verification, adding another 27,872 linked code bytes. ScriptWorldObj then passed full source-link verification after preserving its two local duplicate constructors and original exception-analysis visibility. Two clean builds reproduced main.dol, main.elf and report.json exactly. SaveData, CGameSession, CGameEnding, Message, PhotoSave, errmsg, CNoticeDisplay and CMoneyLifeDisplay subsequently passed full source-link verification. CDlgPiiSelector has all 645 functions exact but still needs source-link ordering/data acceptance. ScriptWorldObj uses five tracked C++ source fragments compiled through the normal build and combined by the original linker with plain `-r`; no original object bytes feed that source output. CGameSession cleanup identities were corrected from exception-table consumers, including the separate ISession and IGameSession destructors.

## Latest function gains

The real unmatched tail is now 12, down from the initial 120. Matched code is 3,054,792/3,082,252 bytes (99.10909%). This checkpoint completes the collection Calc, CPii snapshot swap, filter Sort caller, three remote-memory functions, shadow-model setup, layout pane lookup, JPEG 4:1:1 converter, texture curve, particle calculation and archive-font analysis. Verified source-link coverage remains separate from function matching.

Several link failures were incorrect inferred identities: original vtable relocations prove swapped destructor names in CGameSession, errmsg and CNoticeDisplay. Correcting them removed unnecessary source-fragment experiments. Errmsg also required recovering its NErrMsg namespace from embedded type names, preserving two separately compiled float pools, and the evidenced16-byte start alignment of the following homebutton function. Frozen hashes remain unchanged.

## 2026-11-04: Twelve-function checkpoint

CShadowCamera, CSimpleGround, CPii, CPiiBox, CRemoteMemory, ArchiveFontBase, ParticleManager and RGBA8 now pass full source-link verification. The entire RVL_SDK category is matched and linked from source. Fresh native ninja exited zero, frozen main.dol verification passed, and the original-address audit found no previously exact function regressions or missing exact functions. All 1,389 native-link input aliases remain byte-identical to their selected objects. Linked data is 1,681,464/1,788,746 bytes (94.00239%).

Separate Camera/Ground and CPii/Box source boundaries account for the new unit and alignment inventory; relative to the prior checkpoint the denominator gains 20 code-padding bytes and loses 23 data-padding bytes. No executable bytes or expected hashes changed. CPii's multiplier and CLayout's archive-root/motion literals were corrected from original relocation-backed data. CLayout has all 171 exact function bodies and exception records but remains NonMatching pending ten boolean-literal relocation bytes; CModel, CDlgPiiSelector and CDlgCollection also await final source-link acceptance.

The shared-header audit caught and fixed two ParticleManager-related caller regressions that a comparison against an already edited checkout had missed. The reusable lesson is recorded in AGENTS.md and [the particle/font findings](re/findings/particle-font-native-tail-20261104.md#traps). This checkpoint uses an incremental production build; the preceding 24-function checkpoint has the latest repeated clean-build proof. See [validation](VALIDATION.md#twelve-function-checkpoint).

## Progress images

The README shows images rendered by `tools/progress/progress.py`, the project's own visualizer. It replaces hosted decomp.dev badges, which the project owner reports are being sunset ([D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository)). Regenerate and commit the images whenever phase state or measured progress changes:

```powershell
python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images
```

| Image | Source | Shows |
| --- | --- | --- |
| `docs/images/progress-roadmap.svg` | `config/progress.json` | Each [roadmap](ROADMAP.md) phase as done, in progress, or not started |
| `docs/images/progress-code.svg` | objdiff report, when given | Matched code, code linked from source, matched functions, matched data, and per-category code; without a report it states that progress is not measured |
| `docs/images/progress-treemap.svg` | objdiff report, when given | Translation units sized by code bytes and shaded by match percentage in seven bins |

Render with the real report after a successful build:

```powershell
python tools/progress/progress.py render --phases config/progress.json --report build/wpse-v1/report.json --output-dir docs/images
```

Phase states in `config/progress.json` must agree with the milestone table above and the roadmap gates. States must run done, then at most one active, then pending; the tool rejects anything else. The tool refuses a report with zero total code. Per-category rows appear only when progress categories cover all code; today almost every unit is an uncategorized dtk placeholder. The treemap is opt-in with `--treemap` and stays out of the README until real translation-unit splits replace the 8,781 per-function placeholders, which would otherwise render as noise. "Matched" means source compiles to identical code; "linked from source" means the final build uses that source rather than an original fallback object. Only the linked figure approaches source coverage, and it still requires the ledger review described in [matching](MATCHING.md).

Images use a single blue sequential ramp with separate light and dark steps selected through `prefers-color-scheme`, embed hover titles that work when an SVG is opened directly, and contain no timestamps, so identical inputs produce identical bytes.

## Reconstruction ledger

The ledger is kept per area in [ledger](ledger/README.md). It records source ownership and evidence per unit. Current measured linked coverage is reported above; original split dependencies still count as fallback.

## Latest session

2026-11-04 function checkpoint: six more real functions are exact without regressions. RGB565 now links all seven functions from source and passes the frozen executable verifier; it adds 3,084 linked code bytes. The other newly exact functions remain in NonMatching units until their remaining functions and complete source links are accepted. The U8 animation interpolation helper also corrects the proven reversed `(c0 + c1) - 2.0f` term; that semantic correction does not change its fuzzy score and does not count as another exact function. Original inputs and hashes are unchanged. Evidence: [JPEG](re/findings/tmc-jpeg-final-tail.md), [sort](re/findings/sort-tail-20261104.md), [layout](re/findings/20261104-layout-remote-tail.md), [shadow material](re/findings/20261104-game-small-tail.md), and [animation interpolation](re/findings/animcurve-20261104-continuation.md).

2026-11-04 CUIMenu source-link acceptance: all 61 functions now come from source through the ordinary configure/ninja entry point. Native compiler sections and native linker selectors resolve the final adjacent function-order difference; all 1,385 uniquely named linker inputs are byte-identical copies of their selected inputs. Ninja exited 0 and frozen `main.dol` verification passed. Linked code increased by 11,812 bytes; linked data is 1,660,427/1,788,769 bytes (92.82512%). No previously exact real function regressed, and the unmatched count remains 30. The fresh starting report had 874 completed units, correcting the preceding prose count of 873. The CUIList cleanup at `0x801DC380` is now the natural list-item vector template destructor, replacing a documented stand-in. See [CUIMenu](re/findings/cuimenu-native-sections.md) and [cleanup identity](re/findings/337-cuilist-tail.md#f-337-7-natural-list-item-vector-cleanup). Two subsequent clean builds including all six function gains and RGB565 reproduce main.dol, main.elf and report.json byte-for-byte; both frozen verifications pass. See [validation](VALIDATION.md#24-function-checkpoint-and-native-section-linking).

2026-11-04 data continuation: BModelObj, CMiniMap and CPauseMenu now pass full source-link verification. Camera, species/model names, motor patterns, post-effect, stage, motion and effect tables are ordinary typed C++ definitions with compiler relocations. MSL character maps and the AX compressor table are typed C arrays. The C locale pointer graph and seven typed BMG language resources also pass full source linkage. Fourteen new data units remove 82,960 bytes of fallback data. The report records 1,643,397 linked data bytes (91.870094%); data denominators can change when explicit splits separate alignment padding. New function gains are CMiniMap CalcView, both RGB565 MCU converters and both remaining Pause functions. BWorld Calc/CalcMtx and CLayout ReplaceMotions/DetachAttachedLayouts subsequently became exact; BWorld, homebutton and TextWriterBase subsequently passed full source-link acceptance. NPpuVM destructor, CGameResult Calc, the collection-item constructor and two more CLayout methods are exact. Fresh ninja and frozen main.dol verification pass, with no formerly exact real function regressing against the prior clean build. No new project-wide trap was established; local emission and field-type evidence is retained in the linked findings.

2026-11-04 verified continuation: 49 additional real functions match, leaving 71 of the original 120-function tail. CPiiPersonalData now links from source with all 44 functions exact. Further gains include CDlgMsgWin, CBattleRoyalBox, three CGameSession functions, two CPii helpers, two CDlgPiiSelector helpers and PiiPropData sort. Fresh ninja and frozen main.dol verification pass. Matched code is 2,973,680/3,082,232 bytes (96.478134%); linked code is 2,205,924 bytes (71.569046%), across 828 Matching units. The shared sort prefix-increment correction introduced no regression in any previously exact function. Function-perfect units awaiting source-link acceptance now include CFileData, SaveData, main, ScriptWorldObj, CDlgMsgWin, CGameSession and CBattleRoyalBox.

2026-11-04 verified continuation: CTitleSession now matches all 17 functions and links from source. This brings the session to 38 additional real matches, leaving 82 from the initial 120-function tail. Fresh ninja and the frozen main.dol verifier pass. Matched code is 2,971,244/3,082,232 bytes (96.3991%); linked code is 2,202,640 bytes (71.4625%), across 841 Matching units of 1,386 total units. CFileData now has exact function addresses and exception tables in its source-link trial, but data order remains unresolved and its selector remains NonMatching. Its scoped header changes pass the broad fallback build.

2026-11-04 verified continuation: 37 additional real functions now match, leaving 83 of the original 120-function tail. CBattleRoyalDisplay and FA pdm_mbr now link from source with a fresh full frozen-SHA PASS. ScriptWorldObj now matches 240/240 functions but its source link still needs emission-order work. Current code: 2,968,528/3,082,232 matched bytes (96.31098%), 2,195,700 linked bytes (71.237335%), and 826 Matching units. Original inputs and expected hashes remain unchanged.

2026-11-04 verified continuation: 33 additional real functions match versus the 120-function starting tail, leaving 87. CEffect (58/58), ScriptWorld (37/37), and the IOStream getter in MemorySoundArchive (21/21) now link from source. Ninja exits 0 and frozen-manifest main.dol verification passes. Matched code is 2,968,072/3,082,232 bytes (96.29619%); linked code is 2,192,572 bytes (71.13586%), across 824 Matching units. New function gains include BWorld three, CModel one, CRemoteMemory one, ScriptWorld one, and IOStream one. CFileData, SaveData, main, and CBattleRoyalDisplay remain function-perfect but await source-link acceptance. Original inputs and expected hashes are unchanged.

2026-11-04 verified completion checkpoint: 26 new matching functions; 94 real functions remain below 100%. CUIList, CPpuEngine, and CDividedGround are now linked from source. Full ninja and frozen-manifest verification pass. Matched code: 2,964,684/3,082,232 bytes (96.18627%); linked code: 2,170,864 bytes (70.43156%), 822 Matching units. CFileData 148/148, SaveData 100/100, and main 145/145 match per function but still need source-link ordering/data work. The original binaries and expected hashes are unchanged. New reusable lessons: branch destinations can expose source errors hidden by high fuzzy scores, and packed species data can be misclassified as an EH pointer; both are promoted to AGENTS.md with [F-61-2](re/findings/61-cdividedground-second-block.md#f-61-2-long-tail-emission-order) evidence.

2026-11-04 second verified checkpoint: 14 additional real functions match, leaving 106 below 100%. CUIList is now linked from source with all 31 functions matching; ninja and exact frozen-manifest verification pass. Matched code is 2,949,392/3,082,232 bytes (95.69014%), linked code is 2,125,888 bytes (68.97236%), and 820 units are Matching. CPpuEngine now has 55/55 matching functions but its source link is still pending. Further gains: CFileData one, CLayout two, SaveData five, NPpuVM four. See the active [handoff](HANDOFF.md#main-branch-workflow).

2026-11-04 completion work is active on `main`, with parallel agents compiling isolated experiments and one coordinated full build. The first verified checkpoint gains CFileData `UpdateResourceFiles` (all 148 unit functions now match) and CLayout array `MakeArchiveList`. Ninja exited 0 and the frozen `main.dol` verifier passed. Report: 2,935,828 of 3,082,232 code bytes matched (95.25006%); 2,116,344 linked from source (68.66271%); 118 real functions remain below 100%. CFileData and CLayout remain NonMatching. The experimental CFileData source link failed on emission/data ordering; see [evidence and targeted fake-match lesson](re/findings/310-final-cfiledata.md). No new general compiler rule is claimed. The owner requested main-branch work by default; that instruction is recorded in AGENTS.md and HANDOFF.md.

2026-10-22 single-PC sprint, ended at checkpoint `7a7d20cea`: 95.15% of code matched (13,342 of 13,930 entries), 68.66% linked from 819 Matching units, `main.dol` verified. Units brought to Matching include CSaveManager, COyaInfo, l10n_files, CMsgBalloon, CDlgPiiList, MapPiiTable, ScriptPii, ScriptGame, ScriptCamera, CUIPiiListItem, CUIMenuSoundCallback, CPiiTex, resetmgr, CHPBar, CMultiRanking, two ef draw strategies, snd_SeqSound and puh_msc_blk. The compiler's emission order and register allocator were recovered from `mwcceppc.exe` ([sprint-ida](re/findings/sprint-ida.md), [sprint-ra](re/findings/sprint-ra.md)), and fake matches were allowed by D-0016. 120 real functions remain; see [handoff](HANDOFF.md#next-actions). New traps are promoted to `AGENTS.md`.

## Earlier session: take-issue skill for Codex

Exposed the existing `take-issue` Claude skill to Codex through a repository-local `.agents/skills/take-issue/SKILL.md` entry point that loads the shared procedure. See [validation](VALIDATION.md#codex-skill-port-validation).

## 2026-10-22: codegen rules promoted

Latest checkpoint: 90.67% of code matched, 13,064 of 13,953 functions, game 84.90% matched, 59.69% linked. The sprint compiler-behaviour rules (inline budget, register allocation, stack-slot order, emission order, const-reference load caching) are consolidated in [codegen rules](re/CODEGEN_RULES.md). Remaining work is long-tail near-misses and emission-order flips. No new trap beyond those promoted to `AGENTS.md`.

## Earlier session: per-block issues

Replaced the 36 placeholder issues with 507 per-block decomp issues that together list every function in the executable:
- Dumped all functions with their string and call references from a copy of the IDA database. Four parallel surveys then mapped `.text` 0x80052500–0x80292000 into source files and libraries (F-0012).
- The game address range turned out to contain HBM, RevoEX, NCD, CNT, TMC JPEG, FA, MSL printf, and NW4R ef, g3d and snd. Library and game regions were partitioned into blocks of 6–77 functions, mean 26.6, cut at file starts where the survey evidenced them.
- Each issue body lists every function it owns, its classes or reference files, compiler, and done criteria. Superseded issues were closed with links to their replacements. Research issues #25, #29, #35 and #36 remain. Issue #1 was then completed: CI builds and verifies every push and pull request (see [continuous integration](SETUP.md#continuous-integration)).
- Issue #16, the example run, was released unfinished.

New trap: library code inside the game address range. Validation is in [validation](VALIDATION.md#per-block-issue-set-validation).

## Earlier session: take-issue skill

Added the `take-issue` skill (`.claude/skills/take-issue/SKILL.md`) and `tools/collab/claim_issue.py`, which let any contributor tell an LLM to claim an unassigned issue and work it in an isolated worktree. Read-only and rejection paths are validated; see [validation](VALIDATION.md#issue-claim-tool-validation). No reverse engineering findings.

## Earlier session: parallel work setup

Set up parallel work under D-0011:
- Wrote [collaboration](COLLABORATION.md), which covers claims, the unit workflow, shared-file rules, the definition of done, and per-agent worktrees.
- Split the reconstruction ledger into per-area files under `docs/ledger/`, and added a per-issue findings directory and GitHub issue and pull request templates.
- Created 36 GitHub issues with area and kind labels, indexed in [workstreams](WORKSTREAMS.md). Library issues carry anchor ranges and matched reference files from F-0007.
- Validated the worktree procedure end to end.

No new reverse engineering findings. New trap: junction-aware worktree removal.

## Earlier session: Phases 1 to 3

Completed roadmap Phases 1 to 3 and started Phase 4.

- Acquired and pinned dtk 1.8.4, objdiff-cli 3.8.2, binutils 2.42-2, the CodeWarrior compiler bundle `20251118`, sjiswrap 1.2.2, and ninja 1.13.0. This resolved the earlier dtk provenance question: the user-supplied binary is the official x86 asset (F-0006).
- Froze the `main.dol` acceptance manifest and recorded both executables' sections. A delegated pass identified the boot content as a NAND loader (F-0009).
- Measured `.comment` version 15 (F-0008).
- A second delegated pass identified per-library compilers by masked code search across nine compilers (F-0007). I re-ran its two headline comparisons.
- Vendored dtk-template as comment-free files in `tools/build/` (D-0007 accepted) and reorganized `tools/` by category (D-0009).
- Wrote `configure.py`, added the Wii exception runtime split, and reproduced `main.dol` exactly from two clean builds (F-0010).
- Reconstructed and linked `Runtime/__init_cpp_exceptions.cpp`, and documented the objdiff counting method.
- Rendered progress images from the real report.
- Added the source fidelity standard and the source-language decision (D-0010).

New traps are recorded in `AGENTS.md`.

## Earlier session: reference extraction

Pulled `40483bd` (FEAT: Add progress), preserved local extraction changes, and reconciled overlapping documentation with its roadmap and progress tooling. Completed recursive reference extraction: 156 U8 archives, 134 decompressions, 3,420 inventoried files. A fresh second run produced byte-identical outputs for every file. Original WADs and prior extracted contents remain unchanged. See [extraction](re/EXTRACTION.md) and F-0005 for exact commands, tool pins, and validation limits. Native asset formats remain preserved, not converted to editable exports. New traps: 0x40 assets require LZH8 rather than dtk NLZSS, and backslash-bearing U8 names require parent directories to be created before Windows extraction. No source reconstruction or game match is claimed.

Roadmap Phase 1 remains active: executable acceptance manifests, full section/coverage review, and input provenance still need resolution. Code progress remains unmeasured; no fabricated objdiff report was supplied.

## Earlier session: progress images

Added `tools/progress/progress.py` and `config/progress.json` and embedded the roadmap and code progress images in the README; see [progress images](#progress-images) and [D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository). The code image states that progress is not measured because no objdiff report or denominator exists. Validation is recorded in [validation](VALIDATION.md#progress-image-validation). No reverse engineering findings were produced. New trap recorded in `AGENTS.md`: progress images are committed outputs that go stale unless regenerated.

## Earlier session: Petari study

Studied the Petari (Super Mario Galaxy) decompilation at commit `e0ab141` and recorded its tooling, repository mechanics, and CodeWarrior matching knowledge in [prior art](re/PRIOR_ART.md), with each practice mapped onto this project's rules. Wrote the phased [roadmap](ROADMAP.md) and proposed [D-0007](DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) to adopt the dtk-template ecosystem once the executable is confirmed. Recorded the first reverse engineering finding, [F-0001](re/FINDINGS.md#f-0001-wad-container-metadata): unencrypted WAD header, ticket, and TMD fields of the local WAD, including title ID `WPSE`, title version 1, seven contents, and boot index 6. No content was decrypted or extracted, no tool was acquired, no manifest was fingerprinted, and no code changed. New traps recorded in `AGENTS.md`: vendored dtk-template tooling conflicts with the no-comments rule, and Petari's source-comment conventions must move to documentation.

## Earlier session: documentation rules

Updated `AGENTS.md` to require persistent traps and findings with evidence links, and seeded it with established initialization lessons. Clarified that end-to-end validation is preferred and end-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited. No new reverse engineering findings or newly observed tooling traps emerged. This session changed documentation only; no game or tooling behavior changed.

## Initialization evidence

Added project architecture, living documentation, reverse engineering records, and exact artifact verification. No new reverse engineering findings were produced. No game match was attempted because the original and reconstructed artifacts are unavailable. See `HANDOFF.md` for the next action.

CLI integration checks passed for matching samples and rejection of mismatches, missing output, changed references, overlapping roots, and repeat fingerprinting. Syntax, configuration, documentation links, original ignore preservation, and comment-free Python were checked. See [validation](VALIDATION.md) for evidence and limits. No unit tests were added.

## 2026-11-04: BUI, CDlgInfo and GameUtil acceptance

The native build exits0 and exact frozen main.dol verification passes with873 Matching units. The32-function tail includes a new exact CUIMenu OnInput; CUIMenu remains NonMatching pending emission order. CDlgInfo now links all75 functions after correcting five inferred shared-pointer destructor identities from exception cleanup consumers, reproducing metadata order and replacing an incorrect1.0f bound extension with the original1024.0f constant. GameUtil links all13 functions after keeping its static effect presets as constructor-free aggregates; normal constructor-bearing parameter types had emitted an unsupported0x308 startup initializer. Linked data is1,655,985/1,788,827 bytes (92.57379%). The baseline address comparison found no previously exact function regressions. Forcing a rebuild of the stale CModel source object validated its three additional exact functions; all158 are exact, with source-link ordering pending.

## 2026-11-04: Window and recovered data checkpoint

ShowTypeFilterMenu now compares exactly through two scoped enum shift values. NW4HBM Window DrawFrame is also exact and the whole Window unit passes source-link verification. The accepted build has30 real unmatched functions, 3,029,036 matched code bytes (98.27411%), and2,684,408 source-linked code bytes (87.09299%). Native ninja exited0 and frozen main.dol verification passed; no previously exact function regressed against the clean baseline by address.

Three typed BTE configuration units, five small global-data units, bd_addr_null and ScriptWorld's already generated CWaza metadata now link from source. Data linkage is1,656,651/1,788,773 bytes (92.61382%), with873 Matching units among1,385. Data denominators changed as inferred object sizes and padding boundaries were refined; original hashes and artifacts remain unchanged. The [data findings](re/findings/small-global-data.md) record the boundaries and the [BTE record](re/findings/bta-configuration-data.md) records the typed tables.

A separate baseline experiment proves native final-link section ordering while preserving the frozen SHA. It is still an experimental integration method; no additional function-perfect unit is accepted from it yet. See [native section linking](re/findings/native-section-link.md).
