# F-491-1: NAND open/close, core, secret, check and logging units

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. All five units start within #491 and contain its complete 38-function, 8,052-byte scope. The preceding `nand.c` split ends at 802E5BB4, as established by [F-490-5](490-nand.md); the following `scsystem.c` begins at 802E7C50. No neighboring split ownership is taken.

## Source provenance

Open/close, core, secret and logging use [Petari NAND sources](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/nand), pinned at `e0ab141914921726a595573dc48d3f13d0922efe`. Directory-changing functions, the release report stub and quota-check routines use [cmwp NAND sources](https://github.com/yomcube/cmwp-decomp/tree/cbbeca0c95a3b0ecbbea6377c08d2b87f1632681/src/RVL_SDK/nand), pinned at `cbbeca0c95a3b0ecbbea6377c08d2b87f1632681`. Both repositories are CC0; the consulted NAND files have no additional file-level notices. cmwp's original enum/check/limit declarations corroborate the local constants. Other references consulted for comparison were CC0 ogws `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` and CC0 pbr-decomp `2f00145d6b48689c2d182c409828b13cdf1e55f8`; their NAND code is not copied into the final reconstruction.

The missing `NANDBanner` declaration extends the existing imported `include/revolution/nand.h` using [Petari's pinned header](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/libs/RVL_SDK/include/revolution/nand.h). Its actual layout is signature 0, flag 4, icon speed 8, 22 reserved bytes at 0xA, two 32-element UTF-16 comment arrays at 0x20, a 24,576-byte banner texture at 0xA0, then eight 4,608-byte icon textures at 0x60A0. Total size is 0xF0A0. The reference's field-offset comments for the texture arrays disagree with its actual declaration; sizes and target accesses, not those comments, establish the offsets. No existing shared type is changed.

## Unit boundaries

Every address below is an exclusive-end range derived from the compiled source sections and target relocation operands.

| Unit | Section | Start | End |
| --- | --- | --- | --- |
| `NANDOpenClose.c` | `.text` | `802E5BC0` | `802E60CC` |
| `NANDCore.c` | `.text` | `802E60D0` | `802E72AC` |
| `NANDCore.c` | `.sdata` | `804B8BF8` | `804B8C20` |
| `NANDCore.c` | `.data` | `803DC3E0` | `803DC52F` |
| `NANDCore.c` | `.sbss` | `804B99A8` | `804B99AC` |
| `NANDCore.c` | `.rodata` | `803716C8` | `80371810` |
| `NANDCore.c` | `.bss` | `8046ECE0` | `8046ED20` |
| `NANDSecret.c` | `.text` | `802E72B0` | `802E7368` |
| `NANDCheck.c` | `.text` | `802E7370` | `802E7654` |
| `NANDCheck.c` | `.data` | `803DC530` | `803DC617` |
| `NANDCheck.c` | `.sdata` | `804B8C20` | `804B8C30` |
| `NANDLogging.c` | `.text` | `802E7660` | `802E7C44` |
| `NANDLogging.c` | `.sdata` | `804B8C30` | `804B8C34` |
| `NANDLogging.c` | `.bss` | `8046ED40` | `8046F040` |
| `NANDLogging.c` | `.data` | `803DC618` | `803DC65B` |
| `NANDLogging.c` | `.sbss` | `804B99B0` | `804B99B8` |

The target SDK banner is read directly from the DOL: `<< RVL_SDK - NAND` followed by a tab and `release build: Feb 27 2009 10:05:16 (0x4302_145) >>`. Wii/1.0 with existing `cflags_sdk`, file IPA and default function alignment reproduces all 38 non-relocation instruction streams and source-object function offsets. Initialized non-relocation data also agrees byte-for-byte. Complete object and linked-DOL acceptance is recorded below after validation.

## F-491-2: Function set and release stub

Unused safe-open/close APIs and their callbacks are absent; the open/close unit contains nine retained functions. The core retains synchronous `NANDChangeDir`, its shared helper and callback, and `NANDGetCurrentDir`. The other public directory-changing wrappers in cmwp are absent and omitted. `nandGetParentDirectory` is also absent. `NANDInitBanner` at 802E71D0 belongs to NANDCore, not NANDSecret; it clears 0xF0A0 bytes, writes the WIBN signature and flags, and fills two 32-character UTF-16 comments. NANDSecret contains only `NANDSecretGetUsage` at 802E72B0.

The two routines at 802E7370 and 802E7420 are `NANDCheckAsync` and `nandUserAreaCallback` in a distinct NANDCheck unit omitted by the survey's reference list. The user-directory list has ten paths and a null sentinel. The check compares home limits of 1,024 blocks and 33 inodes, and user limits of 17,408 blocks and 4,000 inodes. The callback also retains its available-area state. Local state names `NAND_STATE_CHECK_USAGE` and `NAND_STATE_AVAILABLE_AREA` describe proven behavior; their original spellings are unknown.

`nandReportErrorCode` is a release stub retaining its error-code switch. Replacing it with an entirely empty helper produces a 0x150-byte converter instead of the observed 0x168 bytes, omitting two groups of range comparisons. The cmwp release switch inlines to precisely those comparisons without introducing another emitted function. The NAND error conversion and logging strings and error-map values remain source data.

Logging retains three 256-byte buffers aligned to 64 bytes, a reserved descriptor, stage and callback. The callback drives the original async seek/read/write/close sequence and formats the timestamp/title/message log line. No executable-byte payloads, unit tests or frozen-manifest changes are used.

## Traps

- An empty release helper can still retain switch comparisons in this compiler. Preserve the evidenced release switch in `nandReportErrorCode`; a bare return changes the caller. This observation is limited to the inspected NAND revision.
- Data-only references can hide another owned section: NANDCheck's `/meta` and `/ticket` literals are reached through the directory pointer table, not direct text relocations. Follow ADDR32 data relocations before accepting the complete section inventory.

## Source-linked acceptance

The baseline link, split-only link and source-linked link each exited 0 and passed the frozen SHA1 check. Split-only and source-linked `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exited 0. `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, and `.venv/Scripts/ninja` exited 0; `--no-progress` skips only the aggregate progress report. Five `objdiff-cli diff` comparisons of the original/source objects exited 0 and reported 100% for every code and data section.

| Unit | Matching functions | Function bytes |
| --- | --- | --- |
| `NANDOpenClose.c` | 9/9 | 1,248 |
| `NANDCore.c` | 23/23 | 4,396 |
| `NANDSecret.c` | 1/1 | 184 |
| `NANDCheck.c` | 2/2 | 736 |
| `NANDLogging.c` | 3/3 | 1,488 |

All 38 issue functions and 8,052 bytes are linked from source, with no fallback within these units. The final rebased commit and fresh-build validation are recorded in the pull request. The frozen DOL SHA256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Verification covers main game DOL only, excluding boot content 6, other data contents and WAD packaging.
