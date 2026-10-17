# KPAD layout for the June 2009 SDK

## F-503-4: Target layout and header prerequisite

The KPAD banner identifies `Jun 22 2009 18:32:13 (0x4302_145)`. Its first retained function is `KPADSetPosParam` at `0x80304650`, in #503; the file continues into #504. This header prerequisite changes no source selection or executable split and does not complete either issue.

The imported Petari header describes an older SDK: status size `0x84`, 120 internal samples, and per-channel size `0x1BF8`. The target uses status size `0xB0`, sixteen internal samples of `0x38` bytes, and channel stride `0x578`. Wii/1.0 layout probes reproduce all three target sizes after the changes below.

| Member or region | Offset | Evidence |
| --- | --- | --- |
| `status.ex_status` | `0x60` | Existing core prefix; balance-board results extend the union to `0x50` bytes |
| Position, horizon, distance, acceleration parameters | `0xB0`, `0xB8`, `0xC0`, `0xC8` | Four setters at `0x80304650` through `0x803046B0` |
| `kobj_sample` | `0xF0` | Reset starts at the fourth object's `0x114` address |
| `uniRingBuf[16]` | `0x13C` | Sampling callback at `0x80307F60` uses a `0x38` stride and switches buffers at index sixteen |
| `extraRingBuf`, `extraRingCount` | `0x4BC`, `0x4C0` | Initializer at `0x80307820` divides the supplied count among four channels; sampling and reading select the external array above index fifteen; disconnect invalidates its samples |
| `sec_length` | `0x4C4` | Reset and object-selection arithmetic |
| `readLocked` | `0x4FF` | Extended reader checks, sets, and clears this byte at `0x8030714C`, `0x8030716C`, and `0x80307770` |
| `appSamplingCallback`, `appConnectCallback` | `0x54C`, `0x550` | Initializer stores results of the corresponding WPAD setters; sampling/connect callbacks invoke them |
| `resetReq`, `exResetReq` | `0x554`, `0x555` | Reset clears the former and sets the latter |
| `dpdEnabled` | `0x556` | Initialization sets the requested DPD state; two channel APIs clear/set it; sampling checks it at `0x803082DC` to choose the enabled DPD format |
| `dpdHardwareEnabled`, `dpdIssued` | `0x557`, `0x558` | Control callback stores `WPADIsDpdEnabled` and clears the issued flag; guitar DPD processing checks actual hardware state |
| Four play modes | `0x560` through `0x56C` | Initializer and motion-calculation algorithms |
| `lastDev`, `accGravityUpdateReq` | `0x570`, `0x574` | Connection resets tracking and requests calibration; sampling compares the probed device and refreshes acceleration-gravity scales when requested |

Existing original names remain where available. Names for the added private fields describe these accesses; their spelling is not recovered from debug information. The former unused `_fg_4` byte is the read lock. Tail padding follows the byte at `0x574`; no opaque storage placeholder is introduced. The existing byte named `padding` at `0x55F` controls per-sample versus aggregate button processing in the newer reader; its final source-level name remains to be established with that reconstruction.

The balance-board extension uses the CC0 Skyward Sword declaration: one total `f64`, four current weights, four averaged weights, and two `s32` error results. This gives the observed `0x50` union and `0xB0` public status. Later MotionPlus fields in that reference are absent here: position parameters immediately start at `0xB0`. Existing WPAD train and balance-board raw types join the unified union without changing its `0x38` size. Train and guitar device constants are 16 and 17 in the same reference; target button-repeat checks accept classic or that adjacent pair.

## Validation

A temporary Wii/1.0 compiler probe emits sizes and offsets from the actual header. A before/after build compares all 321 existing source objects: retained code/data, BSS extents, and relocations resolved to section offsets or external names. All are identical. No new reconstructed-byte credit is claimed by this header change.

`python configure.py --no-progress --build-dir build`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, and the explicit frozen-manifest verifier against `build/wpse-v1` all exit 0. The rebuilt game DOL has SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This checks only the game DOL acceptance artifact. Experimental KPAD function comparisons remain diagnostic until the complete unit links from source and the full artifact verifies.

## Provenance

- Existing header and implementation reference: [Petari revision e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe), CC0.
- Balance-board layout and constants: [Skyward Sword KPAD header](https://github.com/zeldaret/ss/blob/787b0b1e9d30e3c1d91ced61350b059ffc5a2411/include/rvl/KPAD/KPAD.h) and [WPAD header](https://github.com/zeldaret/ss/blob/787b0b1e9d30e3c1d91ced61350b059ffc5a2411/include/rvl/WPAD/WPAD.h), CC0. Comments and unrelated later-version fields are omitted.
- Field placement and retained-version selection: original target instructions and compiler layout probes.

## Traps

- **An old layout can make unchanged algorithms look different.** Recover public extension size, sample storage, callbacks, and flag ordering before changing the algorithms. A uniform displacement was disproved: post-ring fields move eight bytes, while play modes move twelve bytes.
- **Read lock and DPD request are separate.** The extended reader locks `0x4FF`; APIs near `0x80307E80` change the DPD request at `0x556`. Follow the format-table use in sampling before assigning a meaning to an enable byte. The observed hardware state is another byte at `0x557`.
