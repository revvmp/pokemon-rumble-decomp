# F-477-1: GX framebuffer and lighting reconstruction

Issue #477 owns two complete code ranges in wpse-v1/main.dol: GXFrameBuf at 0x802CD440-0x802CDEC0 and GXLight at 0x802CDEC0-0x802CE630. Function ordering, register operations and light-object offsets corroborate pinned Petari CC0 commit e0ab141914921726a595573dc48d3f13d0922efe, src/RVL_SDK/gx/GXFrameBuf.c and GXLight.c. Neither range crosses #476 or #478. The target does not retain GXGetNumXfbLines or GXInitSpecularDirHA; it additionally retains attenuation setters and position/direction getters.

## Recovered symbols

Names have high confidence from reference correspondence and target accesses. DOL file offsets are runtime addresses minus 0x8004FD40. Function extents remain the original dtk sizes.

| Address | Name |
| --- | --- |
| `0x802CD440` | `GXSetDispCopySrc` |
| `0x802CD480` | `GXSetTexCopySrc` |
| `0x802CD4C0` | `GXSetDispCopyDst` |
| `0x802CD4F0` | `GXSetTexCopyDst` |
| `0x802CD600` | `GXSetDispCopyFrame2Field` |
| `0x802CD620` | `GXSetCopyClamp` |
| `0x802CD650` | `GXGetYScaleFactor` |
| `0x802CD880` | `GXSetDispCopyYScale` |
| `0x802CD950` | `GXSetCopyClear` |
| `0x802CD9D0` | `GXSetCopyFilter` |
| `0x802CDBC0` | `GXSetDispCopyGamma` |
| `0x802CDBE0` | `GXCopyDisp` |
| `0x802CDD20` | `GXCopyTex` |
| `0x802CDE80` | `GXClearBoundingBox` |
| `0x802CDEC0` | `GXInitLightAttn` |
| `0x802CDEE0` | `GXInitLightAttnA` |
| `0x802CDEF0` | `GXInitLightAttnK` |
| `0x802CDF00` | `GXInitLightSpot` |
| `0x802CE0A0` | `GXInitLightDistAttn` |
| `0x802CE170` | `GXInitLightPos` |
| `0x802CE180` | `GXGetLightPos` |
| `0x802CE1A0` | `GXInitLightDir` |
| `0x802CE1C0` | `GXGetLightDir` |
| `0x802CE1F0` | `GXInitSpecularDir` |
| `0x802CE300` | `GXInitLightColor` |
| `0x802CE310` | `GXLoadLightObjImm` |
| `0x802CE390` | `GXSetChanAmbColor` |
| `0x802CE470` | `GXSetChanMatColor` |
| `0x802CE550` | `GXSetNumChans` |
| `0x802CE580` | `GXSetChanCtrl` |

## Layout and ABI

The 64-byte light object contains unused words at 0x00-0x0B, packed RGBA at 0x0C, attenuation coefficients a[3] at 0x10, distance coefficients k[3] at 0x1C, position at 0x28 and negated direction at 0x34. Vectors contain three floats. GXGetLightDir negates internal values back to caller coordinates. Shared GX headers are introduced separately by #557 with #479.

Floating constants occupy .sdata2 0x804BB300-0x804BB310 for GXFrameBuf and 0x804BB310-0x804BB344 for GXLight, followed by four padding bytes. The split retains padding through 0x804BB348. Render-mode data ownership has not been established; existing data remains original fallback and no render-mode source coverage is claimed.

## Initial comparison and remaining work

Wii/1.0 with cflags_sdk produces exact target instructions apart from external call-symbol identities. The normal project report scores all 30 functions at 100%. A direct object comparison scores 11/14 GXFrameBuf functions and 14/16 GXLight functions at 100%; five functions differ solely in R_PPC_REL24 target names. This diagnostic evidence does not establish an accepted source link.

| Target address | Source name | Affected routine |
| --- | --- | --- |
| 0x802CE760 | __GetImageTileCount | GXSetTexCopyDst |
| 0x8034033C | __cvt_fp2unsigned | GXGetYScaleFactor, GXSetDispCopyYScale |
| 0x8034CEE4 | cos | GXInitLightSpot |
| 0x8034D798 | sqrt | GXInitSpecularDir |

These external symbols lie outside #477; original labels remain pending owner coordination. Both units remain NonMatching and contribute no linked source progress. Resolve external names and remaining data ownership, then require full source linking and frozen-manifest verification before marking Matching.

Baseline and split-only builds reproduced the frozen executable. Final validation is recorded in the pull request. No new reusable tooling trap was discovered.

## F-477-2: Complete source link

The continuation names the three previously unresolved callees __GetImageTileCount (0x802CE760), cos (0x8034CEE4) and sqrt (0x8034D798), preserving their extents and types. Their identities follow the reference-corresponding call sites and instruction streams established in F-477-1. The runtime helper __cvt_fp2unsigned was already named on main. No callee source implementation is required to replace these caller units; those outside-issue implementations retain their existing ownership and reconstruction status.

GXFrameBuf and GXLight now compile and link from source as Matching. Direct objdiff comparison against the original split objects gives 14/14 and 16/16 functions at 100%, respectively: 2,608 and 1,820 function bytes, 4,428 total. Associated .sdata2 contains 16 and 52 bytes, all matching. GXLight's final constant at 0x804BB340 is one four-byte float (-1.0E+18f), not eight bytes. The original inferred symbol included four following zero-padding bytes; correcting its size to four and ending the .sdata2 split at 0x804BB344 leaves that alignment to the following automatic unit.

The retained functions do not reference the render-mode objects elsewhere in .data. Their ownership was not established by this issue and they are not part of this completion or counted as reconstructed here. All thirty functions in #477 and their required constants are source-built, with no original-object fallback for either owned unit.

The first source-linked build failed the checksum and exact verifier at DOL file offset 0xB4B3. The linker map placed the next fallback function (original 0x802CE630) at 0x802CE624 because GXLight's compiled .text ends at the last instruction rather than including twelve trailing padding bytes. The maintainer approved preserving the observed neighboring function boundary with align:16 on that entry, matching the existing F-479-2 remedy. Its name, size, type, code and owning split remain unchanged. This restores the original next-function address without adding bytes to source.

After the boundary and constant corrections, configure.py and ninja exit 0, including the build SHA-1 check. The exact frozen-manifest verifier exits 0 with SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. Final clean and latest-main validation commands are recorded in the pull request. No package or whole-game completion is claimed.

## Traps

A source object may omit both code and constant trailing padding even when every function compares at 100%. Here source linking moved the next fallback function from 0x802CE630 to 0x802CE624; preserving its evidenced 16-byte alignment restores the boundary. GXLight's last inferred float symbol also included four alignment bytes; inspect section bytes and constant accesses before treating padding as owned data. See F-477-2 above for the failed build and exact accepted correction.
