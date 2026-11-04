# GX render mode definitions

Six original GXRenderModeObj globals occupy0x803D6300 through0x803D6468: GXNtsc480IntDf, GXNtsc480Prog, GXMpal480IntDf, GXPal528IntDf, GXEurgb60Hz480IntDf and GXEurgb60Hz480Prog. Existing imported SDK types and names establish their60-byte layout. The reconstructed C source uses named TV/XFB mode enums, dimensions, origins, field-rendering and antialias flags, twelve sample-coordinate pairs and seven vertical-filter coefficients. Both interior and trailing structure padding are zero in every original record.

The six ordinary initialized globals compile to360bytes without functions or relocations. Their exact .data split is linked from source. Configure, ninja and frozen main.dol verification all pass; no original hashes changed. No fake function, explicit padding array, copied instruction or unit test was introduced.
