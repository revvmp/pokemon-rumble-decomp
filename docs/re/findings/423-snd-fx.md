# F-423-1: NW4R snd FxBase, FxChorus, FxDelay and FxReverbStd units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #423 owns `.text` `0x8027B400–0x8027C044`. Its first three functions close `snd_ExternalSoundPlayer.cpp` ([F-421-1](421-snd-biquad-channel-envgenerator.md)). The rest form four effect units. The last, `snd_FxReverbStd.cpp`, ends with the first three functions of #424's range. This completes #423.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_FxBase.cpp` | `.text` | `0x8027B5E0–0x8027B5E4` | `FxBase::OnChangeOutputMode` |
| `nw4r/snd/snd_FxChorus.cpp` | `.text` | `0x8027B5F0–0x8027BA08` | constructor, `GetRequiredMemSize`, `AssignWorkBuffer`, `ReleaseWorkBuffer`, `StartUp`, `Shutdown`, `SetParam`, `UpdateBuffer` |
| | `.data` | `0x803C6248–0x803C6270` | vtable and padding |
| | `.sdata2` | `0x804BAE68–0x804BAE88` | parameter defaults and clamp limits |
| `nw4r/snd/snd_FxDelay.cpp` | `.text` | `0x8027BA10–0x8027BE68` | the same eight functions |
| | `.data` | `0x803C6270–0x803C6298` | vtable and padding |
| | `.sdata2` | `0x804BAE88–0x804BAEA0` | parameter defaults and clamp limits |
| `nw4r/snd/snd_FxReverbStd.cpp` | `.text` | `0x8027BE70–0x8027C3B8` | the same eight functions |
| | `.data` | `0x803C6298–0x803C62C0` | vtable and padding |
| | `.sdata2` | `0x804BAEA0–0x804BAEB8` | parameter defaults and clamp limits |

`snd_FxReverbHi.cpp` and the `Dpl2` effect units are absent from this executable. The next function, `CreateImpl` at `0x8027C3C0`, starts `snd_InstancePool.cpp` in #424's range.

All four units are `Matching`: objdiff reports 100% code for 1, 8, 8, and 8 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision and reconstruction

No public source reproduces this revision. ogws and the 4.3 System Menu decompilation (koopthekoopa/wii-ipl, CC0, commit `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`) use the older `AXFXChorus`, `AXFXDelay`, and `AXFXReverbStd` interfaces, and zeldaret/ss has only stubs except `snd_FxReverbStdDpl2.cpp`. The units were reconstructed from the target code, following the zeldaret/ss `FxReverbStdDpl2` pattern (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`). The parameter type names (`detail::FxChorusParam`, `FxDelayParam`, `FxReverbStdParam`) and `SetParam` sizes match the symbol map of another title, riptl/wii-symbols `SUPE01`: `FxChorus::SetParam` is 420 bytes and `FxReverbStd::SetParam` 664 bytes there and here.

- Each effect drives the expanded AXFX interface (`AXFXChorusExp`, `AXFXDelayExp`, `AXFXReverbStdExp`) and owns `mIsActive` at 0xC, `AxfxImpl` at 0x10, its parameter block at 0x18, and the AXFX state after it (0x2C for chorus and delay, 0x40 for the standard reverb).
- `StartUp` refuses when the frame heap is smaller than `RoundUp(GetMemSize + HEAP_SIZE_MIN, 32)`, where `HEAP_SIZE_MIN` is `MEM_FRM_HEAP_MIN_SIZE + 32` (0x68). It hooks the AXFX allocator, initializes, restores, and calls `GetMemSize` once more before setting `mIsActive`.
- `SetParam` clamps each field into the AXFX state and clears both bus pointers and the send gain. `FxDelay` and `FxReverbStd` call `Settings` with the allocator hooked when the maximum delay changes and `SettingsUpdate` otherwise; `FxChorus` always calls `SettingsUpdate`.
- `UpdateBuffer` does nothing while inactive or in DPL2 output mode.

The parameter field names are descriptive and follow the AXFX state members each value is clamped into. Defaults: chorus delay 10, depth 0.5, rate 1, feedback 0, output gain 1; delay 160, feedback 0.4, output gain 1, maximum delay 160, low-pass 1; standard reverb as in zeldaret/ss `FxReverbStdParam`.

`FxBase::OnChangeOutputMode` is the only non-inline `FxBase` function. It is its own unit, `snd_FxBase.cpp`, linked before `snd_FxChorus.cpp`; `snd_FxBase.h` now declares it out of line, as zeldaret/ss does. The weak effect destructors and the inline `FxBase` virtuals are emitted first by game code: the vtables point at destructors at `0x801E7580` (`FxReverbStd`), `0x801E7680` (`FxChorus`), and `0x801E7700` (`FxDelay`), each of which calls `Shutdown` and `ReleaseWorkBuffer` through the vtable.

## Headers

New: `snd_FxChorus.h`, `snd_FxDelay.h`, and `snd_FxReverbStd.h`. `snd_AxfxImpl.h` gains `HEAP_SIZE_MIN` and `GetHeapTotalSize` (zero for a null heap). `revolution/mem/heapCommon.h` gains the SDK inline `MEMGetHeapTotalSize`, and `revolution/mem/frameHeap.h` gains `MEM_FRM_HEAP_MIN_SIZE`; both are confirmed by the 0x87 rounding constant and the `heapEnd - heap` subtraction in every `StartUp`.

## Outside symbols

Renamed: the three game-side effect destructors above. Naming them lets the linker keep those strong definitions and discard the weak copies from the effect units. Their splits and sizes are unchanged.

## Traps

- **A weak inline virtual can be a real unit.** The 4-byte `blr` at `0x8027B5E0`, shared by all three effect vtables, looked like a weak `FxBase` inline emitted by `FxChorus`. Compiled that way, it lands after `FxChorus::UpdateBuffer`. It is `FxBase::OnChangeOutputMode`, defined in `snd_FxBase.cpp`, which links before `snd_FxChorus.cpp`.
- **Weak destructors resolve to earlier game copies.** The effect vtables reference destructors in game code at `0x801E7xxx`, not in the effect units. With the game copies renamed to the mangled destructor names, the link matches. Linking with the `dtor_` names was not tried; expect the units' own weak copies to be kept.
