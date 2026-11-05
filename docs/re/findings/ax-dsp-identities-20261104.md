# AX DSP command and routine identities, 2026-11-04

## Scope

This supplements [the DSP image investigation](ax-dsp-image-20261104.md). The input is the 8,192-byte `axDspSlave` image whose SHA-256 is `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. All addresses below are DSP word addresses, not PowerPC addresses or file-byte offsets. Instruction memory and data memory are distinct address spaces.

The coordinator's modern Dolphin assembler roundtrip reproduces every byte. This investigation uses its private listing, the matched PowerPC command writer in `src/RVL_SDK/ax/AXCL.c`, the parameter-block layout in `include/revolution/ax.h`, the source-selection assignments in `src/nw4r/snd/snd_AxVoice.cpp`, and task-mail handling in `src/RVL_SDK/dsp/dsp_task.c`. The private inventory `tools/local/tail-20261104/smallgame/DSP/inspect_tables.py` verifies the sealed image hash, roundtrip identity, and every code-table destination against instruction starts. It writes `tables.json`, not a production source unit.

The recovered names below describe proven roles. The command enum names are established host-side names; no original DSP spelling is claimed. Neither the private listing nor this inventory changes source-completion credit.

The later complete reconstruction is now accepted: the coordinator installed the 29-file source tree, ran the normal configure/ninja workflow successfully, and verified the full DOL against its frozen manifest. [The staging inventory and production acceptance](ax-dsp-staging-20261104.md) record the exact source closure. The earlier private investigation and its evidence boundaries are retained below.

## Command dispatch

At PC `0x006D`, `AR0` is initialized to DMEM `0x0C00`, the command buffer populated by DMA. PC `0x0071` consumes the next command word. PCs `0x0072` through `0x0077` reject values outside 0 through 14. PC `0x0079` loads table base `0x0D02`; `ILRR` at `0x007D` reads the selected word from instruction memory, and `JMPR` at `0x007F` transfers to that handler.

This establishes the 15 words at IMEM `0x0D02` through `0x0D10` as code addresses. In particular, `0x024F` at IMEM `0x0D04` is the subtraction command's handler address. The linear disassembler's unknown-opcode output at that location is not an unidentified instruction.

| ID | Host command | Handler PC | Argument words after ID |
| --- | --- | --- | --- |
| 0 | `COMMAND_SETUP` | `0x008A` | 2: studio-state address |
| 1 | `COMMAND_ADD_TO_LR` | `0x0222` | 2: surround-buffer address |
| 2 | `COMMAND_SUB_TO_LR` | `0x024F` | 2: surround-buffer address |
| 3 | `COMMAND_ADD_SUB_TO_LR` | `0x0E81` | 2: surround-buffer address |
| 4 | `COMMAND_PROCESS` | `0x027E` | 2: first parameter-block address |
| 5 | `COMMAND_MIX_AUXA` | `0x049B` | 5: volume and two addresses |
| 6 | `COMMAND_MIX_AUXB` | `0x04B4` | 5: volume and two addresses |
| 7 | `COMMAND_MIX_AUXC` | `0x04CD` | 5: volume and two addresses |
| 8 | `COMMAND_UPL_AUXA_MIX_LRSC` | `0x0DBB` | 13: volume and six addresses |
| 9 | `COMMAND_UPL_AUXB_MIX_LRSC` | `0x0DDD` | 13: volume and six addresses |
| 10 | `COMMAND_COMPRESSOR` | `0x057B` | 4: threshold, release frames, table address |
| 11 | `COMMAND_OUTPUT` | `0x0609` | 5: master volume and two addresses |
| 12 | `COMMAND_OUTPUT_DPL2` | `0x0EC4` | 5: master volume and two addresses |
| 13 | `COMMAND_WM_OUTPUT` | `0x067A` | 8: four speaker-buffer addresses |
| 14 | `COMMAND_END` | `0x0670` | 0 |

These argument counts follow the matched host writer and agree with the handler reads. For example, the remote-output handler consumes four address pairs and transfers `0x24` bytes per speaker, matching 18 signed 16-bit samples. The processing handler transfers a `0x140`-byte parameter block to DMEM `0x02D0`, matching `AXPB`. Setup transfers `0x78` bytes, matching the 20 six-byte studio records for twelve main/AUX channels and eight remote channels.

The current AXWii command enum independently agrees with all 15 identities. Its older variant has an extra parameter-block-address command and shifted subsequent IDs, so that older table must not be imported for this image. See the pinned [AXWii header](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/UCodes/AXWii.h) and [command interpreter](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/UCodes/AXWii.cpp). These are behavioral references, not matching assembly source.

## Voice dispatch tables and processing helpers

The processing handler copies `AXPB` into DMEM `0x02D0`, then reads its selectors to populate cached routine pointers. The correspondence between field layout, immediate table base, `ILRR`, and the later indirect call supplies stronger identities than the apparent instructions in a linear listing.

| IMEM table | Entries | Index and role | Selected pointer storage |
| --- | --- | --- | --- |
| `0x0D11` | 32 | Main mixing, `mixerCtrl & 31` | DMEM `0x0CD3` |
| `0x0D31` | 32 | AUX A mixing, `(mixerCtrl >> 16) & 31` | DMEM `0x0CD4` |
| `0x0D51` | 32 | AUX B mixing, `(mixerCtrl >> 21) & 31` | DMEM `0x0CD5` |
| `0x0D71` | 64 stored entries | AUX C mixing, shifted high halfword without a final five-bit mask | DMEM `0x0CD6` |
| `0x0DB1` | 4 | Successive two-bit fields of `rmtMixerCtrl` | DMEM `0x0CD7` through `0x0CDE` |
| `0x0DB5` | 3 | `AXPB.srcSelect` | DMEM `0x0CDF` |
| `0x0DB8` | 3 | `AXPB.coefSelect`, coefficient addresses rather than code | DMEM `0x0CE0` |

The four main/AUX cached pointers are called at PCs `0x0398`, `0x039B`, `0x039E`, and `0x03A1`. The remote selector reads DMEM `0x033C`, exactly `AXPB.rmtMixerCtrl`, and decodes eight two-bit fields. Its destinations are `0x0CAE` for no mixing, `0x0CB1` for constant-volume mixing, and `0x0CCE` for either index with the ramp flag set. The first helper returns a zero depop value; the latter two process 18 remote samples.

The first four tables use the same five-bit channel/volume-ramp selection. Entries 0 through 3 choose none, left, right, or both; indices with bit 2 set select the stereo ramp helper; bit 3 enables surround and bit 4 selects surround ramping. The host's `AXMixerCtrlFlags` and `__AXMixCycles` indexing agree. AUX C has a second 32-word block at `0x0D91` whose left/right handlers use the undelayed common sample pointer at DMEM `0x0CE1`, instead of the separate left/right pointers at `0x0CE2` and `0x0CE3`. This proves its alternate input selection. The host-level meaning and valid-use conditions of the extra high selector bit are not established; do not name it DPL2 solely from its location.

| Helper PC | Recovered role | Evidence |
| --- | --- | --- |
| `0x0084` | Wait for DSP DMA completion | Polls DMEM register `0xFFC9`, bit 2 |
| `0x06A9` | First-order low-pass filter | Called when `AXPB.lpf.on` at DMEM `0x032D` is set; uses its history and two coefficients |
| `0x06C1` | Biquad filter | Called when `AXPB.biquad.on` at DMEM `0x0331` is set; handles four histories and five coefficients |
| `0x0718` | Four-tap sample-rate conversion | Selector 0; four multiply/accumulate terms and coefficient-table selection |
| `0x0769` | Linear sample-rate conversion | Selector 1; two adjacent samples and fractional weighting |
| `0x07BE` | Sample-rate conversion bypass | Selector 2; copies 96 decoded samples |
| `0x07E4` | Load decoder and sample-address state | Writes format, loop/end/current address, ADPCM coefficients, and histories to hardware registers |
| `0x081D` | Empty main/AUX mixing operation | Single return, shared by zero-index mix tables |
| `0x0BCF` | One-channel constant-volume mixing | Called by single-channel table entries; multiplies samples and accumulates 96 outputs |
| `0x0BE9` | Two-channel constant-volume mixing | Called by left-plus-right entries |
| `0x0C1F` | One-channel volume-ramp mixing | Builds ramp values, updates the parameter-block volume, then mixes |
| `0x0C4F` | Two-channel volume-ramp mixing | Builds both ramps, updates both volumes, then mixes |

The three coefficient-table entries are DMEM ROM bases `0x1000`, `0x1200`, and `0x1400`. The matched `AxVoiceParamBlock::SetVoiceSrcType` assigns these selectors to 8, 12, and 16 kHz four-tap modes. They must remain symbolic coefficient-data references, not routine labels.

## Task protocol and persistent state

Initialization at `0x0010` and resume at `0x0037` send the host messages `DSP_MAIL_INIT` and `DSP_MAIL_RESUME`. The command-list receiver at `0x004C` waits for `0xBABE` in the mailbox high halfword, takes the transfer length from the low halfword, then receives the command-list address. This matches `DSP_MAIL_CLSIZE` and the next address mail in `__AXOutNewFrame`.

The output handlers send `DSP_MAIL_SYNC` (`0xDCD10004`); the END handler sends `DSP_MAIL_YIELD` (`0xDCD10002`). These names follow the matched host-side mailbox enum. The dispatcher at `0x0F99` indexes a separate four-word IMEM table at `0x0F95` using the next mail's low halfword:

| Index | Target PC | Role |
| --- | --- | --- |
| 0 | `0x0FA7` | Jump to the resume entry at `0x0037`; host enum name not recovered |
| 1 | `0x0FAA` | `CPU_MAIL_NEW_UCODE`: receive current DRAM save and next-task transfer fields, then enter the ROM loader at `0x80B5` |
| 2 | `0x0FE2` | `CPU_MAIL_RESET`: jump to ROM `0x8000` |
| 3 | `0x0FE5` | `CPU_MAIL_CONTINUE`: return to command-list reception at `0x004C` |

The 10-word transfer sequence in `__DSP_exec_task` independently identifies the loader inputs. Helpers at `0x0FE8` and `0x0FEE` wait for incoming mail while retaining the high halfword in different accumulators.

The PowerPC task declares its 64-byte saved DRAM image at DSP address `0x0CD2`. Within that span, the listing establishes the current command cursor at `0x0CD2`, cached mix/SRC pointers through `0x0CE0`, common/left/right sample pointers at `0x0CE1` through `0x0CE3`, compressor release state at `0x0CE4`, prior master volume at `0x0CE5`, and prior AUX A/B/C volumes at `0x0CE6` through `0x0CE8`. Initialization writes `0x8000` to the volume history words. The zero word at `0x0CE9` is also used as the disabled decoder source by the sample-end handler.

The identified instruction-memory data regions are `0x0D02` through `0x0DBA` and `0x0F95` through `0x0F98`: 189 words, or 378 bytes. The final twelve zero words at `0x0FF4` through `0x0FFF` remain part of the transferred image; this investigation does not remove or reclassify them as discardable padding.

## Original-language provenance

The bounded primary-source search located Nintendo's [Revolution Audio Library manual, version 1.01](https://pokeacer.xyz/wii/pdf/AX.pdf), revision `RVL-06-0031-001-C`, dated October 22, 2007. Section 2.1.2 identifies the DSP workload and states that its proprietary microcode was not available for review or modification. It establishes the distributed-program boundary but does not state which source language Nintendo used. No matching source or original AX assembly filename was recovered from that document.

The AX HLE author's [2012 account of his implementation](https://delroth.net/posts/emulating-gamecube-audio-dolphin/) explicitly describes reverse engineering the AX program and implementing its behavior. It supports using Dolphin as independent behavioral evidence, not as original source provenance. Nintendo's [sound-mixing patent US7369665B1](https://patents.google.com/patent/US7369665B1/en) describes the processing design but supplies no assembly-language or AX source-file evidence found in this search.

The coordinator then found concrete AX-family assembly input names in an archived SDK build inventory. At repository commit `81da5adc12d863744af0906e21bc175c6ff80a8d`, the [DIAG4RVL SDK build tree](https://github.com/bowsersbigbeanburrito/N/blob/81da5adc12d863744af0906e21bc175c6ff80a8d/DIAG4RVL/trunk/SDK/build_directory_tree.txt) places eight assembly files under `build/libraries/ax/dsp/src/`: `core.asm`, `dpl2.asm`, `isr.asm`, `jump.asm`, `mixinit.asm`, `src.asm`, `task.asm`, and `vector.asm`. The same subtree contains a DSP makefile and eleven includes: `au32.inc`, `cmd_list.inc`, `dram32.inc`, `drom.inc`, `flags.inc`, `irom.inc`, `mbox.inc`, `paramblk.inc`, `regs.inc`, `state.inc`, and `task.inc`. Its parent AX source directory contains the host library and `DSPCode.c`.

The [corresponding file metadata](https://github.com/bowsersbigbeanburrito/N/blob/81da5adc12d863744af0906e21bc175c6ff80a8d/DIAG4RVL/trunk/SDK/build_file_tree.json) records file sizes and MD5/SHA-256 values for those inputs. For example, `core.asm` is listed as 99.91 KB with SHA-256 `d4667e61512d3936898a2302e5943eca7378a0f5cc1f5e80341e79dfd597ed65`; the DSP makefile is 3.12 KB with SHA-256 `65165358b60f4197417ed2ff183a3f5767651930db1f4e21138b80bf5a771590`. The build-tools inventory names `dspasm.exe`, `dsplnk.exe`, `hex2bin.exe`, `mxhex2h.exe`, `DspIDE.exe`, and `relnotes_dspasm.txt`. These are concrete original-input and tool candidates, rather than an inference from DSP programming conventions.

This is an archival metadata source, not an authenticated Nintendo publication. Its complete repository tree exposes the inventory files, not the assembly bodies or makefile contents. All AX file modification times in this inventory are May 12, 2018; they must not be presented as original authoring or SDK release dates. The inventory does not connect its `DSPCode.c` hash to this game's `4cc52064` image revision. It establishes specific AX-family assembly-source evidence for D-0015 review, with archive authenticity and exact revision left qualified. No proprietary assembly body was downloaded or reused. The pinned metadata downloads have SHA-256 `ee83bdca8b8196e85a4ce446882fdc3b19e87d6a1ccf6827768aa79d27f9aeed` for the tree and `82c38f26802d0313859cbd9a8510de022c0a1529c35e0346f386df9acf20a1a9` for the JSON.

Two further package inventories in that same pinned archive corroborate the file layout: [RVL SDK 0.2](https://github.com/bowsersbigbeanburrito/N/blob/81da5adc12d863744af0906e21bc175c6ff80a8d/wiidump/RVL_SDK_0_2_package/RVL_SDK_directory_tree.txt) and [BOST SDK](https://github.com/bowsersbigbeanburrito/N/blob/81da5adc12d863744af0906e21bc175c6ff80a8d/wiidump/BOST/SDK_directory_tree.txt) both list those eight AX assembly filenames. The latter also lists CVS metadata directories beside them. These are distinct package inventories within one archive, not independent authenticated publications.

The bounded search also inspected the public Petari and ogws trees. Petari supplies host AX sources but no AX DSP assembly; ogws supplies `DSPCode.c` as a byte array for a different image, with no original assembly filename or generator metadata in that file. It is not a reconstruction input. Current homebrew descriptions of the Macronix DSP16 toolchain do not establish Nintendo AX's source language independently of the SDK inventory.

The fixed exception vectors, explicit accumulator/register mode changes, hardware-loop scheduling, and paired ALU/load/store instructions are consistent with the archival assembly layout. Those native features alone would not prove original language. The coordinator subsequently accepted the combined target identity and concrete family-level archival evidence in [D-0015's AX application](../../DECISIONS.md#2026-11-04-application-to-ax-dsp), preserving the authenticity and exact-revision qualifications. No copied opcode array or generated listing is used as reconstruction source.

## Private readable runtime fragment

The private `tools/local/tail-20261104/smallgame/DSP/ax_runtime_research.ds` reconstructs initialization, resume, command-list reception, bounded command dispatch, invalid-command handling, and DMA waiting. It is written around the semantic identities above, with symbolic mailbox/DMA registers, named host-protocol constants, and routine/loop labels. It is not an automatically renamed full disassembly. External table and persistent-state locations remain named constants until their owning fragments are reconstructed.

The actual private assembler command is:

```powershell
tools/local/dsp-tools/dolphin-assembler/dsp-inspect.exe assemble tools/local/tail-20261104/smallgame/DSP/ax_runtime_research.ds tools/local/tail-20261104/smallgame/DSP/ax_runtime_research.bin
```

Assembly exits zero. Direct comparison of DSP words `0x0010` through `0x0089` finds zero differing bytes across 244 bytes, with range SHA-256 `24f3f325b0d127e53a4ae82dc021f637fb2f7324d7b64a0a470753849f326fa8`. The source SHA-256 is `922889897c5145f6f7dbc48bae5ee895aac2cf9c1ebccb724074bc0a1dd3ee3d`; `runtime_research_comparison.json` records the partial comparison. The leading zero area emitted by `org 0x0010` is outside this comparison and does not reconstruct the exception vectors. This remains a private research result, without production linkage, full-image source coverage, or acceptance credit.

### Runtime expansion

The later private fragments `ax_runtime_vectors.ds`, `ax_runtime_head.ds`, `ax_runtime_isr.ds`, and `ax_runtime_task.ds` cover the complete assigned runtime ranges. They share `ax_symbols.inc` with the command, mixer, and DPL2 investigations. The final combined scaffold resolves code references through real labels in the owning fragments; only the standalone range-check wrapper binds the external command-table label numerically.

`python tools/local/tail-20261104/smallgame/DSP/check_runtime.py` invokes the real private assembler, checks the sealed original image, and compares both owned ranges without normalization. Assembly and comparison exit zero:

| DSP words | Bytes | Range SHA-256 |
| --- | --- | --- |
| `0x0000` through `0x0089` | 276 | `0967dcb970419a90154424e2b8bed155ece5c1b16b741fe553ecb262d23802b6` |
| `0x0F3A` through `0x0FFF` | 396 | `38b3f63f26c0071dc4482ad9edaf1541f6f1221b7d23bbf220a84c39007572e1` |

All 672 bytes agree. `runtime_combined_comparison.json` records the assembler exit, comparison results, and all input-source hashes. The unowned gap in this standalone output is excluded explicitly; its zero fill is not reconstructed code. The task fragment reserves the final twelve words through the declared 4,096-word image extent, preserving every transferred byte.

The vector identities follow the pinned Dolphin DSP exception definitions. The stack-fault handler preserves the return-stack entry and reports it with tag `0xECC0`. Raw-read overflow clears `AXPB.state` only for non-looping voices. Sample-read overflow also redirects a stopped decoder to the silent sample. Looping ordinary voices restore the saved ADPCM predictor and both histories; streaming voices restore the predictor while preserving current histories. The `VOICE_TYPE_STREAM` value of 1 is independently established by the matched `AxVoice::VoiceType` enum and its direct parameter-block assignment.

The task-transfer routine consumes the host's ten messages in order: current DRAM address/size/DSP address, next instruction-image address/size/DSP address, entry vector, and next DRAM address/size/DSP address. It starts the current context save, retains the next-task inputs in the ROM loader's registers, waits for the save DMA, and enters the ROM task loader. All task dispatch entries and internal branches use symbolic labels. These private range checks preceded the coordinator's separate D-0015 review and production acceptance.

## Complete private assembly

The runtime fragments now join the separately reviewed [command/voice source](ax-dsp-command-voice-20261104.md), [processing kernels](ax-dsp-processing-20261104.md), and [tables/DPL2 source](ax-dsp-tables-dpl2-20261104.md). `tools/local/tail-20261104/smallgame/DSP/ax_dsp_research.ds` includes the actual definitions from all four investigators. The full source closure contains 29 files. No diagnostic numeric external-function bindings participate.

The actual `check_combined.py` invocation assembles that master with the pinned private inspector, checks its source closure, and compares the complete output directly against the sealed 8,192-byte image. It exits zero, emits no assembler diagnostics, and finds zero differing bytes. The resulting SHA-256 is `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. `combined_comparison.json` records the assembler SHA-256 and every included source hash. The source audit finds no literal control-flow targets, numeric code-label `EQU` bindings, or numeric `CW` words. All 189 instruction-memory table words are symbolic references: 186 routine labels and three coefficient-data addresses.

An independent investigator [reviewed the same closure](ax-dsp-source-review-20261104.md) and assembled it again, with the same complete byte identity. That review checked unresolved and duplicate symbols, table roles, branch targets, and Harvard-memory uses. Integration initially exposed two same-spelled internal AUX-return labels in distinct routines; assigning the DPL2 channel stages their own descriptive names resolved the collision without changing either fragment's bytes.

A portable frozen copy is under `smallgame/DSP/complete_source/`, with entry `ax_dsp.ds`. Its master begins with `incdir "."` and the assembler runs from that directory. Body and include bytes are unchanged; only the master include-directory paths differ. The real portable invocation also exits zero without diagnostics and emits the same exact 8,192 bytes. `bundle_comparison.json` records its complete provenance/hash manifest; the master SHA-256 is `ad2628643ff82c2fc12277a22a28073780d39997f0b30b3ed148daeb71d985f7`. No original binary resides in that source directory.

The independent review's optional readability refinements are available separately in `complete_source_refined/`: named main/remote depop counts, SRC history count, remaining remote-output count, and the ordinary stereo-output staging/size aliases. The two-file `readability-refinement.diff` changes names only. Actual reassembly exits zero without diagnostics and reproduces the same complete image; `readability-refinement-comparison.json` records its input hashes. The original portable bundle remains frozen for build-wrapper validation.

These private results established a complete readable assembly candidate and exact assembler output. They did not by themselves accept the original-language evidence or the source link. The coordinator then completed those separate steps: the pinned production tool and generated word initializer replaced the AX fallback; normal configure and ninja exited zero, and frozen full-DOL verification passed. The 29 installed source files match [the accepted staging hashes](ax-dsp-staging-20261104.md). The separate [128-byte OS initializer](os-dsp-init-20261104.md) remains unaccepted and is not covered by AX's language evidence or source-link acceptance.

## Traps

- An invalid opcode in a linear DSP listing can be an instruction-memory table word. Here, the checked command index and `ILRR` prove `0x024F` is an address. Classify through its reader before trying to repair the opcode decoder.
- Equal numerical DSP addresses do not imply shared storage. IMEM `0x0D08` is a command-table slot; DMEM `0x0D08` is frequently used as scratch. A reconstruction must preserve this Harvard-memory distinction in its symbol names.
- Code and data pointers can share a contiguous instruction-memory table region. The three SRC entries lead to routines, while the following three coefficient entries select DMEM ROM. Do not label every table word as code merely because `ILRR` reads it.
- A newer or older AX command enum can silently shift handler identities. This image's checked command maximum is 14 and PROCESS itself consumes the parameter-block address; it does not use the older separate PB-address command.
- A source-tree inventory can identify original-language inputs without proving an exact revision. Here the archive lists AX assembly files and content hashes, but does not expose their bodies or connect the generated image to this game. Preserve that distinction when using the evidence for D-0015 review.
- An isolated assembly range can hide global label collisions. The two AUX-return implementations each assembled correctly with diagnostic external bindings, but their common internal names collided in the real combined source. Assemble the complete symbol graph before accepting the image.
- This assembler resolves `INCLUDE` through the active `INCDIR`, relative to its working directory, rather than relative to the including file. `INCDIR` also persists after nested includes and across assembly passes. A portable master must reset it explicitly and preserve the real include closure in dependency tracking.
