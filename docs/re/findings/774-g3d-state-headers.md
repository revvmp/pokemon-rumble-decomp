# F-774-1: G3D state and draw interface prerequisite

The source-file issue #774 owns `g3d_state.cpp` at `0x8025B6A0`–`0x8025ECB0`. This prerequisite adds the missing MSL algorithms without taking any neighboring source splits. Main imported the canonical light and GPU headers during this work, so both add/add conflicts were resolved by retaining main's headers unchanged. The light implementation is tracked separately by #793.

## Provenance

The two new headers derive from zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`: the MSL C++ `algorithm` and `iterator` headers under `src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/`. The pinned root `LICENSE` is CC0-1.0. Each reused file was inspected and has no separate file-level notice. Comments were removed. The MSL headers retain the original find/move/swap and iterator traits/distance facilities needed by this unit. The combined prerequisite also adds the original sorting templates consumed by #778, plus its bounded draw interfaces; [F-778-1](778-g3d-draw-headers.md) records their native evidence. Main's canonical workmem header is reused unchanged.

No existing class declaration, source header or object registration changes.

## Native evidence

A scratch compilation of the complete state source under the existing Wii/1.1 NW4R profile matches all 42 native function sizes and 13,644 function bytes, including 445 independently decoded relocation sites. The EnvironmentMapping, EnvironmentSpecularMapping, LightState::SetLightSetting, LightState::LoadLightSet and static initializer accesses validate the light layouts consumed by this source. LightObj is `0x44` bytes, with flags at zero and GXLightObj at four; LightSetting is `0x10`, with u16 counts at zero/two and three array pointers at four/eight/twelve; LightSetData is `0xC`, with eight light indices and an ambient index at eight; AmbLightObj is four bytes. The state initializer constructs 128 LightObj entries, and the containing LightState has native size `0x2A2C`.

The canonical GPU inline helpers reproduce every retained instruction of the state resource loaders. Actual linked source acceptance belongs to the subsequent #774 source pull request; scratch identity is not counted as reconstruction progress here.

## Validation

Before and after the original four-header proposal, `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exited 0 on base `ad5aee5c`. All 408 existing source objects retained identical SHA-256 hashes. The final proposal retains main's light/GPU/workmem headers and adds two MSL headers, the bounded draw public header and draw-mode enum. The shared draw prerequisites were folded into the still-unmerged header PR by owner coordination. The merge gate repeats the full build and verifier from a fresh output directory after the add/add resolution.

## Traps

- The pinned MSL find implementation uses `while (first != last && *first != value)`. A loop with an internal early return produced a smaller, different LoadLightSet; using the original facility matches the native `0x42C` function without extra compiler flags.
- A fresh custom output directory linked successfully, but Ninja's CHECK read the fixed `build/wpse-v1/main.dol` path in the frozen SHA list and failed because that default output did not exist. The baseline was rerun through the default build path and passed. No hashes or outputs were copied to work around the check.
