# F-420-1: NW4R snd block 5 is covered by BasicSound

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #420 owns `.text` `0x80278380–0x80279244`, which holds 25 inferred functions. All of them lie inside `nw4r/snd/snd_BasicSound.cpp` (`.text` `0x80277BC0–0x8027930C`). That unit was reconstructed and linked as `Matching` for #419 in pull request #692; see [F-419-2](419-snd-bank-basicplayer.md#f-419-2-nw4r-snd-basicsound-unit).

| Range | Functions |
| --- | --- |
| `0x80278380–0x80278950` | `BasicSound::Update`, `UpdateMoveValue` |
| `0x80278950–0x80278E90` | `UpdateParam`, `Shutdown` |
| `0x80278E90–0x80278F10` | the eight attach and detach functions for player heap, sound player, sound actor, and external sound player |
| `0x80278F10–0x80279040` | `GetVoiceOutCount`, `SetPlayerPriority`, `OnUpdatePlayerPriority`, `SetInitialVolume`, `SetVolume` |
| `0x80279040–0x80279244` | `SetRemoteFilter`, `SetPanMode`, `SetPanCurve`, `SetAmbientInfo`, `GetAmbientPriority`, `IsAttachedGeneralHandle`, `IsAttachedTempGeneralHandle` |

The issue lists 25 functions; the source has 24. dtk had split the `blr` at `0x80279204` into its own function. It is the unreachable epilogue of `GetAmbientPriority`, after that function's tail call. `symbols.txt` now records `GetAmbientPriority` as 0x38 bytes and has no `fn_80279204`.

Evidence: objdiff reports 100% code for all 36 functions of `snd_BasicSound.cpp`, and the source-linked `main.dol` passes `tools/verify/verify.py`. On `main` at the time of this finding, `python configure.py`, `.venv/Scripts/ninja`, and verification exit 0.

The unit continues past this issue's range to `0x8027930C`. That tail is `DetachGeneralHandle`, `DetachTempGeneralHandle`, `SetId`, the weak destructor, `GetRuntimeTypeInfo`, and the static initialiser, at the start of #421's estimated range. #421's own work therefore starts at `0x80279310`.

## Traps

None beyond F-419-2.
