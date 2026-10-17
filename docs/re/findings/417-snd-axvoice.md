# F-417-1: NW4R snd AxVoice unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. `snd_AxVoice.cpp` starts with `AxVoice::AxVoice` at `0x80273650`, the last function of #416 ([F-416-1](416-snd-axmanager.md)), covers all 26 functions of #417 (`0x802736A0–0x80275874`), and continues through the first eight functions of #418's estimated range, ending with `AxVoiceParamBlock::SetVoiceRmtIIRCoefs` at `0x80276280`. This completes #416 and #417.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80273650–0x80276404` | 35 functions, `AxVoice` constructor through `AxVoiceParamBlock::SetVoiceRmtIIRCoefs` |
| `.sdata2` | `0x804BAD98–0x804BADC0` | the float and double constants of `SetSrcType`, `SetSrc`, and `SetVe` |

The next function, `0x80276410`, is `AxVoiceManager::GetInstance` (it constructs a static object with three link lists), which starts `snd_AxVoiceManager.cpp` in #418. The unit has no `.data` or `.bss`.

`snd_AxVoice` is `Matching`: objdiff reports 100% for all 35 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision

The target matches the zeldaret/ss revision of the file (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) function for function, and differs from ogws (CC0, commit `27fa94a593096eed7442efc4e70981fd52025b9c`) as follows:

- Voices store an `nw4r::snd::SampleFormat` (`IsPlayFinished` compares the format with 3, 2, and 1 for DSP ADPCM, PCM8, and PCM16) instead of an AX format; `SetAddr` converts it with the new `GetAxFormatFromSampleFormat` (0, 25, 10).
- `SetMix` calls the new `IsNeedNextUpdate`, and the inline delta helper returns 0 when the two volumes are equal.
- `SetSrc` clamps the initial ratio to `[0, 65535]`, and `SetVe` scales by 32767 and clamps both volumes to `[0, 0xFFFF]`.
- `SetBiquad` applies one of `AxManager`'s biquad filter callbacks, with the new `AxVoiceParamBlock::SetVoiceBiquad` and `SetVoiceBiquadCoefs`.
- `AxVoiceParamBlock::Sync` clamps the volume envelope to 0xFFFF before testing for a zero delta.

The source follows ogws for layout and naming and ss for these behaviors. `MixParam` sits at 0x1E (the `Setup` memset starts there), not at 0x20 as ss's comment says. Functions with no out-of-line copy in the target are inlined into their callers and removed by the linker: `GetCurrentPlayingDspAddress`, `GetLoopEndDspAddress`, `GetDspAddressBySample`, `GetAxFormatFromSampleFormat`, the `AxVoiceParamBlock` constructor, `IsRmtIirEnable`, `Clear`, and every `AxVoiceParamBlock::SetVoice*` setter except `SetVoiceMix`, `SetVoiceSrcType`, `SetVoiceRmtMix`, and `SetVoiceRmtIIRCoefs`.

## Headers

`include/nw4r/snd/snd_AxVoice.h` is rewritten for this revision in the ogws style: `SampleFormat` replaces `AxVoice::Format`, and it declares `SetBiquad`, `GetAxFormatFromSampleFormat`, `IsNeedNextUpdate`, `SetVoiceBiquad`, and `SetVoiceBiquadCoefs`. The ogws inline helpers `CalcAxvpbDelta` and `CalcMixVolume` are gone from the header; the delta helper is local to the source file, as in ss. `SampleFormat` moved from `snd_FxBase.h` to `snd_Types.h`, and `Voice::GetFormat` now returns it.

`snd_AxVoiceManager.h`, `snd_Util.h`, and `snd_adpcm.h` come from ogws at the commit above, stripped with `tools/vendor/vendor_strip.py`. `snd_Util.h` makes Wii/1.1 warn `illegal constant expression` for the float-to-int `VOLUME_MIN` and `VOLUME_MAX` initializers; this unit does not use them, and the owner of `snd_Util.cpp` should check their values.

`include/revolution/ax.h` gains `AX_PB_OFF`, the ogws ADPCM frame macros, and the `AXMixerCtrlFlags`, `AXRmtMixerCtrlFlags`, and `AXPBSyncFlags` enums with ogws names and values. `src/RVL_SDK/ax/AXVPB.c` had private copies of the sync enum and of `AX_VOICE_STOP`, `AX_PB_LPF_ON`, and `AX_PB_BIQUAD_ON`; they are removed in favor of the header, and `AXVPB.c` still matches (12/12 functions).

## Outside symbols

Renamed in `symbols.txt` so the unit links: `AxVoiceManager::GetInstance` `0x80276410`, `AxVoiceManager::ReserveForFreeAxVoice` `0x80276C30` (the only call in `VoiceCallback`, after clearing the parameter block), `Util::GetRemoteFilterCoefs` `0x80290FE0` (both calls in `SetRemoteFilter`), and `DecodeDspAdpcm` `0x80292C68` (the per-nibble call in `CalcOffsetAdpcmParam`). Splits and sizes of these symbols are unchanged.

## Traps

- **A private SDK enum blocks a shared header addition.** Adding the AX sync flags to `revolution/ax.h` broke `AXVPB.c` with `illegal name overloading`, because the file declared its own copy. Search `src/` for a symbol before adding it to a shared header, and remove the private copy in the same change.
