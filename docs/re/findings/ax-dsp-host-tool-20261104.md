# Reproducible AX DSP host assembler, 2026-11-04

## Result and scope

The canonical host-tool setup builds the standalone Dolphin DSP assembler without Visual Studio. Two clean qualification builds at distinct source, compiler, and cache paths produced the same 574,464-byte executable:

`b9afa1a0a47628f6b15464394792707f8a1bc1ba7f27be064b18801d762d47fd`

Both native compiler invocations and acquisition helpers exited zero. Offline verification, corrupted-cache rejection, native disassembly/reassembly, and assembly of the readable 29-file AX program passed. Both assembly routes reproduce the sealed 8,192-byte image:

`d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`

The coordinator subsequently integrated this tool and the readable AX program into the normal build. Native ninja exited zero and frozen-DOL verification passed. The separate [D-0015 application](../../DECISIONS.md#2026-11-04-application-to-ax-dsp) accepts this reconstruction's original-language boundary while preserving the archive-authenticity and exact-revision limits in [the DSP identity finding](ax-dsp-identities-20261104.md#original-language-provenance).

## Packet and pinned inputs

The reviewed private packet is preserved at `tools/local/tail-20261104/sort/DSP/tool_packet`. Its six files were installed byte-for-byte at these canonical repository paths:

| File | Purpose |
| --- | --- |
| `config/dsp_assembler.json` | Exact upstream file, archive, compiler, host-source, and output pins |
| `tools/setup/acquire_dsp_assembler.py` | Acquisition, verified staging, portable build, installation, offline verification |
| `tools/dsp/host/main.cpp` | Native assemble/disassemble/list command-line driver |
| `tools/dsp/host/Common/FileUtil.h` | Host file input |
| `tools/dsp/host/Common/StringUtil.h` | Host lowercase conversion |
| `tools/dsp/host/Common/Logging/Log.h` | Host diagnostic output |

These six owned files contain no code comments or docstrings. The packet contains no executable and no vendored upstream implementation. Fetched sources, licenses, the portable compiler, generated staging files, and build outputs remain in ignored local directories.

The acquisition manifest pins Dolphin commit `66df2b4e18ae6169f9718b869da334022914e305` and fmt commit `e424e3f2e607da02742f73db84873b8084fc714c`. It fetches seventeen files from their commit-specific GitHub raw URLs and verifies each file's exact size and SHA-256 before use. These include the four Dolphin assembler/disassembler/table/label-map implementation pairs, common types, DSP common/core headers, Dolphin's license notice and GPL-2.0-or-later text, three fmt headers, and the fmt license.

All instruction parsing, encoding, decoding, and instruction tables are compiled unchanged. The only adapted upstream file is `DSPCore.h`: the helper retains its copyright/SPDX notice and copies its original enum block, from the first `enum : u32` to immediately before `struct DSP_Regs`, into a small host include wrapper. This omits emulator classes and their dependencies. The complete original header remains separately in the download cache and has SHA-256 `5b5541d66c811e75bc2824d8e5c4ea5c9901d404c57fccaf003bfbbe03222a2e`. No enum, opcode value, or encoding implementation is rewritten.

The portable compiler is [Zig 0.13.0 for Windows x86-64](https://ziglang.org/download/0.13.0/zig-windows-x86_64-0.13.0.zip). Its 79,163,968-byte archive has SHA-256 `d859994725ef9402381e557c60bb57497215682e355204d754ee3df75ee3c158`. The extracted compiler has SHA-256 `2e44af5bbf7a72ef8cbdae370284687c95d65a19affa469d2ad0364d905b8e84`; its root license is separately pinned, and the complete archive's notices remain present after extraction. Archive members are checked for escaping paths and symbolic links before extraction.

The supported host is Windows x64 with the Universal CRT available and Python 3.10 or newer. The independently inspected final executable is PE32+ for x86-64 and imports `KERNEL32` and `api-ms-win-crt-*` API sets. It does not import an MSVC C++ runtime DLL. Portable acquisition removes the Visual Studio compiler/install requirement; it does not make the executable independent of Windows runtime APIs.

## Build and reproducibility controls

The compiler runs through Python's native `subprocess.run`, with its actual return code checked. The build uses:

```text
zig.exe c++ -target x86_64-windows-gnu -std=c++23 -O2 -DFMT_HEADER_ONLY -I. -Iinclude main.cpp Core/DSP/DSPAssembler.cpp Core/DSP/DSPDisassembler.cpp Core/DSP/DSPTables.cpp Core/DSP/LabelMap.cpp -s -o dsp-inspect.exe
```

The helper supplies separate local/global Zig cache directories and sets `LINK=/timestamp:0`. This is the bundled native `lld-link` option consumed through its supported environment input; it is not an output patch.

The original prototype command produced varying COFF timestamps. The prototype and first fresh upstream build had identical bytes in every PE section, but differed at two bytes of the four-byte timestamp field. Passing `-Wl,--no-insert-timestamp` through Zig 0.13.0 failed with an unsupported-linker-argument error. The supported `LINK` control made the timestamp zero and allowed exact repeated builds.

The output basename is also fixed. The first timestamp-zero probe used `dsp-inspect-repro.exe`; that name appears in the PE export directory and changes metadata layout compared with `dsp-inspect-zig.exe`. The final packet consistently builds and installs `dsp-inspect.exe`.

Only owned host sources normalize CRLF to LF before hashing and staging, explicitly recorded in each manifest entry. This accommodates the repository's `core.autocrlf=true` checkout policy. Downloaded upstream files retain their exact pinned bytes. The earlier prototype had an extra trailing newline in each fetched file; the final packet uses the untouched downloads.

## Acquisition and failure behavior

A normal invocation downloads missing pinned inputs, rejects an existing wrong digest, stages a fresh source/compiler directory, invokes Zig, and checks the complete output's pinned size and SHA-256 before publication. It never repairs the executable by editing bytes.

`--offline` verifies cached source/archive inputs, normalized owned inputs, the retained staged source, compiler identity/license, manifest identity, and installed executable. It performs no download or build and creates no directories. `--rebuild` selects fresh staging and cannot be combined with offline verification.

The helper stages both the executable and build report before publication. It retains prior installed files in the new build directory and rolls back completed replacements after a publication error. If recovery itself fails, the diagnostic preserves the original publication error and identifies each retained backup path. Interrupted publication is not treated as acceptance: the next verification requires the manifest and complete executable digest to agree.

The host driver flushes and closes its output stream before checking the result. Invalid output paths return status 2. The separate source-wrapper review also established that Dolphin's low-level assembler may silently ignore an unresolved INCLUDE, so production callers must retain the wrapper's dependency-closure validation rather than rely on the inspector alone.

## End-to-end evidence

The two successful clean installations are `sort/DSP/tool_final_a` and `sort/DSP/tool_final_b`. Each has its own fresh source directory, compiler extraction, both Zig caches, native build log, and `build.json` source hash inventory. Both final executables have timestamp zero and the same complete SHA-256 stated above.

The actual helper invocations used the packet's normal CLI with separate `--local` values and a shared download cache. The shared cache began with the coordinator's already downloaded, digest-verified Zig archive; the helper fetched every upstream source/license file itself. Both builds compiled from fresh staging without borrowing a prototype object or executable.

`python tools/local/tail-20261104/sort/DSP/check_tool_packet.py` performs seven real command-line checks and writes `tool_validation/validation.json`:

| Check | Result |
| --- | --- |
| Offline verification of clean install A | Exit 0 |
| Offline verification of clean install B | Exit 0 |
| One-byte corruption in a separate source cache | Exit 1, explicit wrong-digest diagnostic, installed executable unchanged |
| Disassemble the sealed DSP image | Exit 0 |
| Assemble that diagnostic disassembly | Exit 0, all 8,192 bytes equal |
| Assemble the refined readable AX master | Exit 0, all 8,192 bytes equal |
| Assemble to an output under a missing parent directory | Exit 2 |

The final validation manifest SHA-256 is `efc47f901f8ab131a2b85f4e9c9ee5d186f89128b1869d9c7d930b973d472593`. The report records both complete build inventories and every command, exit code, stdout, and stderr. Its two generated image hashes equal the frozen input hash without normalization.

The independent reviewer additionally reran the refined master and both wrapper word widths, reproducing the original image and all 378 table bytes. Source staging and the higher-level build wrapper are owned by the other DSP lanes. None of these checks changed an original hash.

## Canonical integration and failure propagation

The coordinator ran `tools/setup/acquire_dsp_assembler.py` from the canonical paths into a fresh local installation and reproduced the same pinned host executable. `tools/setup/acquire_tools.py` now invokes that helper with the current Python executable, the manifest selected by `config/toolchain.json`, and the selected local tools directory. It forwards `--offline` and uses `subprocess.run(..., check=True)`. A child failure is caught and causes the parent setup command to return 1.

The source and license review confirmed that the canonical manifest and all five owned code files are byte-identical to the independently reviewed packet. The full original `DSPCore.h`, all other downloaded sources, and the upstream notices/license texts remain in the ignored download cache. The enum-only staged header retains its original notice and enum bytes; encoding source and instruction tables remain unchanged. The tracked DSP source uses semantic routine/state names, symbolic dispatch words, and shared processing includes. Its generated initializer is a build output from `src/RVL_SDK/ax/dsp/ax_dsp.ds`, not a stored original-byte input.

The actual canonical parent command `python tools/setup/acquire_tools.py --offline` exited zero. A second invocation supplied a private toolchain copy selecting a manifest with one deliberately wrong source digest. The child reported the digest mismatch, the parent exited 1, and the accepted installed executable retained its original pinned hash. Only private configuration copies were changed for this negative check. `sort/DSP/canonical_tool_review/acquisition-review.json` records both real invocations and before/after executable hashes; `audit_canonical_acquisition.py` reproduces the review.

The coordinator's accepted normal build selects `RVL_SDK/ax/AXDSPCode.c` from source. The DSP rule consumes the canonical assembler, symbolic master and its include closure, emits the 8,192-byte image and generated big-endian initializer, and passes those outputs to the ordinary Wii compiler. Native ninja exited zero and the full frozen-DOL comparison passed. This finding records AX/tool integration; the project's wider completion inventory remains in the coordinator's progress and acceptance records.

## Traps

- A deterministic compiler invocation can still yield a nondeterministic PE through its linker timestamp. Here all sections matched while the COFF header changed. The actual supported timestamp control, not binary rewriting or section-only hashing, supplied a reproducible complete executable.
- The output basename can be part of an executable's contents. This tool's PE export directory embeds the basename; hold it fixed during whole-file determinism checks.
- A successful PowerShell pipeline observation is weaker than the native process result. The packet invokes the compiler directly from Python and checks its return code before reading or publishing an output.
- Repository line-ending conversion and immutable upstream downloads need distinct treatment. Normalize only explicitly owned host files; preserve and verify fetched source/license bytes exactly.
