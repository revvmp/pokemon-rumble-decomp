# Issue 305: HBM sound, blocks 1 and 2

## F-305-1: mix, syn, synctrl, synenv, synmix, synpitch, synsample, synvoice and seq

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The blocks of #305 and #306 (`0x8019A2D0`–`0x8019DC48`, 56 functions) are the Home Button Menu's private copy of the SDK MIX, SYN and SEQ libraries, nine units. `synmix.c` starts in #305's block and ends in #306's, so both issues are done here.

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/mix.c` | `0x8019A2D0`–`0x8019BC3C` (15 functions) | `.data` `0x803BCE78`–`0x803BDA08`, `.bss` `0x803E9C60`–`0x803EA31C` |
| `RVL_SDK/hbm/syn.c` | `0x8019BC40`–`0x8019C0DC` (8 functions) | `.bss` `0x803EA320`–`0x803EA7EC` |
| `RVL_SDK/hbm/synctrl.c` | `0x8019C0E0`–`0x8019C578` (4 functions) | `.data` `0x803BDA08`–`0x803BDC08` |
| `RVL_SDK/hbm/synenv.c` | `0x8019C580`–`0x8019C95C` (2 functions) | `.rodata` `0x80366F68`–`0x80366F88` |
| `RVL_SDK/hbm/synmix.c` | `0x8019C960`–`0x8019CAA0` (5 functions) | `.data` `0x803BDC08`–`0x803BDF98` |
| `RVL_SDK/hbm/synpitch.c` | `0x8019CAA0`–`0x8019CD1C` (4 functions) | `.rodata` `0x80366F88`–`0x80366FA0`, `.data` `0x803BDF98`–`0x803BE388` |
| `RVL_SDK/hbm/synsample.c` | `0x8019CD20`–`0x8019D194` (4 functions) | none |
| `RVL_SDK/hbm/synvoice.c` | `0x8019D1A0`–`0x8019D350` (3 functions) | none |
| `RVL_SDK/hbm/seq.c` | `0x8019D350`–`0x8019DC48` (11 functions) | `.rodata` `0x80366FA0`–`0x80366FC8`, `.data` `0x803BE388`–`0x803BE408`, `.bss` `0x803EA7F0`–`0x803EA7F8` |

The unit names, function names, sizes and offsets are identical to CC0 xenoblade's `config/jp/splits.txt` and `symbols.txt` (`ea25691b`). The sources follow CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/sound`), whose tables match the target data byte for byte.

Evidence:

- **C++ compilation.** The units are built with the `homebuttonLib` flags (`-lang=c++`). File-local functions carry C++ mangled names (`__HBMMIXSetPan__FP13HBMMIXChannel`, `__HBMSEQRunEvent__FP11HBMSYNSYNTHP12_HBMSEQTRACK`), while the public API and the `__HBMSYN` functions shared between units are `extern "C"`. The new private header `include/revolution/hbm/sound/synprivate.h` declares those shared functions, the tables and the 0x4C-byte `HBMSYNVOICE`.
- **Stripped helpers.** `__HBMMIXResetChannel`, `__HBMMIXClampPan`, `__HBMMIXGetVolume`, `__HBMSYNAddSynthToList`, `__HBMSYNSetController`, `__HBMSYNGetWavetableData`, `__HBMSYNGetEnvelopeTime`, `__HBMSYNGetNibbleAddress` and the small `seq.c` event handlers are inlined into their callers and have no copy in the DOL.
- **Asserts shape inlining.** `__HBMSEQRunEvent` keeps its own copy, called from `HBMSEQRunAudioFrame`. Without the reference's assertions, the compiler inlined it into `HBMSEQRunAudioFrame`, which then failed to match. Release-build `ASSERT` and `ASSERTMSG` statements, which generate no code, keep the inliner's size estimate above its limit. `seq.c` defines both as `((void)0)`, as `Pad.c` does, and places them where the reference does.
- **Track chunk scan.** `__HBMSEQInitTracks` matches only with the reference's backward jump to the next chunk header; a `while` loop changes the block order.
- **Data.** The tables and their order are `__HBMMIXVolumeTable`, `__HBMMIXPanTable`, `__HBMMIX_DPL2_front`, `__HBMMIX_DPL2_rear` (`mix.c`), `__HBMSYNn128` (`synctrl.c`), `__HBMSYNVolumeAttenuation`, `__HBMSYNAttackAttnTable` (`synmix.c`), `__HBMSYNCentsTable`, `__HBMSYNOctavesTableUp`, `__HBMSYNSemitonesTableUp`, `__HBMSYNSemitonesTableDown` (`synpitch.c`) and `__HBMSEQMidiEventLength` (`seq.c`).

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with all nine units `Matching`. objdiff reports all 56 functions and every `.text`, `.data` and `.bss` section at 100%. The `.rodata` of `synpitch.c` (90.9%) and `seq.c` (94.7%) differs only in trailing alignment padding. The linked DOL is byte-identical.

### Traps

- **Release-build asserts can decide inlining.** `((void)0)` assertions produce no code, but CodeWarrior's auto-inliner counts them. Leaving them out of `__HBMSEQRunEvent` let it be inlined into its only caller. If a static function that has its own copy in the target is inlined in your object, restore the reference's assertions before trying other changes.
- **One unit can span two block issues.** `synmix.c` has three functions in #305 and two in #306, so neither block can be `Matching` alone. Check the reference's unit extents against both blocks before claiming one.
