# F-378-1: NW4R ef particle and particle manager units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #378 begins inside the animation-curve code that #377 starts (`0x802290A0` to `0x80231640`, unreconstructed). This change splits and reconstructs the two units after it.

| Unit | Section | Range | Status |
| --- | --- | --- | --- |
| `nw4r/ef/ef_particle.cpp` | `.text` | `0x80231640–0x802324A4` | Matching, 7/7 functions |
| | `.data` | `0x803C5578–0x803C5590` | `Particle` vtable (0x14) and padding |
| | `.sdata2` | `0x804BA698–0x804BA6E8` | constants |
| `nw4r/ef/ef_particlemanager.cpp` | `.text` | `0x802324B0–0x802347B4` | NonMatching, 15/16 functions at 100% |
| | `.ctors` | `0x80352A3C–0x80352A40` | `__sinit_\ef_particlemanager_cpp` (a bare `blr`) |
| | `.data` | `0x803C5590–0x803C55D8` | `Calc` jump table, `ParticleManager` vtable (0x20) |
| | `.bss` | `0x8042F2A8–0x8042F2D8` | `smDrawMtxPMtoEM` |
| | `.sbss` | `0x804B92C0–0x804B92C4` | `smMtxInvId` |
| | `.sdata2` | `0x804BA6E8–0x804BA730` | constants |

`ef_particlemanager.cpp` continues into #379 (`BeginDraw` `0x80234280`, `EndDraw`, `Draw_ModifyColor`). `ef_resource.cpp` follows at `0x802347B4`.

## Reference

Both sources follow CC0 `Etrainquandt17/Whippy` commit `8ee40c841475db862231f37b8cb64196b613cc17` (the Kirby revision of F-375-2). Differences found:

- `Particle::Draw_GetColor` is absent from the target, so the source does not define it. `Particle::GetAlphaFlick` (`0x80232270`, 0x234 bytes) is out of line.
- `ParticleManager::Closing` (0x8C bytes) does not step `mLastCalced` back; it is the 2006 body from CC0 ogws.
- `ParticleManager::smMtxInv` does not exist in the target. `.bss` `0x8042F2D8` is `Resource::mResource` (0x20 bytes), which `0x802347B4` onward and the resource static initializer reference.
- `ParticleManager::RetireParticle` and `Draw_GetMtxPMtoEM` are inlined into their callers; the linker removes the out-of-line copies.
- `EmitterDrawSetting` gains `mTevTexture[4]` (after `mIndirectTargetStage`) and `mAlphaInput` (after `mColorInput`), 12 bytes in all. `GetAlphaFlick` reads `mAlphaFlickType` at draw-setting offset 0x71, as in the reference.

## Remaining difference

`ParticleManager::Calc` (`0x80232930`) is 0x1558 bytes. The Whippy body is the 2006 one (0x738 bytes) and carries a note that the original is about three times as long; ogws leaves its field-effect cases empty. The target body inlines the field effects: it calls `fmod` six times, `__fpclassifyf`, `memcpy`, `MTX34RotAxisFIdx`, `PSVECCrossProduct`, `PSVECMag`, the animation-curve executors at `0x802290A0`, `0x8022B350`, `0x8022C430`, `0x8022CB80` and `0x80230DA0`, and three helpers at `0x80235CB0`, `0x80235D30` and `0x80235DB0`. objdiff scores the current body at 26%. The unit therefore stays `NonMatching`, and the build links the original object.

## Outside symbols

`ParticleManager::smMtxInvId` (`0x804B92C0`) was named in #737. This change names `Resource::mResource` (`.bss` `0x8042F2D8`).

## Traps

- **A reference can mark its own gaps.** Whippy's `ParticleManager::Calc` is the older body with a TODO saying so. Check reference sources for such notes, and compare function sizes, before assuming a reference matches its revision throughout.
- **An adjacent `.bss` symbol may belong to the next unit.** The reference defines two `ParticleManager` matrices. The target's second `.bss` object after `smDrawMtxPMtoEM` is the 0x20-byte `Resource::mResource`. Check which code references each object before extending a split.
