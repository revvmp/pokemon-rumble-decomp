# Initialization validation

Validation ran with Python 3.14.0 on Windows. These results concern repository tooling only. They provide no evidence of an original game hash or reconstructed game match.

## Completed checks

- Parsed `tools/verify/verify.py` and checked that it contains neither comment tokens nor docstrings.
- Parsed both JSON configuration files.
- Checked relative Markdown links resolve to existing files.
- Confirmed `.gitignore` begins with all original content, including its headings and original game filename exclusion.
- Exercised CLI help and confirmed the unconfigured target template fails with exit code `1`.
- Ran the actual fingerprint and verify CLI commands against temporary synthetic artifacts, then removed the temporary directory.
- Confirmed initial fingerprinting succeeds and a second fingerprint attempt refuses to overwrite the recorded values.
- Confirmed matching artifacts succeed and produce a passing JSON report.
- Changed the last byte of a 2,097,408-byte sample and confirmed verification fails with the exact differing offset, beyond the first comparison chunk.
- Confirmed missing output fails.
- Changed both original and output to identical new bytes and confirmed the frozen reference manifest still rejects them.
- Confirmed overlapping reference and build roots fail.
- Checked Git diff whitespace.

The synthetic input was `bytes(range(256))` repeated 8,193 times, stored as separate original and rebuilt files under temporary roots. This was a one-time command-line integration check; no unit tests or test infrastructure were added.

## Remaining validation

The documented Python 3.10 minimum has not been exercised on a separate interpreter. No real game extraction, compilation, linking, runtime check, executable match, or package match has run. Establish those results from the identified original and real reconstruction before changing their progress status.

## Progress image validation

Ran with Python 3.14.2 on Windows. These checks concern repository tooling only.

- Rendered the committed images with the real CLI from `config/progress.json` without a report: exit `0`. Rendered again to a separate directory and confirmed byte-identical output.
- Rendered all three images from a real objdiff `report.json` produced by Petari's public CI (artifact `RMGK01_report`, created 2026-10-15) to confirm the schema, including 64-bit counts serialized as strings and 2,205 units. Output stayed in temporary storage.
- Rendered a temporary copy of that report with match percentages deterministically varied across all seven bins, then viewed screenshots in headless Microsoft Edge in light and dark color schemes to check shading, legends, labels, and overflow.
- Confirmed rejection with exit `1` and no output for two active phases, out-of-order phase states, a report with zero total code, a file lacking report structure, and a missing report. Confirmed missing required arguments exit `2`.
- Confirmed `tools/progress/progress.py` contains no comment tokens or docstrings, and `config/progress.json` parses.

No external or modified report is committed, and no game progress has been measured.

## Recursive extraction and incoming-commit integration

The real `tools/reference/extract_reference.py` workflow and a fresh repeat both exited 0. All 3,420 file outputs compared byte-for-byte; F-0005 and [the extraction guide](re/EXTRACTION.md) record content, archive, wrapper, and native-header checks. Existing-output rejection exited 1 before writes. Extractor syntax and the absence of comments/docstrings were checked without unit tests.

Pulled origin/main commit `40483bd` with `git pull --ff-only origin main` after saving tracked local edits in a named stash. Restored the edits, reconciled five overlapping documentation files, retained the incoming roadmap/progress/prior-art features, and left local changes unstaged. The safety stash named `local extraction work before 40483bd` remains available. No unmerged paths or conflict markers remain; `git diff --check` passed. No commit or push was performed.

Ran `python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images` successfully. The resulting images agree with the incoming images after Windows newline handling: Phase 1 remains active and code progress remains unmeasured. No objdiff report was provided or fabricated.

## Build pipeline validation

Ran on Windows with Python 3.14.2, dtk 1.8.4 x86, objdiff-cli 3.8.2, ninja 1.13.0, and CodeWarrior Wii/1.1 linker. Evidence is in [F-0010](re/FINDINGS.md#f-0010-split-build-reproduces-maindol).

- `python configure.py` exit 0. `ninja` with the exception runtime split exit 0, including dtk's `build.sha1` check.
- `tools/verify/verify.py fingerprint` sealed `config/targets/wpse-v1.json` (exit 0), and its digest equals F-0003. `verify` passed (exit 0) with all units split, and again with `Runtime/__init_cpp_exceptions.cpp` linked from source.
- Rejection paths observed during real work: before the runtime split, with 16-byte function alignment, and with dead-stripped table entries, both dtk's check and our verifier failed (exit 1) and reported the first differing offset.
- Two clean builds, each after deleting `build/wpse-v1`, `build.ninja`, and ninja state, produced identical `main.dol`, `main.elf`, and `report.json` SHA-256 values. Both passed verification.
- `tools/vendor/vendor_strip.py` vendored seven dtk-template files. Each output was checked to have the same syntax tree as upstream with docstrings removed, and every Python file under `tools/` contains no comment tokens or docstrings.
- Progress images rendered from the real `build/wpse-v1/report.json` (exit 0) and were inspected in a headless Edge screenshot. `--treemap` without `--report` exits 1 before writing anything.
