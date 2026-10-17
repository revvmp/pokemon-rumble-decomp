# F-285-1: RevoEX NET and NCD compiler and flags

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #285.

The block `0x801770F4`–`0x801778E4` holds two libraries from one network SDK drop. RevoEX NET identifies itself as `<< REX-PPC 2.4.255.0 (RevoEX-2.4) REL 090609111526 >>`. NCD identifies itself as `<< RVL_SDK - NCD \trelease build: Jun  9 2009 11:59:48 (0x4199_60831) >>`. Both banners share the build date of June 9 2009.

Compiler: CodeWarrior GameCube **GC/3.0a5.2**, using `cflags_base` with `-inline auto` replaced by `-inline auto,deferred`, and without `-ipa file`. GC/3.0a3 and GC/3.0a5 emit identical code for all four units, so the exact GC 3.0 revision is not determined by this block. GC/3.0a5.2 was chosen because it already builds `RVL_SDK/wenc/wenc.c` in this project. The community Monster Hunter Tri decompilation independently measured GC/3.0a5.2 for the same NCD and SO build. Wii/1.0 and Wii/1.1 are ruled out: they pad functions to 16 bytes, while this block packs functions at 4-byte starts (`0x801770F4`, `0x801770FC`, `0x801776CC`). They also choose different registers in `NETSHA1Update` and `NCDiLock`.

Deferred inlining is supported by two independent observations.

- **Reverse function emission.** With `-inline deferred`, CodeWarrior emits functions in reverse source order. Each unit's static helper (`NETSHA1ProcessBlock`, `NCDiLock`) is therefore written first, in the usual C style, and appears last in `.text`.
- **NCD `.bss` order.** Without deferred mode, objects are placed in first-use order of the emitted code. `NCDiGetWirelessMacAddress` touches the vectors first, then `ncdMutex` last, which yields vectors at `+0x00` and the mutex at `+0x40`. The target has the mutex at `+0x00`, the reply buffer at `+0x20`, and the vectors at `+0x40`. Deferred mode with `NCDiLock` written first reproduces this exactly, with no dead helper function.

`-ipa file` cancels the reverse emission under these compilers, so it is not used.

Validation: `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0 with the four units `Matching` and linked. objdiff reports 8/8 functions at 100%.

# F-285-2: Unit boundaries, names and data

| Unit | `.text` | Data |
| --- | --- | --- |
| `RevoEX/net/NETVersion.c` | `0x801770F4`–`0x801770FC` | `.data` `0x803B9D88`–`0x803B9DC0` (banner, `0x36` bytes plus padding); `.sdata` `0x804B80E0`–`0x804B80E8` |
| `RevoEX/net/wireless_macaddr.c` | `0x801770FC`–`0x80177100` | none |
| `RevoEX/net/sha1.c` | `0x80177100`–`0x801776CC` | `.sdata2` `0x804BA320`–`0x804BA328` (`0x80` padding byte); `.sbss2` `0x804BBC00`–`0x804BBC08` (eight zero bytes) |
| `RVL_SDK/ncd/ncdsystem.c` | `0x801776CC`–`0x801778E4` | `.data` `0x803B9DC0`–`0x803B9E60`; `.bss` `0x803E9980`–`0x803E99E0`; `.sdata` `0x804B80E8`–`0x804B80F0`; `.sbss` `0x804B90F8`–`0x804B9100` |

The twelve bytes after `0x801778E4` are alignment before CNT at `0x801778F0`, which is compiled with 16-byte function alignment.

`ncdsystem.c` is the `__FILE__` string passed to `OSPanic` at line 1500. `NETVersion.c` and `wireless_macaddr.c` follow the file names in the CC0 [Wii Menu decompilation](https://github.com/koopthekoopa/wii-ipl/tree/main/libs/RevoEX/src/net), which also supplies `NETGetRexPPCVersionPrintable`, `NETRexPPCVersionPrintableString`, `NETGetWirelessMacAddress`, and its callee `NCDiGetWirelessMacAddress`. The public `NETSHA1Init`, `NETSHA1Update` and `NETSHA1GetDigest` names come from the `NETDigest.h` interface in the same repository, and agree with Mario Kart Wii symbol maps. The behavior here matches: initial hash constants, 64-byte blocks, and a big-endian bit count. `sha1.c` is a descriptive file name, because no string names the SHA-1 file. `NETSHA1ProcessBlock`, `NCDiLock`, `ncdInitFlags`, `ncdIpcBuffer`, `ncdMutex`, `ncdReply`, `ncdVectors`, `sPaddingHead` and `sPaddingZero` are descriptive names. `__NCDVersion` follows the RVL_SDK `__<LIB>Version` convention.

`NETGetRexPPCVersionPrintable` has no caller in the executable. Without `#pragma force_active on`, the linker strips it with its string, and `.data` shrinks by `0x40`. The Wii Menu source keeps it alive the same way.

`NETSHA1Context` is `0x60` bytes: five state words at `0x00`, the 64-byte block buffer at `0x14`, the buffered byte count at `0x54`, and a 64-bit block counter split into low (`0x58`) and high (`0x5C`) words. `NETSHA1GetDigest` computes the bit length as `blocks * 512 + count * 8` before padding. It appends `0x80`, then zeros from `sPaddingZero` if fewer than eight bytes remain. An inlined zero-fill loop pads to 56 bytes, and the two bit-length words follow.

NCD's IPC buffer is `0x1B60` bytes taken from the IPC arena, which is 32-byte aligned. `NCDiGetWirelessMacAddress` issues ioctlv 8 on `/dev/net/ncd/manage` with a 32-byte reply vector and a 6-byte output vector. It returns -3 for a null pointer, -5 outside a thread, -8 when the device does not exist, -2 on other IPC failures, and -1 when closing fails.

## Traps

- **Unreferenced RevoEX version getters are dead-stripped.** Trigger: marking `NETVersion.c` `Matching` moved `.bss` down by `0x40` and verification failed at DOL header byte `0x37`. The function has no callers, so the linker removed it with its banner. Keep it with `#pragma force_active on` around the definition. Evidence: F-285-2.
- **`-inline deferred` reverses function order and changes data layout.** Trigger: NCD `.bss` objects came out in the opposite order to the target, and SHA-1 functions matched only in reverse source order. Under GC/3.0 with `-inline auto,deferred` and without `-ipa file`, write helpers first. Evidence: F-285-1.
- **A compiled-out assertion can block inlining.** Trigger: with the register allocation correct, `NETSHA1Update` was inlined into `NETSHA1GetDigest`, but the target keeps three calls. One `((void)0)` assertion statement at the top of `NETSHA1Update` raises its inline cost enough to keep it out of line, with no code change. A local pointer copy also blocked inlining, but it changed register allocation. Evidence: F-285-1.
