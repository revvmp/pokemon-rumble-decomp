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

## F-466-2: Complete OS error, exec, and fatal units

The remaining units are reconstructed from the same pinned Petari revision, under CC0-1.0: [OSError.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/OSError.c), [OSExec.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/OSExec.c), and [OSFatal.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/OSFatal.c). Sources absent from this executable are omitted; the exec-parameter setter and fatal drawing helpers are inline. The retained register-transfer assembly is reference SDK source, with normal C control flow elsewhere.

| Unit | Functions | Function bytes | Text extent |
| --- | --- | --- | --- |
| OSContext | 15 | 2184 | `802B46D0–802B4FB0` |
| OSError | 4 | 1844 | `802B4FB0–802B56EC` |
| OSExec | 11 | 5528 | `802B56F0–802B6CD0` |
| OSFatal | 4 | 3096 | `802B6CD0–802B78F8` |

All 34 functions, totaling 12,652 function bytes, and every owned data section compare at 100% using objdiff-cli 3.8.2. All four source objects are linked as `Matching`; the full rebuilt DOL passes both the configured SHA-1 check and the frozen SHA-256 manifest/direct comparison. The external `__cvt_fp2unsigned` name is now present on main, resolving F-466-1's former blocker.

The existing Wii/1.0 SDK compiler and flags match every unit. The target adds `__OSRestoreMEM1Config(0xBA2CF)` before allocating boot parameters, and its eleven firmware-launch diagnostic line constants are three greater than this reference revision. `ConfigureVideo` supports progressive EURGB60 when VI register 0x36 bit zero is set, mirroring its existing NTSC handling; the reference only selected interlaced EURGB60. These changes are corroborated by target instructions and the complete linked match.

OSError owns `.data` `803C7C08–803C7EE8`, `.bss` `80452590–804525D4`, and `.sdata` `804B8A68–804B8A6E`. OSExec owns `.data` `803C7EE8–803C7F58`, `.bss` `804525E0–80452600`, `.sdata` `804B8A70–804B8A7B`, and `.sbss` `804B94A0–804B94B4`. OSFatal owns `.bss` `80452600–804528D4`, `.sdata` `804B8A80–804B8A84`, and `.sdata2` `804BB230–804BB270`. Unowned alignment padding stays with the following fallback object. String boundaries in OSContext and the new units are annotated so objdiff resolves their otherwise equivalent relocation bases.

The shared OS umbrella now imports existing canonical OSAddress and OSInterrupt declarations instead of duplicating identical types and constants. Minimal standard variadic/wide-string headers follow the pinned Petari declarations; `stdio.h` gains the used variadic printing declarations, and `string.h` gains `memmove` and `strcpy`. The complete `all_source` target validates these shared-header changes against registered source units.

Validation entry points: `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, focused `objdiff-cli diff` for all four units, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. All exit zero. Final integration also repeats these against latest main in a fresh ignored build directory. No original bytes, hashes, or fallback coverage were relabeled as reconstructed source.

## Traps

- Adding a `strlen` declaration to the common string header caused Runtime's weak definition to inherit its earlier `.init` section at the include site, growing the executable's init section. Keep the declaration local to these OS units; both the original section location and exact DOL then pass. This is a demonstrated declaration-placement interaction, not a general prohibition on standard prototypes.
- Reference calls that reread an argument pointer after `strlen` need a named local pointer in this build configuration to retain the target's cached pointer and register allocation. `PackArgs` caches `arg`, and `Halt` caches `msg`; their full instruction streams and linked results match. No compiler-wide aliasing flag change is needed.
