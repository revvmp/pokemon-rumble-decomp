# Typed motor, post-effect and stage tables

## Provenance and scope

The definitions come from the frozen wpse-v1 original main.dol, SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. Existing source declarations and consumers establish the types; no instruction bytes or opaque executable chunks were copied. The table contents are normal C++ aggregate values, string literals and symbolic pointers. Native relocation entries remain intact for the ordinary linker.

| Source | Original range | Typed content |
| --- | --- | --- |
| MotorPatternData.cpp | .rodata 0x803550B8?0x803550DC | Three 12-byte MotorPattern records, indices 0?2 and six named Wii/GC RumbleStep array pointers |
| PostEffectData.cpp | .rodata 0x8035B8E0?0x8035B9C0 | Eight 28-byte CPostEffect::EffectInfo records |
| PostEffectData.cpp | .data 0x803B4058?0x803B4108 | Model and animation names referenced by those effects |
| StageInfo.cpp | .rodata 0x8035B9C0?0x8035BA58 | Four kind message IDs, five StageRankInfo records and eight StageWorldInfo records |
| StageInfo.cpp | .data 0x803B4108?0x803B411E | terminal and grassland names |
| StageInfo.cpp | .sdata 0x804B77E0?0x804B7827 | Rank letters D/C/B/A/S and short world names |

The final four bytes after MotorPatternData, two bytes after StageInfo .data, and one byte after StageInfo .sdata are alignment padding for following objects. They are excluded from the source extents.

## Recovered values

Post effects 0?2 use contrast.brres with contrast__begin, contrast__loop and contrast__end. Effects 3?6 use vibrate.brres with gate-open, gate-close, horizontal and vertical animation names. Effect 7 uses zoom_slow.brres with a null animation pointer. Every record has width 8.0f, height 6.0f, reserved value 0 and scale mode 2, exactly as consumed by CPostEffect::Show.

Stage kind messages are 0x60000, 0x60001, 0x60002 and zero. Rank messages 0x60003?0x60007 correspond to D, C, B, A and S. Worlds 0?7 are terminal, forest, beach, tower, grassland, cave, furnace and battle, with messages 0x60015?0x6001C. These are the existing StageRankInfo and StageWorldInfo types from StageID.h, also used by resource-path construction and stage labels.

## Object verification

Each new translation unit was compiled with the native Wii 1.1 game flags. The three objects contain exactly 681 source-data bytes across their owned sections and 34 ordinary R_PPC_ADDR32 relocations. Resolving these relocations in temporary comparison buffers against the proposed section addresses and existing named motor arrays yields exact byte identity with the frozen original for every section. Object files and linked binaries are not patched. Reproducible evidence is ignored tools/local/session/game-data-ppu/verify_data.py and the adjacent source/object snapshots.

The first three data translation units were integrated as Matching. The native build exited 0 and the frozen executable verifier passed. These are straightforward typed data reconstructions and do not require D-0016 fake-match entries.

## Pii motion tables

PiiMotionData.cpp reconstructs thirteen PiiMotionSet records at 0x80359220 and 124 shared PiiMotionInfo records at 0x80359600. The thirteen sets occupy 988 bytes; native alignment supplies the four bytes before the shared table, making the complete .rodata range 0x80359220?0x8035A1A0. The strings occupy .data 0x803B3090?0x803B4058. Compiler string reuse and source record order reproduce all 147 distinct strings in their original positions, including intentionally repeated Pii_ROOT_die references. Floating-point literals were chosen as short decimal values that round to each original binary32 value and verified through the native compiler.

This data exposes a previously incorrect inferred field type: PiiMotionInfo +0x0C is a string pointer, not a reserved integer. The walk entries for bone types 1, 10 and 11 point to bound_anim_run_ROOT, seal_anim_run_ROOT and slime_anim_run_ROOT at 0x803B30C0, 0x803B31F0 and 0x803B3228. The field is now const char* mRootMotionName. Existing source does not read this field, and the 24-byte record layout remains unchanged. The descriptive member name is inferred from these embedded names; a particular runtime use is not claimed.

The native PiiMotionData object contributes 3,968 .rodata bytes, 4,040 .data bytes and 154 ordinary address relocations. Resolving relocations only in temporary comparison buffers proves exact identity for all 8,008 bytes. The object itself is unmodified and requires the normal linker. The fourth translation unit was integrated as Matching, with native ninja exit 0 and frozen executable verification PASS.

## Motor pattern dependency closure

The six motor-step arrays at .sdata 0x804B7728?0x804B7740 are now also defined in MotorPatternData.cpp. Each contains one existing RumbleStep aggregate: on-duration 50 milliseconds for short and pulse patterns, 100 milliseconds for long patterns, with stop-hard false and every other flag/duration zero. The typed bitfield compiler output matches all 24 bytes. The six gMotorPatterns address relocations now refer entirely to source-defined arrays.

The native compiler normally places these pointer-free constant four-byte objects in .sdata2. A scoped section sconst_type pragma places them in the observed .sdata section and restores the default immediately afterward. This records original section ownership without changing the declared const type or patching any object. Native final integration of this additional .sdata contribution remains pending.
