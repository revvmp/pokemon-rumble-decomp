# F-780-1: Shared world-matrix attribute helpers

## Scope

The original header prerequisite extended the canonical `g3d_dcc.h` from merged PR749 with the five static helpers used by the native XSI world-matrix implementation at `0x80267060`–`0x80267214`. It changes no source-unit split, object registration, class storage, or source-coverage figure. Issue #777 retains its source/split/config ownership. Merged PR830 subsequently supplied the complete canonical WorldMtxAttr interface, including all five helpers unchanged. PR826 was closed as redundant, and the source delivery preserves main's header byte-for-byte.

## Evidence

The pinned CC0 Skyward Sword reference is `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `include/nw4r/g3d/g3d_dcc.h` and `src/nw4r/g3d/dcc/g3d_xsi.cpp`. Neither file has a separate file-level license notice. The five retained helper interfaces and four used attribute masks are preserved from that reference.

Native tests at `0x802670B8` and `0x80267128` test bit31. At `0x8026719C`, scale-one sets bit30; `0x802671BC` clears bits30–31 otherwise. The uniform path sets bit28 at `0x802671F8`, while `0x802671F0` clears bits28–31. These instructions establish the used helper semantics and all four mask bits. The class has no data members or virtual functions. No unobserved billboard or translation helpers are introduced.

All 31 candidate functions of the two owned DCC units reproduce their native non-relocation instruction streams under the existing Wii/1.1 NW4R flags with canonical TexSrt, ChrAnmResult, and math interfaces. Those candidate checks do not yet claim linked source reconstruction; complete-object relocation/data and full source-selected verification belong to the source delivery.

## Traps

The imported map spans mark their first/last reference functions as unmapped. These are bounds to validate, not permission to claim adjacent source files. The inspected target starts at the first retained texture helper and ends after the retained world/texture dispatch routines; neighboring Maya and ScnObj files remain outside this header change.
