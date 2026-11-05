# AX DSP private source review

## Review scope

This review examines the 29-file include closure rooted at `tools/local/tail-20261104/smallgame/DSP/ax_dsp_research.ds`. It covers the runtime, command/voice, filter/mixing, and table/DPL2 contributions. The table/DPL2 contribution is the reviewer's own preceding work; the other three contributions received an independent source review. Files outside this master, including standalone diagnostic address bindings and generated disassemblies, are not reconstruction inputs.

`tools/local/tail-20261104/layout_dsp/review_sources.py` records the include closure, source hashes, symbol definitions, control targets, data directives and table operands in `source-review.json`. It also invokes the real assembler with a separate output, `review-master.bin`, and checks that no reviewed file changed during the run. Manual inspection supplies the organization and naming judgments below; the automated checks do not establish original-language provenance or source readability by themselves.

## Source structure

The master uses actual labels for every routine within the image. There are no numeric direct branch targets, unresolved direct targets, in-image code-address `EQU` bindings, direct numeric memory operands, numeric `CW` operands, embedded binary directives, or code comments in the reviewed closure. The only direct control targets defined by numeric constants are the documented DSP ROM boot and task-switch services at `0x8000` and `0x80B5`. These are external ROM services, not unresolved routines within the reconstructed image.

The named units express the recovered program structure: task startup and mailbox handling; studio initialization and command processing; voice envelope, filtering, SRC and channel routing; shared constant/ramped mixing kernels; DPL2 upload, return mixing and output; and instruction-memory dispatch tables. Channel-specific setup and mix wrappers use named parameter-block fields and buffer roles. Repeated filter, SRC-history and mix-pipeline sequences are factored into named include fragments. Pipeline prologues and epilogues remain visible where the native scheduling requires them. I found no remaining numeric-disassembly label scheme or opaque instruction-word array in these sources.

The first combined assembly exposed two global-name collisions: ordinary AUX and DPL2 channel stages both used `AxDspMixAuxReturnLeft` and `AxDspMixAuxReturnRight`. The DPL2 stages now use the `AxDspDpl2AuxReturn` prefix, including their surround counterparts. The resulting closure has no duplicate symbol definitions. The DPL2 source SHA-256 after this naming correction is `0553321027eab66b222a975466a191a62066ca5d28872672c254ba5d903188d7`; the owned machine-code bytes did not change.

## Instruction and data memory

The tables use labels in instruction memory. Their readers obtain entries through `ILRR`, and cached routine pointers reside in named data-memory state. The coefficient selector table contains three named data-ROM bases instead of code labels. No instruction label is used as a direct data-memory address in the reviewed source.

The overlapping numerical addresses are represented by different semantic names. In particular, the data-memory scratch/ramp buffer at `0x0D08` is distinct from the command-table slot at the same instruction-memory address, and data-memory biquad coefficient storage at `0x0D03` is distinct from the corresponding command-table slot. DPL2 aliases describe reused channel storage without changing its allocation or declaring an unsupported meaning for the alternate AUX C selector.

The retained instruction-memory tables contain **189 words, or 378 bytes**: 185 words at `0x0D02` through `0x0DBA`, plus four task-mail words at `0x0F95` through `0x0F98`. Of these, 186 entries reference actual code labels and three reference the coefficient-ROM constants. Their exact source counts are 15 command entries, five groups of 32 mixer entries, four remote entries, three SRC entries, three coefficient entries, and four task-mail entries. Both table ranges match the frozen image separately.

## Readability follow-ups

The following small naming improvements were sent to the owning agents. They do not identify encoding mismatches:

- The ordinary output handler can use `AX_OUTPUT_STEREO_BUFFER` and `AX_STEREO_OUTPUT_BYTES` for its final DMA, matching the semantic aliases already used by DPL2. The existing AUX A buffer and channel-size constants have the same values, but describe a different active role and sample layout.
- The command/voice source's literal counts 12 and 8 clear main/AUX and remote depop channels; four is the SRC history length; three is the remaining remote outputs after the first transfer. Named channel/history/output counts would make those relationships explicit.

The twelve final zero words remain explicitly reserved image storage through the image-end boundary. Their original intended role is still unknown. This review does not reinterpret them as removable alignment padding or infer their provenance from the fact that `org` reproduces them.

The optional naming changes above were subsequently applied in the separate `smallgame/DSP/complete_source_refined` packet. The original reviewed files were held fixed. Independent assembly of that refined packet with the final portable tool reproduces every byte and all 378 table bytes; the [tool review](ax-dsp-tool-review-20261104.md) records the final source hashes and actual CLI evidence.

## Exact encoding and acceptance limits

Independent assembly of the reviewed master exits zero and produces exactly 8,192 bytes with SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`, equal to the frozen AX image. No reviewed source changed during the comparison. The separate 370-byte and 8-byte table comparisons also pass. The assembler executable is the pinned private inspector described in the [image investigation](ax-dsp-image-20261104.md).

The source is organized as a readable symbolic DSP program under the recovered behavior, with the naming refinements above implemented in the final packet. That is a source-fidelity judgment. The exact hash is an encoding result. Neither authenticates the archived AX build inventory or connects its assembly-file revision to this game. [The original-language evidence and its limits](ax-dsp-identities-20261104.md#original-language-provenance) remain distinct from the coordinator's qualified D-0015 acceptance. Portable tool construction, independent assembly, and the later canonical dependency/final-link audit are recorded in the [tool review](ax-dsp-tool-review-20261104.md#canonical-production-acceptance). The production audit confirms all 897 units are source-selected, all 902 final-link aliases preserve their compiled source inputs, and the main executable retains its frozen hash.

## Traps

- Independently exact fragments can reuse the same global semantic label for different routines. Assemble the complete source with actual labels and reject duplicate definitions before treating the per-fragment comparisons as integration evidence.
- A complete source review must follow the master's include closure. Standalone diagnostic `EQU` bindings and generated listings exist nearby but are not inputs to the actual-label reconstruction.
