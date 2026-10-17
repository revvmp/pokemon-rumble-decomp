# Workstreams

Every workstream is a GitHub issue. Claim work with `tools/collab/claim_issue.py` or by telling Claude Code to take an issue (the `take-issue` skill), as described in [collaboration](COLLABORATION.md). Maintainers keep this table current; contributors do not edit it.

Library address regions and compilers come from [F-0007](re/FINDINGS.md#f-0007-per-library-compiler-identification). Each issue body has the anchor range, the matched reference files, and its done criteria.

## Infrastructure

| Issue | Title | Kind |
| --- | --- | --- |
| #1 | Infra: build and verify in CI on every push and pull request | infra |
| #35 | Toolchain: confirm the original linker version | research |

## RVL_SDK

| Issue | Title | Kind |
| --- | --- | --- |
| #2 | RVL_SDK: OS | decomp |
| #3 | RVL_SDK: base, db, EXI, SI | decomp, good first issue |
| #4 | RVL_SDK: VI | decomp |
| #5 | RVL_SDK: MTX | decomp, good first issue |
| #6 | RVL_SDK: GX | decomp |
| #7 | RVL_SDK: DVD | decomp |
| #8 | RVL_SDK: AI, AX, DSP | decomp |
| #9 | RVL_SDK: MEM | decomp, good first issue |
| #10 | RVL_SDK: NAND | decomp |
| #11 | RVL_SDK: SC | decomp |
| #12 | RVL_SDK: ARC, TPL, GD | decomp, good first issue |
| #13 | RVL_SDK: ESP, IPC, FS | decomp |
| #14 | RVL_SDK: PAD and KPAD | decomp |
| #15 | RVL_SDK: WPAD and WUD | decomp |
| #16 | RVL_SDK: EUART | decomp, good first issue |
| #17 | RVL_SDK: scope libraries without reference source | research |
| #36 | Boot loader: decide reconstruction scope for content 6 | research |

## MSL_C, runtime, MetroTRK

| Issue | Title | Kind |
| --- | --- | --- |
| #18 | MSL_C | decomp |
| #19 | Runtime: remaining runtime units | decomp |
| #20 | MetroTRK: scope and reconstruct the debugger kernel | research |

## NW4R

| Issue | Title | Kind |
| --- | --- | --- |
| #21 | NW4R: ut | decomp |
| #22 | NW4R: lyt | decomp |
| #23 | NW4R: math and db | decomp, good first issue |
| #24 | NW4R: locate and scope g3d, snd, and ef | research |
| #25 | NW4R: resolve Wii/1.0 versus Wii/1.1 | research |

## Home Button Menu

| Issue | Title | Kind |
| --- | --- | --- |
| #26 | HBM: Home Button Menu and its layout library copy | decomp |

## Other middleware

| Issue | Title | Kind |
| --- | --- | --- |
| #27 | Middleware: scope RevoEX, NCD, TMC JPEG, and the second printf | research |

## Game code

| Issue | Title | Kind |
| --- | --- | --- |
| #28 | Game: survey the game code region and propose translation units | research |
| #29 | Game: recover names from debug and script strings | research |
| #30 | Game: main.cpp | decomp |
| #31 | Game: CPauseMenu.cpp | decomp |
| #32 | Game: CLayout.cpp | decomp |
| #33 | Game: CFileData.cpp | decomp |
| #34 | Game: CPath.cpp | decomp |

## Suggested order

1. Start with `good first issue` library work: #3, #5, #9, #12, #16, #23. These have reference source and a known compiler.
2. Run the larger library issues in parallel with file-level claims: #2, #6, #7, #10, #18, #19, #21, #22.
3. Research issues #17, #24, #27, and #28 open the next decomp issues; #25 and #35 settle open toolchain questions.
4. #1 (CI) should land early so every pull request is verified automatically.
5. Game issues #29 to #34 can start at once, but #28's boundary map makes them faster.
