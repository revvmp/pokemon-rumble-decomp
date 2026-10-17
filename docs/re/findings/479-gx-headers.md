# F-479-1: shared GX header prerequisites

The GXTev reference at Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` compiles as C under Wii/1.0 with this header set. Its sixteen function sizes and offsets reproduce the issue #479 survey exactly, from `GXSetTevOp` at relative zero (0x94 bytes) through `GXSetNumTevStages` at 0x6a0 (0x24 bytes). This corroborates the GXData offsets, GX enums, register layouts and FIFO writes used by those functions. Full function and artifact matching remain separate gates in the implementation PR.

Headers follow the CC0 Petari `libs/RVL_SDK/include/` dependency closure for GXTev, GXPixel, GXDisplayList, GXFrameBuf and GXLight. Comments were removed; existing project integer types were retained. The umbrella header deliberately includes only this closure. Register definitions preserve the named hardware fields rather than replacing source operations with raw masks. The VI types are required by GXRenderModeObj; PPCWGPipe supplies the FIFO port union. No new compiler settings, original hashes or target manifests are introduced.

This prerequisite is coordinated between issues #479 and #477 under the shared-header rule. No new reusable trap was established. Implementation evidence will be recorded separately.
