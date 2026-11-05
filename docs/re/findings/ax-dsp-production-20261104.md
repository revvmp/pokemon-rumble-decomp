# AX DSP source and normal-build acceptance

The normal `wpse-v1` build now reconstructs the 8,192-byte AX DSP program from 29 readable assembly files and selects all 897 logical units from source. No original split object appears among the 902 raw final-link inputs. All 13,462 CPU functions, 3,078,460 CPU code bytes and 1,788,360 selected data bytes are complete under the measured denominator. The separate firmware-source limitations below prevent treating this selection count alone as proof that every embedded program has recovered source.

## Source and original-language evidence

The assembly entry point is `src/RVL_SDK/ax/dsp/ax_dsp.ds`. Its include closure implements task startup and interrupts, command dispatch, voice processing, filtering, sample-rate conversion, mixing, DPL2 output and symbolic dispatch tables. Repeated processing sequences have named include fragments. Internal branches reference actual labels; the only external code addresses are identified DSP ROM services. The final twelve reserved zero words remain part of the original transfer extent.

[D-0015's AX application](../../DECISIONS.md#2026-11-04-application-to-ax-dsp) records why the target identity and archived AX assembly inventory support this source language. The archived metadata is not an authenticated Nintendo publication or the exact original source revision. The independently written source uses target behavior and verified host interfaces; it does not reuse proprietary assembly bodies. [The staged inventory](ax-dsp-staging-20261104.md), [source review](ax-dsp-source-review-20261104.md) and individual [runtime](ax-dsp-identities-20261104.md), [command](ax-dsp-command-voice-20261104.md), [processing](ax-dsp-processing-20261104.md) and [table/DPL2](ax-dsp-tables-dpl2-20261104.md) findings preserve the reconstruction evidence.

The host declaration remains `u16 axDspSlave[4096]`, with 32-byte alignment, in `src/RVL_SDK/ax/AXDSPCode.c`. Its generated initializer is a build output. The generated binary and C include are never tracked reconstruction inputs. The original host interface, symbol address `0x803D95E0`, and 8,192-byte extent are unchanged.

## Pinned and dependency-tracked build

`tools/setup/acquire_tools.py` invokes `tools/setup/acquire_dsp_assembler.py`. `config/dsp_assembler.json` pins the Dolphin and fmt source revisions, every downloaded source/license file, the Zig 0.13.0 archive and executable, owned host shims, compiler arguments and final assembler. The supported host is Windows x86-64 with Universal CRT; Visual Studio is not required. External source and notices remain in the ignored tool cache. Only the independently authored host shims and acquisition code are tracked.

The final assembler is 574,464 bytes, SHA-256 `b9afa1a0a47628f6b15464394792707f8a1bc1ba7f27be064b18801d762d47fd`. Two fresh source/compiler/cache builds produce that exact file. The pinned linker environment clears the PE timestamp through the supported native linker option; no binary is patched. The [host-tool evidence](ax-dsp-host-tool-20261104.md) records the timestamp, basename and line-ending controls, license handling and real acquisition rejection paths.

Configure adds a normal pre-compile edge. `tools/dsp/build_dsp.py` invokes the assembler on the readable source, emits the unchanged binary and a big-endian 16-bit initializer, and records all 29 assembly files plus the tool and wrapper as 31 Ninja dependencies. It reads no original image, acceptance manifest or expected content hash. Missing includes are rejected explicitly because the underlying assembler can silently omit one. Input/size/assembly failures preserve prior outputs; the real locked-output case exercises publication rollback. See [the wrapper evidence](ax-dsp-build-wrapper-20261104.md).

The original DOL is still used by the established dtk analysis, layout and comparison workflow. It supplies no selected original object in the final link and is not an input to either the DSP generator or the semantic system-message encoder. This distinction does not claim that the existing build command can run without its original analysis reference.

## Full artifact and clean-build proof

The isolated `ax-dsp-production` trial replaced the original DSP object with the ordinary compiler's source object. Native link and ELF-to-DOL conversion exited zero; every DOL byte agreed. The normal configuration then marked AXDSPCode Matching. After deleting the previous DOL, configure and ninja exited zero and the frozen verifier passed.

Two complete builds from an empty `build/wpse-v1` subsequently ran the real `build.bat` entry point. Both acquired/verified the pinned tools, configured, compiled, linked and passed frozen verification. Both reproduced all seven artifacts identically to each other and to the preceding incremental build:

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `main.dol` | 4094880 | `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da` |
| `main.elf` | 6036304 | `bd7b247419913824c9f91747c129b9092433cf7ed0cbbea48ce643c232200ee3` |
| `report.json` | 4561010 | `c88be97be32c594219052939c0a613ccc4c82eec59cd4ca6f0942421a29ef54e` |
| `assets/system_message.arc` | 11488 | `e27d380bbad3aff2ef2d41cfdc5b77590390a68937fef6edc87502f2d1813161` |
| `include/system_message.inc` | 68928 | `fd3a483529c348ffc3eb9b27ee3d59f43268b17e8ae9417fa16f827bacbe76d5` |
| `dsp/ax_dsp.bin` | 8192 | `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed` |
| `include/ax_dsp.inc` | 34816 | `d3c3f8e6f3f8cc6d4fe16899eb479ab8dcde0ee2c3e2568644c12f108c122fdd` |

Private receipts are `tools/local/tail-20261104/all-source-clean-one.json` and `all-source-clean-two.json`, with corresponding complete logs. The actual commands were `build.bat`, followed by the independent original-address/alias audit and `tools/re/audit_alignment.py`. All prior exact functions remain present and exact. All 902 aliases preserve the selected compiler objects byte-for-byte; all 470 native alignment ranges, totaling 3,792 bytes, remain exact. The [independent production audit](ax-dsp-tool-review-20261104.md#canonical-production-acceptance) also verifies installed source/tool hashes, the absence of public generated DSP binaries, and both Ninja dependency records.

## Scope and remaining embedded firmware

The CPU decompilation and AX DSP reconstruction are accepted. The pre-existing 128-byte `DSPInitCode` array in OSAudioSystem and the Bluetooth controller patch embedded in WUD are executable coprocessor payloads, despite residing in data in already Matching CPU units. Their readable private candidates do not establish original authoring language. The owner subsequently approved exactly these two original-language exceptions under D-0017, and both now build from reviewed symbolic source through the normal pipeline. Their original language remains qualified. See [OS initialization](os-dsp-init-20261104.md), [the firmware audit](embedded-code-audit-20261104.md) and [final repeated clean acceptance](main-source-completion-20261104.md).

The frozen acceptance scope is main.dol. Boot-loader content 6, other content assets and WAD packaging are outside it. No full release/package reconstruction is claimed.

## Traps

- Source selection and embedded-code reconstruction require separate reviews. An ordinary Matching C owner can still contain an instruction array; review its consumers and payload format before treating every data byte as reconstructed program source.
- The portable host linker inserts a timestamp unless its supported timestamp option is set. Fixed source and compiler alone did not produce a stable executable hash.
- A raw DSP assembler success can hide a missing include. The production wrapper validates the real include closure and the emitted size before publishing output.
