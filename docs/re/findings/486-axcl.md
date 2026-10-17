# Issue 486: AX command lists

## F-486-1: Shared output-mode symbol

For `wpse-v1` / `orig/wpse-v1/main.dol`, `lbl_804B98B0` is the original SDK global `u32 __AXClMode`. The frozen manifest remains `config/targets/wpse-v1.json`.

Evidence is independently checked against [ogws AXCL.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXCL.c): `__AXClInit` initializes this global to stereo (0), `AXSetMode` stores its argument, and `__AXNextFrame` dispatches modes 0, 1, and 2. Target `0x802DBC90` initializes the word with `stw r7,-16752(r13)` at `0x802DBCB4`, where r7 is zero. Target `0x802DBCE0` consists of `stw r3,-16752(r13)` and `blr`. Target `0x802DB344` loads that same word and branches on 0, 1, and 2. With the established r13 base `0x804BDA20`, the displacement resolves exactly to `0x804B98B0`. These observations corroborate AXAux's mode-2 comparisons documented by #485.

The symbol has four-byte `.sbss` storage with no file-backed offset. The initial dependency change renames only this symbol, preserving its section, address, size, and attributes. It does not reconstruct code or claim additional source coverage. AXCL begins inside #486 at `0x802DB290`; the rest of this issue remains to reconstruct. This dependency unblocks AXAux PR #553.

Validation: baseline configure/build succeeds. After the rename, `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. The full DOL remains exactly identical. This symbol-only change adds no source units; objdiff coverage is unchanged. No new reusable tooling trap emerged.

## F-486-2: Command-list, output, studio, and parameter-buffer reconstruction

The 35 functions in issue #486 cover 8,008 instruction bytes. They are reconstructed as C with Wii/1.0, existing `cflags_sdk`, and default 16-byte alignment. Source follows CC0-1.0 ogws commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73`, files `src/revolution/AX/AXCL.c`, `AXOut.c`, `AXSPB.c`, and `AXVPB.c`. The root license and individual source files were inspected. Reference comments and decompilation helper annotations are not included.

| Source | Text extent | Functions | Code bytes | Owned data bytes |
| --- | --- | --- | --- | --- |
| AXCL.c | 0x802DB290-0x802DBD68 | 12 | 2,708 | 292 |
| AXOut.c | 0x802DBD70-0x802DC5A8 | 12 | 2,036 | 3,644 |
| AXSPB.c | 0x802DC5B0-0x802DC9DC | 6 | 1,040 | 200 |
| AXVPB.c implementation fragment | 0x802DC9E0-0x802DD2A0 | 5 | 2,224 | 0 |

AXVPB continues into neighboring issue #487. The split at 0x802DD2A0 is an implementation boundary, not a claimed original translation-unit boundary. The five functions here include naturally inlined service helpers and reference the original shared parameter-buffer pointers, counters, and cycle tables externally. The later AXVPB functions and their data remain fallback dependencies assigned to #487. No original instruction bytes are copied into reconstructed source. The compressor table, DSP program image, and profile getter also remain externally linked data/code owned outside this issue.

### Target revisions

AXCL retains twelve functions: the command writer is naturally inlined and AXGetMode is absent. Unlike the reference constants, this target stores a compressor-table pointer and a 16-bit release-frame count. Initialization at 0x802DBC90 sets the pointer to 0x803D8620 and count to ten. Command generation at 0x802DBA10 and 0x802DBA24 loads those fields. Their original source names are unavailable; `sCompressorTable` and `sCompressorReleaseFrames` describe proven behavior. The threshold remains immediate 32768.

AXOutAiCallback first returns when the volatile DSP completion flag equals one, evidenced at 0x802DBFDC-0x802DBFE4. This guard is absent in the reference. The DSP task priority requires a volatile qualifier; see [F-486-3](486-dsp-priority.md). Profile storage is 56 bytes, followed by alignment to the remote buffers; the final four bytes are structure tail padding. DSPTaskInfo is 80 bytes with alignment padding to the following DRAM image. AXSTUDIO uses twenty packed signed-word/halfword pairs, size 120 bytes and 32-byte alignment.

### Cross-unit names

Original callsites matched against the reference identify AIRegisterDMACallback at 0x802D9EE0, AIInitDMA at 0x802D9F30, AIStartDMA at 0x802D9FB0, AIGetDMABytesLeft at 0x802D9FD0, DSPCheckMailToDSP at 0x802E3AE0, DSPSendMailToDSP at 0x802E3B20, DSPInit at 0x802E3B40, DSPCheckInit at 0x802E3C00, DSPAddTask at 0x802E3C10, DSPAssertTask at 0x802E3C80, OSWakeupThread at 0x802BCF60, and __AXGetCurrentProfile at 0x802DD7A0. These names change neither outside function sizes nor splits. Allocation callees are shared with merged issue #485.

### Validation

All 35 functions compile to 100% objdiff matches, totaling 8,008 code bytes. AXCL, AXOut, and AXSPB also show 100% for all 4,136 owned data bytes. Split-only builds and the AXCL/AXSPB source-linked build passed the frozen main.dol verifier. The all-source build then passed `ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, both exit 0. The final PR records committed-head validation after the last main rebase.

### Traps

A reference SDK may use a constant where this release stores configuration. Check the target initializer and command writer together; the AXCL compressor pointer and release count explain the version difference without changing compiler flags.

The imported DSP task priority lacked a volatile qualifier. Its correction restores original AXOutInitDSP scheduling. Read the companion finding before changing statement order to compensate for a shared-header mismatch.
