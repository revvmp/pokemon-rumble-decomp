# Initialization validation

Validation ran with Python 3.14.0 on Windows. These results concern repository tooling only. They provide no evidence of an original game hash or reconstructed game match.

## Completed checks

- Parsed `tools/verify.py` and checked that it contains neither comment tokens nor docstrings.
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
