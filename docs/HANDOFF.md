# Handoff

## Current position

Target `wpse-v1`: title ID `0001000157505345`, USA, TMD title version 1. Original root WAD unchanged; identical local copy at `orig/wpse-v1/reference.wad`. Frozen package provenance manifest: `config/targets/wpse-v1-package.json`. Do not fingerprint it again. See `re/FINDINGS.md` F-0001 for offsets, hashes, declared contents, and limitations.

Python 3.14.0 inspection and real verifier fingerprinting succeeded (exit 0); independent SHA-256 checks agreed for both copies. The user extracted contents under `orig/rumble-unidentified/extracted/`; all seven sizes and SHA-1 values match the TMD. No reconstructed source or game build exists. Authenticity and toolchain remain unresolved.

## Next actions

Follow [the roadmap](ROADMAP.md) and [prior art](re/PRIOR_ART.md). Phase 1 remains active: package provenance is frozen and recognized archive extraction is complete, but executable acceptance manifests and full executable coverage/section review are still pending. Do not infer compiler versions from Petari.

1. Use [the extraction guide](re/EXTRACTION.md), [archive index](re/ARCHIVE_INDEX.md), and [inventory](re/wpse-v1-extraction.csv). The successful reference tree is `orig/wpse-v1/extraction/`; both repeated extractions produced 3,420 identical files. Existing destinations are intentionally rejected by the extractor.
2. Keep the original package, compressed contents, wrappers, and derived native resources. Main executable: `orig/wpse-v1/main.dol` (entry 0x80004050); boot executable: `orig/wpse-v1/boot.dol` (entry 0x81330000). Trace boot-to-main control flow and inspect for embedded code before claiming full code coverage.
3. Freeze separate executable acceptance manifests after reviewing scope, and finish raw section records. Do not modify or re-fingerprint `config/targets/wpse-v1-package.json`.
4. Establish input acquisition provenance and the user-supplied dtk binary's download source; these remain unknown. Decoder source acquisition, revisions, and hashes are recorded in F-0005.
5. Analyze library/compiler evidence, then settle D-0007 open items before adopting a build scaffold. Native BRRES/RSAR, effects, BSP, and PKC resources remain intact for later format-specific work; editable asset conversion is separate from reference extraction.

The incoming `40483bd` commit was pulled with local edits preserved, and its progress tooling and prior-art notes were retained. Code coverage has no measured denominator. Regenerate progress images from real phase/report inputs; never invent an objdiff report.

## Available commands

```powershell
Get-FileHash -Algorithm SHA256 orig/wpse-v1/reference.wad
.\dtk.exe dol info orig/wpse-v1/main.dol
python tools/verify/verify.py --help
python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images
```

Recursive extraction and a fresh repeat both exited 0; every recorded output matched byte-for-byte. No game build command exists yet. Follow root `AGENTS.md` and `PROJECT_RULES.md`; all files under `orig/` are eligible for tracking by explicit user instruction; keep tools and extracted contents outside `orig/` ignored, and do not add unit tests, code comments, or docstrings.

## Git tracking of originals

The user explicitly requested that all of `orig/` be committable. Final `!/orig/` and `!/orig/**` rules override extension exclusions inside that directory. An earlier `git ls-files --others --exclude-standard -- orig/` check validated the original 13 files; recursive extraction has since added the recorded reference tree. The earlier staging dry run could not create `.git/index.lock`. During the pull integration, approved Git operations resolved conflicts and returned local edits to an unstaged state. Preserve these exceptions after general ignore patterns.
