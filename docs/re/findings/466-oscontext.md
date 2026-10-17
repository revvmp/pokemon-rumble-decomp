# Issue 466: OSContext reconstruction

## F-466-1: Complete OSContext unit and source comparison

Target `wpse-v1`, `orig/wpse-v1/main.dol`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The complete first unit of issue #466 is `OSContext.c`: `.text` `0x802B46D0`–`0x802B4FB0` (2,272 bytes including alignment), `.data` `0x803C7A28`–`0x803C7C08` (480 bytes including alignment). The text contains 15 functions, totaling 2,184 function bytes. The following unit begins with `OSReport` at `0x802B4FB0`; its first string begins at `0x803C7C08`.

Source and layout follow [Petari OSContext.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/OSContext.c) and its [OSContext.h](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/libs/RVL_SDK/include/revolution/os/OSContext.h), under CC0-1.0, corroborated with target instruction and relocation inspection. Reference functions absent from this target were omitted. The implementation preserves the reference's CodeWarrior inline assembly for register state transfer; this is independently maintained source, not an original-byte or object fallback.

The five unnamed functions recover as `OSGetCurrentContext` (`0x802B49A0`, 12 bytes), `OSSaveContext` (`0x802B49B0`, 128 bytes), `OSSwitchFiber` (`0x802B4B20`, 48 bytes), `OSSwitchFiberEx` (`0x802B4B50`, 48 bytes), and `OSInitContext` (`0x802B4BB0`, 188 bytes). Names have high confidence from reference function order and complete code comparison. `.text` DOL file offsets equal addresses minus `0x8004FD40`.

The SDK compiler is Wii/1.0 with the existing `cflags_sdk` and default 16-byte alignment (F-0007). The low-memory context pointers are absolute `0x800000D4` and `0x800000D8`. The restartable interrupt-disable sequence used by `OSLoadContext` spans `OSDisableInterrupts` through `OSDisableInterrupts + 12`; target relocations directly establish this extent.

`objdiff-cli 3.8.2 report generate` reports 15/15 functions and both sections at 100%, including 2,184 matched function bytes and 480 matched data bytes. This is a comparison result, not source-link acceptance. The unit deliberately remains `NonMatching`: `OSDumpContext` emits four relocations to the compiler helper `__cvt_fp2unsigned`, while the corresponding target relocations name `fn_8034033C` at `0x8034033C`, outside issue #466. That external symbol was preserved under the ownership rule. No new linked source coverage is claimed.

The split-only build and full build with the registered `NonMatching` source both preserve the frozen executable. The full verifier command is `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. Local diagnostic assembly and the focused objdiff report are under ignored `build/reference/`.

## Context ABI

The context is 0x2C8 bytes, aligned to eight bytes. General registers occupy `0x000`–`0x07F`; `cr`, `lr`, `ctr`, and `xer` are at `0x080`, `0x084`, `0x088`, and `0x08C`. Thirty-two double FPRs begin at `0x090`; FPSCR padding/value are at `0x190`/`0x194`; `srr0`/`srr1` at `0x198`/`0x19C`; 16-bit `mode`/`state` at `0x1A0`/`0x1A2`; eight GQRs at `0x1A4`; paired-single padding at `0x1C4`; and 32 double paired-single slots at `0x1C8`. Reference declarations and target load/store offsets agree. State bit 0 marks saved FPU state; bit 1 selects the exception restoration path.

## Remaining work

Coordinate the external conversion-helper name with its owning issue, then enable source linking and perform exact full verification before changing the status to `Matching`. Issue #466 still includes OSError, OSExec, and OSFatal; none is claimed reconstructed by this change. No new generalized trap emerged; the external-symbol ownership dependency is recorded here for the next session.
