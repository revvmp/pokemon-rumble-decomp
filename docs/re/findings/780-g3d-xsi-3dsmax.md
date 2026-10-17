# F-780-2: XSI and 3ds Max DCC matrix units

## Scope and reference

Issue #780 owns 31 functions / 5,744 instruction bytes in two complete retained source units. The pinned reference is CC0 Skyward Sword `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `src/nw4r/g3d/dcc/g3d_xsi.cpp` (SHA-256 `2f441b9280053a09350b2073f54cce77cf7f876c520aa1ce4142a330834bc6db`) and `g3d_3dsmax.cpp` (`a3347cd4fc70ce0e08052bc6ec3ff4251756e1b8623a9de624243c05eeb45bb1`). Neither source contains a file-level exception to the repository's CC0 license.

Both references compile against the existing TexSrt, ChrAnmResult, matrix, and trigonometric headers plus the five native-evidenced WorldMtxAttr helpers from [F-780-1](780-g3d-dcc-headers.md). Every retained function has its native size and non-relocation instruction stream under the established Wii/1.1 `cflags_nw4r`, with default 16-byte function alignment. Complete source objects now compare at 100% in `.text`, `.rodata`, and `.sdata2`. Both are linked from source with all 31 native final addresses and sizes, all object offsets, the complete issue inventory, and direct frozen-DOL byte equality verified. Merged PR830 supplies the complete canonical WorldMtxAttr interface, superseding the redundant closed PR826; this source delivery changes no shared header.

## Boundaries

| Unit | Functions / instructions | Text | Read-only dispatch tables | Small read-only constants |
| --- | --- | --- | --- | --- |
| `dcc/g3d_xsi.cpp` | 16 / 3,008 bytes | `0x80266600`–`0x80267214` | `0x8036C8F0`–`0x8036C928` | `0x804BAD00`–`0x804BAD0C` |
| `dcc/g3d_3dsmax.cpp` | 15 / 2,736 bytes | `0x80267220`–`0x80267D3C` | `0x8036C928`–`0x8036C960` | `0x804BAD10`–`0x804BAD24` |

The first file ends after CalcWorldMtx_Xsi and the second after CalcTexMtx_3dsmax. The preceding Maya and following ScnObj files retain their owners. Each dispatch table contains seven native texture-function pointers in the reference order. The actual source objects contain 12 and 20 constant bytes respectively with eight-byte section alignment. Four zero bytes after each extent are inter-unit alignment, not an extra float member of the final literal. The original terminal literal metadata is narrowed from eight bytes to the four bytes actually loaded; expected hashes remain untouched.

## Traps

The F-0013 spans are first/last mapped functions, not a proof that the reference's first/last functions survived. Here complete reference compilation reproduces all 16 and 15 target functions, including the dispatch routines and the XSI world-matrix routine, without claiming neighboring files.

The compiler's small-constant sections are eight-byte aligned but have lengths 12 and 20. Ending a unit at the next aligned start would include another four bytes of padding that the source object does not contain. Inspect actual section sizes and preserve the following unit alignment rather than adding invented storage.

Repeated rebases of this branch's historical NonMatching-to-Matching ledger edits resurrected stale rows through the union merge even when an old cleanup commit was skipped as already applied. Reviewing the final table exposed both statuses for each unit. Consolidating the reviewed issue-only final snapshot into one commit and retaining exactly its two Matching rows avoided replaying the stale intermediate states; all main rows were preserved, and the exact committed snapshot was verified again. Always inspect union-merged ledger content rather than treating a conflict-free rebase as sufficient.

## Retained inventory

| Address | Function | Unit | Instruction bytes |
| --- | --- | --- | --- |
| `0x80266600` | `MakeTexSrtMtx_S` | `g3d_xsi` | 56 |
| `0x80266640` | `MakeTexSrtMtx_R` | `g3d_xsi` | 136 |
| `0x802666D0` | `MakeTexSrtMtx_T` | `g3d_xsi` | 56 |
| `0x80266710` | `MakeTexSrtMtx_SR` | `g3d_xsi` | 160 |
| `0x802667B0` | `MakeTexSrtMtx_RT` | `g3d_xsi` | 200 |
| `0x80266880` | `MakeTexSrtMtx_ST` | `g3d_xsi` | 80 |
| `0x802668D0` | `MakeTexSrtMtx_SRT` | `g3d_xsi` | 200 |
| `0x802669A0` | `ProductTexSrtMtx_S` | `g3d_xsi` | 120 |
| `0x80266A20` | `ProductTexSrtMtx_R` | `g3d_xsi` | 272 |
| `0x80266B30` | `ProductTexSrtMtx_T` | `g3d_xsi` | 36 |
| `0x80266B60` | `ProductTexSrtMtx_SR` | `g3d_xsi` | 316 |
| `0x80266CA0` | `ProductTexSrtMtx_RT` | `g3d_xsi` | 300 |
| `0x80266DD0` | `ProductTexSrtMtx_ST` | `g3d_xsi` | 136 |
| `0x80266E60` | `ProductTexSrtMtx_SRT` | `g3d_xsi` | 348 |
| `0x80266FC0` | `CalcTexMtx_Xsi` | `g3d_xsi` | 156 |
| `0x80267060` | `CalcWorldMtx_Xsi` | `g3d_xsi` | 436 |
| `0x80267220` | `MakeTexSrtMtx_S` | `g3d_3dsmax` | 72 |
| `0x80267270` | `MakeTexSrtMtx_R` | `g3d_3dsmax` | 172 |
| `0x80267320` | `MakeTexSrtMtx_T` | `g3d_3dsmax` | 56 |
| `0x80267360` | `MakeTexSrtMtx_SR` | `g3d_3dsmax` | 180 |
| `0x80267420` | `MakeTexSrtMtx_RT` | `g3d_3dsmax` | 204 |
| `0x802674F0` | `MakeTexSrtMtx_ST` | `g3d_3dsmax` | 88 |
| `0x80267550` | `MakeTexSrtMtx_SRT` | `g3d_3dsmax` | 204 |
| `0x80267620` | `ProductTexSrtMtx_S` | `g3d_3dsmax` | 128 |
| `0x802676A0` | `ProductTexSrtMtx_R` | `g3d_3dsmax` | 292 |
| `0x802677D0` | `ProductTexSrtMtx_T` | `g3d_3dsmax` | 36 |
| `0x80267800` | `ProductTexSrtMtx_SR` | `g3d_3dsmax` | 324 |
| `0x80267950` | `ProductTexSrtMtx_RT` | `g3d_3dsmax` | 320 |
| `0x80267A90` | `ProductTexSrtMtx_ST` | `g3d_3dsmax` | 148 |
| `0x80267B30` | `ProductTexSrtMtx_SRT` | `g3d_3dsmax` | 356 |
| `0x80267CA0` | `CalcTexMtx_3dsmax` | `g3d_3dsmax` | 156 |

## Verification

The split baseline passed plain Ninja and the frozen verifier. After registering the units NonMatching, actual source objects reproduced every compared section at 100%. The source-selected default pipeline then passed `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, all with exit 0. The outside issue audit reconciles all 31 functions / 5,744 instruction bytes against the cached issue body, requires each source-object offset and final function address/size, inspects actual source-only linker inputs for both units, and compares the full DOL directly. It also exits 0. No original executable fallback remains in the two complete units; alignment gaps remain linker layout rather than invented source storage.
