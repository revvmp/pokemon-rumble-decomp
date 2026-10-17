# F-483-1: AI unit boundary and target revision

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. [F-484-1](484-ai.md) established that `AIRegisterDMACallback` at 802D9EE0 is the AI unit's first function, so #483 owns its split. Ten subsequent AI functions belong to #484; their implementation shares this unit's static callback and timing state.

The reconstruction uses [Petari ai.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/ai/ai.c), commit `e0ab141914921726a595573dc48d3f13d0922efe`, CC0 with no additional file-level notice on the consulted file. Existing imported `revolution/ai.h`, OS headers and `private/flipper.h` supply the SDK types and absolute hardware-register declarations. The target version is `Feb 27 2009 10:01:30 (0x4302_145)`, read from the original DOL; source preserves the original banner whitespace. The calibration loop is rewritten with descriptive timing/sample names and without decompiler-style temporary names.

`AIGetDMABytesLeft` at 802D9FD0 reads DSP register 0x1D, masks 15 bits and converts blocks to bytes. `AICheckInit` at 802DA010 returns the four-byte initialization flag. Unused `AIStopDMA` is absent in this target; sample-rate accessors inline into initialization. A single local timer-clock value reproduces this target's clock read and five time conversions; repeated macro expansions load the clock again between static stores and produce a different 0x1C0-byte initializer. The accepted initializer is 0x180 bytes.

`__AICallbackStackSwitch` at 802DA250 is the reference's original-style CodeWarrior assembly needed to change stacks, 0x64 bytes. It saves/restores the old stack pointer, loads the callback stack and invokes the callback. It is source assembly, not an executable-byte payload. C reconstruction is not claimed for that routine.

| Unit | Section | Start | Exclusive end |
| --- | --- | --- | --- |
| `ai.c` | `.text` | `802D9EE0` | `802DA48C` |
| `ai.c` | `.sbss` | `804B9800` | `804B983C` |
| `ai.c` | `.sdata` | `804B8BC8` | `804B8BCC` |
| `ai.c` | `.data` | `803D84E8` | `803D852D` |

Wii/1.0 with current cflags_sdk, file IPA and default 16-byte function alignment reproduces all eleven retained non-relocation instruction streams and the initialized version data. AI passed complete object and source-linked DOL acceptance below. DVD acceptance remains pending. The new split excludes trailing padding before AX.c.

## F-483-2: DVD register-name correction

The target's low-level DVD unit starts at 802D7830 and ends at 802D9ED8. Petari's pinned `src/RVL_SDK/dvd/dvd_broadway.c` provides the reference. The target adds control-register retrieval and preparation. `DVDLowPrepareControlRegister` at 802D9BC0 sends command 0x96 and names itself explicitly in its failure string. Its getter at 802D98D0 reads the same 32-byte buffer's first word. Command 0x95 and its diagnostic identify `DVDLowPrepareStatusRegister` at 802D9A50, with getter at 802D98C0. `DVDLowPrepareCoverRegister` at 802D98E0 sends 0x7A through `doPrepareCoverRegisterCallback`; its getter at 802D98B0 reads `diRegValCache.CoverRegVal`.

Four existing symbol names are shifted: the status getter/preparer were labelled cover, and the control getter/preparer labelled status. Correcting them requires corresponding name-only substitutions in the existing matching `dvderror.c`, preserving all original branch targets. That caller update is outside #483's range and has been presented to the user for scope approval; no caller edits have been applied.

The initial full-reference compilation agrees with every retained DVD instruction stream and all initialized non-relocation data after adding the control operation and preserving its exact diagnostic spelling. Removing unlinked public APIs changes the shared diagnostic-string layout, so dead-stripping and complete object/link behavior are being validated before source acceptance.

## Traps

- Sample calibration must use the target's single clock read. Repeated macros can reload through static stores under this compiler's alias analysis; the cached local is verified only for this AI initializer.
- Reference field names and existing symbol guesses can disagree with the target's embedded diagnostics. Verify command numbers, buffers and call sites before renaming linked source dependencies.

## AI source-linked acceptance

`python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0. Split-only and source-linked DOLs pass the frozen manifest. `objdiff-cli diff` reports 100% for AI text, data, sdata and sbss. All eleven functions, 1,388 instruction bytes, are linked from source: ten C routines totaling 1,288 bytes and the 100-byte stack-switch assembly routine. This covers the final function of #483 and ten AI functions of #484. The DVD portion of #483 and AX initializer portion of #484 remain pending; neither issue is complete.

Frozen DOL SHA256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope is the main game DOL only, excluding boot content 6, data contents and WAD packaging. Final rebased validation is recorded in the pull request.
