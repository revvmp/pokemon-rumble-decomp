# Issue 540: MSL math and string units

## F-540-1: Fifteen source units cover the complete block

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, original `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The issue's `0x8034D184–0x8034D83C` range contains fifteen functions totaling 1,720 code bytes, each in its own source unit. The omitted reference-list entry at `0x8034D2F0` is `modf`, not `cos`.

Source provenance is Petari at `e0ab141914921726a595573dc48d3f13d0922efe`, plus Sun fdlibm `s_modf.c` revision 1.3 from Netlib. [Source notices](../licenses/540-msl-math-NOTICE.md) preserve the Sun permission notice and distinguish the independently written `w_log.c` wrapper. The Petari reference's unlinked `strncasecmp` and `strcasecmp` wrappers are omitted from `extras.c`; its `stricmp` matches this target. Headers declare only the interfaces and bit-access macros required by these units, extending the existing public `math.h`.

The wrapper function names are supported by the corresponding SDK reference wrappers, link order, and their branch destinations. Their `w_*.c` filenames are conventional reference names; the unavailable `w_log.c` filename is inferred rather than recovered from an embedded string. Outside callee names are recorded without changing their splits, sizes, or types. Diagnostic compilation of the log, log10 and power callees independently reproduces their non-relocation instructions; differences in the other reference callees remain outside this issue's scope.

Each file is compiled with Wii/1.1 and four-byte function alignment, consistent with F-0007. The new `cflags_msl` derives from the existing runtime flags, disables FP contraction and enables file IPA. This flag evidence applies to the newly reconstructed units; future MSL files still require their own object and executable comparisons.

`s_ldexp.c` owns `.sdata2` `0x804BBBB8–0x804BBBE0`, `s_sin.c` owns `0x804BBBE0–0x804BBBE8`, `s_tan.c` owns `0x804BBBE8–0x804BBBF0`, and `e_sqrt.c` owns `0x804BBBF0–0x804BBBF8`. All other new units contain only code. Together they replace 1,720 code bytes and 64 constant-data bytes; no original code bytes are embedded in source.

Evidence uses GNU binutils 2.42-2 symbols, instructions and relocation records, dtk 1.8.4 splits, MWCC Wii/1.1 diagnostic objects, and objdiff-cli 3.8.2 complete-object comparisons. SDA/SDA2 loads establish data addresses; relative branch relocations establish external callees. Every new object's code and data sections compare at 100%.

## F-540-2: The target's `nan` is the reference's empty-body function

`0x8034D794` contains one `blr` instruction. Petari's `MSL_C/PPC_EABI/SRC/math_ppc.c` implements `double nan(const char*)` with only a preprocessor definition inside the body, which emits the same instruction. The target's narrow and wide numeric parsers call this address after collecting a parenthesized NaN tag and optionally negate its result; the call sites at `0x80346538`, `0x80346544`, `0x80347E60` and `0x80347E6C` support the reference name and signature. This is a preserved target behavior, not a replacement implementation invented to fill an unknown function. It does not provide a defined C return value; changing it to construct a NaN would change the original bytes.

## Traps

- With FP contraction enabled, `__ieee754_sqrt` compiles to `0x244` bytes and fuses `x * x + x`; the target is `0x248` bytes with separate multiply and add instructions. Disabling contraction produces the complete target stream. This conclusion is local to the tested sources, not a new assumption for all MSL code.
- A one-instruction function cannot be named from a masked byte search alone: masking its relocation removes every instruction byte. Wrapper names require the reference interfaces, branch destinations and neighboring unit order. The return-only `nan` additionally needs caller and source evidence.

## Validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0 for the baseline, split-only and source-linked builds. Fifteen complete `objdiff-cli diff` comparisons also exited 0 and show 100% for every code and data section. The report records 15/15 matching, source-linked functions and 1,720/1,720 code bytes for the new units, plus 64/64 data bytes. The frozen SHA-256 and direct byte comparison pass without changes to original artifacts or expected hashes.
