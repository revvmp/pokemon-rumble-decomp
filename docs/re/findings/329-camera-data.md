# Pokemon portrait camera parameter data

## Source and schema

The frozen wpse-v1 executable contains gPiiTexCameraParams at virtual addresses 0x8035A1A0?0x8035B8E0, corresponding to DOL file offset 0x3562A0 and 0x1740 bytes. CPiiTex::UpdateCamera indexes the table by the species identifier and reads three signed integer fields. The existing PiiTexCameraParams schema names these mDistance, mHeight, and mAngle. There are 496 consecutive 12-byte records, beginning with the zero entry.

The reconstruction decodes each original record as three big-endian signed 32-bit integers and writes decimal aggregate initializers in src/game/PiiTexCameraParams.cpp. The type and extern declaration moved from CPiiTex.h into the focused include/game/PiiTexCameraParams.h. CPiiTex.h includes that header; the record layout and caller access remain identical. This is typed game data, with no executable bytes or runtime binary dependency. No meaning beyond the existing proven field names is assigned to individual rows.

## Verification

Direct compilation with the configured Wii/1.1 game flags exits zero. The object has one defined data symbol, gPiiTexCameraParams, covering exactly 5952 bytes of .rodata; it contains no relocations. Its entire .rodata section equals the corresponding frozen-original bytes. Both have SHA-256 307f81abe8f330e6c67fc22ec1a72ff4493ec7934cf8f569cf41e7a01f2fa8c7. The diagnostic original slice and compiled object are in ignored tools/local/session/329-camera-data. The original slice is not a build input.

The requested split is game/PiiTexCameraParams.cpp, .rodata start 0x8035A1A0 end 0x8035B8E0. Root integrated the Matching source split; ninja exited zero and frozen-manifest verification passed. No fake match, unit test, or raw-byte initializer was needed.
