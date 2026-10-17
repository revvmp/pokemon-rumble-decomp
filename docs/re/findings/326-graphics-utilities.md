# F-326-1: graphics initialization and model-resource utilities

## Scope and ownership

Issue #326 owns thirty surveyed functions, 6,292 code bytes, from `0x801C6DB0` through the last destructor ending at `0x801C8700`. The survey title does not identify an original class or source filename. The opening six functions manage GX and VI state, rather than a G3D extension class. Descriptive candidate names are used only in external diagnostics.

The #325 owner independently established four preceding units ending at `0x801C6DAC`: primitive helpers, AnmObjChrGroup, AnmObjChrOverride and AnmObjChrRootScale. The following BUI constructor starts at `0x801C8700` and belongs to #327. No neighboring source, header, split or configuration is taken by this investigation.

Whether the graphics functions and the following model-resource factory were originally one translation unit remains unresolved. Contiguous surveyed addresses alone do not establish that boundary. No split or source selection has been published.

## Native inventory

| Address | Code bytes | Observed responsibility |
| --- | ---: | --- |
| `801C6DB0` | 476 | Initialize VI and G3D, allocate FIFO and two framebuffers, initialize GX and render mode |
| `801C6F90` | 188 | Configure and copy the current display framebuffer, then wait for GX completion |
| `801C7050` | 100 | Present the framebuffer, unblack the first frame and swap buffers |
| `801C70C0` | 812 | Reset G3D and explicit GX raster, pixel, TEV and indirect state |
| `801C73F0` | 64 | Set and cache GX pixel and depth formats |
| `801C7430` | 264 | Convert clear color between RGB8 and RGBA6 framebuffer formats |
| `801C7540` | 4 | Tail-call G3dObj::Destroy |
| `801C7550` | 144 | Construct factory options and delegate to the resource factory |
| `801C75E0` | 1036 | Load and bind resource files, select a model and construct ScnMdlSimple or ScnMdl |
| `801C79F0` | 8 | Construct a ResMdl wrapper |
| `801C7A00` | 44 | Bind a resource-file wrapper |
| `801C7A30` | 200 | Reset a shared pointer with a ScnMdl and function-pointer deleter |
| `801C7B00` | 200 | Reset a shared pointer with a ScnMdlSimple and function-pointer deleter |
| `801C7BD0` | 20 | Initialize vector storage, count and capacity |
| `801C7BF0` | 1668 | Resize the resource shared-pointer vector, including exception cleanup |
| `801C8280` | 16 | Index an eight-byte vector element |
| `801C8290` | 12 | Assign a ResMdl wrapper |
| `801C82A0` | 20 | Test a ResMdl wrapper for validity |
| `801C82C0` | 16 | Initialize an empty two-word shared pointer |
| `801C82D0` | 52 | Swap vector capacity, storage and count |
| `801C8310` | 276 | Set eight G3dObj options |
| `801C8430` | 76 | Query the ScnMdlSimple function-pointer deleter by type |
| `801C8480` | 20 | Invoke the ScnMdlSimple control block's function-pointer deleter |
| `801C84A0` | 76 | Query the ScnMdl function-pointer deleter by type |
| `801C84F0` | 20 | Invoke the ScnMdl control block's function-pointer deleter |
| `801C8510` | 136 | Assign a shared pointer with retain and release |
| `801C85A0` | 108 | Destroy an owning custom-deleter guard |
| `801C8610` | 108 | Destroy the second owning custom-deleter guard specialization |
| `801C8680` | 64 | Destroy the ScnMdl control block |
| `801C86C0` | 64 | Destroy the ScnMdlSimple control block |

## Data and type evidence

The graphics routines reference a FIFO allocation pointer at `0x804B9160`, GXFifoObj pointer at `0x804B9164`, two framebuffer pointers at `0x804B9168` and `0x804B916C`, and the current framebuffer at `0x804B9170`. No reference to `0x804B9174` was found; its storage must not be invented as a field merely to occupy the gap before BUI state.

The first-frame flag at `0x804B82F8` initially contains one. The cached GXPixelFmt and GXZFmt16 words at `0x804B82FC` and `0x804B8300` initially contain minus one. The initializer changes both to zero after GXSetPixelFmt. Constants at `0x804BA480`, `0x804BA484` and `0x804BA488` are respectively float zero, float one and the eight-byte `0x4330000000000000` unsigned-to-floating conversion constant.

The factory's five-word options initializer starts at `.rodata:0x80367020` and contains `0, 0, 0, 1, 0`. Its later fallback-value call takes the address `0x804B8308`, whose first word is one. The exact original type of that fallback and the neighboring zero words remain unresolved.

The initialized-data cluster starts with `vector length error` at `0x803C00D8`, followed by the exception filter at `0x803C00EC`. RTTI names at `0x803C0138` and `0x803C01B8` identify the original specializations `std::tr1::detail::shared_ptr_deleter<nw4r::g3d::ScnMdlSimple, void (*)(nw4r::g3d::G3dObj *)>` and the corresponding ScnMdl specialization. Their vtables are at `0x803C0124` and `0x803C01A4`; RTTI records are at `0x804B8310` and `0x804B8318`. The following BUI exception cluster begins at `0x803C0220`.

The factory uses the existing resource shared-pointer destructor at `0x80075D50`, rather than owning another destructor implementation. The vector and typed shared-pointer interfaces are coordinated with #53 and #327; this issue has not introduced competing standard-library declarations.

## Diagnostic compilation

An independently written external graphics candidate uses the actual BController Ninja compiler command: Wii/1.1, `cflags_game`, exceptions and RTTI enabled, `-ipa file`, and the normal sixteen-byte function alignment. All includes resolve from the #326 worktree's canonical headers. Decompiler output was used only to inspect behavior.

The following five candidates reproduce every native instruction after independently pairing every call and SDA relocation: framebuffer copy, framebuffer presentation, GX state reset, pixel-format caching and clear-color conversion. They total 1,428 code bytes and ninety-five relocations. The reset uses the canonical nw4r::ut::Color integer constructor for the white fog color and explicit calls for each texture coordinate and TEV stage. The conversion uses promoted integer color components and shifts without redundant masks; assignment to the byte fields provides the required truncation.

The initializer has the native 476-byte size and all thirty-seven relocation targets agree, but seventeen register-allocation words differ. It is not accepted. These are diagnostic results only: no complete source object, exception-section audit, source-selected DOL, objdiff result or reconstruction credit is claimed.

Repeating the identical source and flags with Wii/1.3 produces the same five exact diagnostic bodies and the same seventeen initializer differences. This does not justify changing the project compiler or flags. The external default-render-mode callee at `0x801B66A0` copies a complete sixty-byte GXRenderModeObj into its first-argument storage; its original return-by-value versus explicit output-parameter declaration remains unproven.

The two 200-byte reset routines construct twenty-byte function-pointer control blocks: common state occupies the first twelve bytes, the owned pointer is at twelve and the deleter at sixteen. Their exception guard has an owned pointer at zero and a reference to the stack deleter at four. The guard destructors at `0x801C85A0` and `0x801C8610` call through that reference only while the owned pointer is nonnull. This is native evidence for an ordinary custom-deleter reset responsibility; the original private guard class name has not been recovered.

A declaration-only extraction from the previously pinned MSL smart-pointer reference, blob `e57966d2bb1331d2b2b21137c9bc389e90c6c573`, confirms the public `template<class Y, class D> void shared_ptr<T>::reset(Y* p, D d)` interface. It also declares move_ptr custom-deleter families. These names do not prove the native private guard's identity, layout or implementation. Restricted implementation algorithms were not imported or used. The filtered extraction is reproducible with the external `326-smart-declarations.py` and its JSON output.

Reproduction files are outside the repository at `C:/MassStorage/GitHub/326-graphics-probe.cpp`, `326-compile-graphics.py`, `326-audit-graphics.py` and `326-graphics-proof.json`. The compile driver must exit zero before the audit is run. Native inventory reading aids are in `326-initial-native.json` and `326-tail-native.json`.

## Traps

- The decompiler omits some register arguments when its inferred prototype is wrong. Native `0x802D07B0` is GXSetZMode and consumes three arguments, including GX_LEQUAL; a one-argument interpretation loses eight instructions bytes and calls the wrong API. Canonical symbols and actual native call preparation establish the interface.
- The default GX umbrella header does not declare GXInit, GXSetClipMode or GXSetNumIndStages. Include their existing GXInit, GXTransform and GXBump headers directly; do not invent local declarations or replace the canonical header tree.
- A size match is insufficient: the initializer candidate still differs. The diagnostic audit records unpaired or incorrect relocations separately from other instruction differences, and no matching claim follows from masking relocation words. The earlier eleven-word color-conversion mismatch was resolved using the ordinary promoted integer expression without redundant bit masks.
- The native framebuffer size multiplies the aligned sixteen-bit width by height before doubling it. A mathematically equivalent differently associated expression changes instructions. Preserve the demonstrated arithmetic types and order without adding artificial padding or instructions.
