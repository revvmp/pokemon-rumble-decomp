# Handoff

## Current position

Target `wpse-v1` (WPSE, USA, TMD title version 1). Roadmap Phase 4 is active. Frozen manifests: package `config/targets/wpse-v1-package.json` and executable `config/targets/wpse-v1.json` (`main.dol`). Do not fingerprint either again.

State at checkpoint `7a7d20cea` (2026-10-22, end of the single-PC sprint; `main.dol` verified byte-identical):

| Measure | Value |
| --- | --- |
| Code matched (functions at 100%) | 95.15%, 13,342 of 13,930 entries (467 are `pad_03_*` alignment entries, not code) |
| Code linked from source (Matching units) | 68.66%, 819 of 1,387 units |
| Real functions below 100% | 120, in about 50 units |
| Units with every function at 100% but not linked | CBattleRoyalDisplay (FormatString emission slot), CEffect (extra `__bitvec_base` destructor from the `vector<bool>` header) |
| Open GitHub issues | 64; each still lists its functions and links prior findings |
| Fake matches (D-0016) | 22 rows in [the ledger](re/FAKEMATCHES.md) |

The sprint committed from one PC straight to `main`. The owner explicitly requested main-branch work again on 2026-11-04; follow the workflow below.

## Main-branch workflow

Use `main` by default. Do not split work into branches or worktrees unless the owner explicitly requests that. Parallel agents own distinct source files and use direct compiler commands with separate scratch inputs and outputs under ignored `tools/local/session/`. One coordinator owns the shared build, configuration updates, full verification, and checkpoints. Issue claim exclusions still apply; branch/PR/merge-loop instructions are superseded for this workflow.

The initial 2026-11-04 worktrees were preserved when this instruction arrived. Their reviewed gains were transferred to main, and further experiments run from main snapshots. Do not blindly copy their files back over newer main changes.

2026-11-04 verified continuation: 90 additional real functions match, leaving 30 of the original 120-function tail. Fresh ninja and frozen main.dol verification pass. Matched code is 3,029,036/3,082,232 bytes (98.27411%); linked code is 2,684,408 bytes (87.09299%), across 873 Matching units. Original fallbacks remain explicit. Recent source-link acceptances include CFileData, CDlgMsgWin, main, CBattleRoyalBox, ScriptWorldObj, SaveData, CGameSession, CGameEnding, Message, PhotoSave, errmsg, CNoticeDisplay CMoneyLifeDisplay, BModelObj, CMiniMap, CPauseMenu and fourteen typed data units. Two new clean builds at the preceding50-function checkpoint reproduce main.dol, main.elf and report.json exactly, including the source-fragment pipeline and gains accepted at that checkpoint; repeat clean verification after later changes.

CDlgPiiSelector has all 645 functions exact and still needs ordering/data acceptance. BWorld, homebutton and TextWriterBase have since passed full source linkage. BUI, CDlgInfo and GameUtil now also pass full source linkage; CUIMenu has all61 functions exact with emission work pending. Current parallel ownership: selector source link, NPpuVM, CModel, CPii, sort templates and ParticleManager. The coordinator owns shared configuration, full builds, acceptance and checkpoints. Scratch inputs/scripts under tools/local/session are ignored; never overwrite newer main files wholesale.

## Next actions

Two clean builds at the preceding nine-function checkpoint reproduced main.dol, main.elf and report.json byte-for-byte, with native ninja and frozen-artifact verification passing each time. See [clean-build evidence](VALIDATION.md#nine-function-checkpoint-and-grouped-source-linking). The subsequent Child gain passes incremental native ninja, the original-address regression audit and frozen verification.

The current 2026-11-04 continuation has 8 real unmatched functions, 3,060,296/3,082,252 matched code bytes (99.28767%), 2,837,388 source-linked code bytes (92.05568%), and 1,711,080/1,788,746 source-linked data bytes (95.65807%). Native ninja exited zero and frozen main.dol verification passed. There are 887 Matching units of 1,389; 1,392 raw final-link inputs include the three additional CModel fragments. The original-address audit finds no regressions or missing exact functions. CModel, CDlgCollection and CMsgRes are the latest full source-link acceptances. Checkpoint `1f6e0fc5` was committed and pushed at the preceding nine-function stage.

1. Read [codegen rules](re/CODEGEN_RULES.md) first. It now includes the compiler's recovered emission-order model (from `mwcceppc.exe` in IDA; [sprint-ida](re/findings/sprint-ida.md)) and its register allocator and scheduler ([sprint-ra](re/findings/sprint-ra.md)). Diagnose with `tools/re/regscan.py` and `tools/re/ra_trace.py` before editing.
2. Fake matches are allowed after a documented honest attempt ([D-0016](DECISIONS.md#d-0016-documented-fake-matches-for-the-remaining-long-tail)); every remaining function has one in `re/findings/sprint-*.md`. Add a ledger row for each fake.
3. Function-perfect units still needing source-link acceptance: CDlgPiiSelector, CLayout and CPlayerPii. CLayout has all171 exact bodies/natural order and all117 exact exception records; ten boolean-literal relocation bytes remain. CPlayerPii has all254 exact functions and all retained data exact, with deque/helper emission pending. CDlgPiiSelector has all645 exact bodies and371 exact per-function exception records, with native placement/ownership pending. CUIMenu and CModel use supported native `text_sections`; CModel also declares `link_source_order` for its raw source fragments. No object bytes are rewritten.
4. Remaining functions: CGameResult one, CPiiListFilterSort one, PiiPropData two, NPpuVM one, ef_animcurve three. Existing private partial candidates are documented in each continuation finding. Parallel ownership: layout agent finishes CLayout; selector agent handles CDlgPiiSelector; sort agent handles both reference sorts; VM agent handles NPpuVM Load; JPEG agent handles PiiPropData; particle agent handles CPlayerPii source linkage; small-game agent handles Rotate privately; animcurve agent handles U8/F32 and owns their canonical file. Child is exact; its conversion-slot trace and retained source controls are in [the dedicated finding](re/findings/animcurve-child-20261104.md).
5. decomp-permuter only parses C. The C++ text-level permuter (`cxxperm.py`, `minimize.py`) and the patched C harness are in ignored `tools/local/perm/`; setup is in [sprint-perm](re/findings/sprint-perm.md). Its third wave (fakes allowed, 51 functions at 97% or more) never completed.
6. The ef draw strategies and CRemoteMemory match with per-function optimizer pragmas; the original was probably built with different file-level settings. Revisit when evidence appears. Ledger entries are candidates for honest rewrites.
7. After the long tail, the owner wants a native Windows and WebAssembly port (a GX graphics layer such as Aurora, audio, input, file I/O, endianness). Not started.

The earlier sprint's `tools/local/session/` and `tools/local/perm/` helpers are absent on this PC. Current isolated experiments and reusable local probe helpers are under ignored `tools/local/tail-20261104/`; do not assume old handoff scratch paths exist. The tracked native section preparer is `tools/build/order_text_sections.py`, invoked by ninja automatically.

## Available commands

## Repository skills

The current Claude skill, `take-issue`, is also exposed to Codex through `.agents/skills/take-issue/SKILL.md`. Invoke it in Codex with `$take-issue`, optionally naming an issue or area label, or ask to pick up an issue. The Codex entry point loads `.claude/skills/take-issue/SKILL.md`, which remains the single authoritative procedure. Edit that shared file for workflow changes and update both descriptions if the trigger changes. No global installation or Windows symlink is required.

Validation and its limits are recorded in [validation](VALIDATION.md#codex-skill-port-validation).

## Build and verification commands

Prerequisites: tools in `tools/local/` and ninja in `.venv`, as in [setup](SETUP.md#acquire-tools).

```powershell
python configure.py
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1 --report build/wpse-v1/verification.json
python tools/progress/progress.py render --phases config/progress.json --report build/wpse-v1/report.json --output-dir docs/images
python tools/reference/extract_reference.py --help
```

Follow root `AGENTS.md` and `PROJECT_RULES.md`. All files under `orig/` are eligible for tracking by explicit user instruction. Tool binaries stay in ignored `tools/local/`. Do not add unit tests, code comments, or docstrings.

## Git tracking of originals

The user explicitly requested that all of `orig/` be committable. Final `!/orig/` and `!/orig/**` rules override extension exclusions inside that directory. An earlier `git ls-files --others --exclude-standard -- orig/` check validated the original 13 files; recursive extraction has since added the recorded reference tree. The earlier staging dry run could not create `.git/index.lock`. During the pull integration, approved Git operations resolved conflicts and returned local edits to an unstaged state. Preserve these exceptions after general ignore patterns.
