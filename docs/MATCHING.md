# Exact matching and completion

## Reference contract

Each acceptance artifact has a byte size and SHA-256 captured from an identified original. The verifier validates the reference and reconstruction against those values and directly compares both files. Changed originals, missing artifacts, unsealed manifests, and mismatched builds fail.

The verifier rejects overlapping reference and output roots, links in artifact paths, and reference/output files that are the same underlying file. Do not run builds or extraction while verification is reading their files. A manifest change during verification also fails.

Never strip headers, normalize padding, ignore sections, or patch differences during acceptance. Narrower comparisons are diagnostics and must be labeled partial. See [the reverse engineering workflow](re/WORKFLOW.md) for mismatch investigation.

## Scope and progress

The manifest is an explicit acceptance list. Extra files in either root are not checked. Review it against [the content inventory](re/INVENTORY.md). Matching a single executable does not establish a full game or package match.

| Measure | Required evidence |
| --- | --- |
| Reconstructed functions or bytes | Actual source implementations and an established counting denominator |
| Matching functions or bytes | Local binary identity and a recorded comparison method |
| Remaining fallbacks | Explicit list of assembly, original objects, blobs, and unresolved implementations |
| Artifact match | Passing exact verification for a named frozen manifest |
| Full release match | Complete inventory coverage, including packaging when claimed |

Do not publish percentages before the denominator is known. Runtime behavior and compilation alone cannot prove exact identity. Hash equality cannot prove source reconstruction: copying originals into the output directory can satisfy byte comparison and must never count as decompilation progress.

## Completion gates

1. Release identity and provenance are documented and reference fingerprints frozen.
2. All executable modules and data are inventoried, with section layouts and address mappings where applicable.
3. Tool versions, binary digests, flags, linker order, and environment are pinned with reproducible acquisition and build commands.
4. All executable code in the claimed scope is reconstructed from source. Assembly or binary fallbacks remain explicit blockers to a full source-decompilation claim.
5. Asset reconstruction and local original-data dependencies are documented. No undisclosed original executable dependency remains.
6. A clean build passes exact verification for every required artifact. Repeat a clean build when establishing reproducibility and after toolchain or determinism changes.
7. A whole-game package claim requires exact final package bytes too. Unresolved metadata, signing, encryption, and container details remain blockers; they do not justify weakening the claim.
8. Progress, findings, handoff, and `AGENTS.md` agree with the evidence.

No unit tests are used. Prefer end-to-end tests that execute real tooling or the complete build-to-verification workflow, checking exit status and produced artifacts. Include relevant rejection paths. Tool checks and documented runtime observations supplement these workflows; they do not replace exact byte comparisons.
