# F-326-1: graphics initialization and model-resource utilities

## Scope and ownership

Issue #326 owns thirty surveyed functions, 6,292 code bytes, from `0x801C6DB0` through the last destructor ending at `0x801C8700`. The survey title does not identify an original class or source filename. The opening six functions manage GX and VI state, rather than a G3D extension class. Descriptive candidate names are used only in external diagnostics.

The #325 owner independently established four preceding units ending at `0x801C6DAC`: primitive helpers, AnmObjChrGroup, AnmObjChrOverride and AnmObjChrRootScale. The following BUI constructor starts at `0x801C8700` and belongs to #327. No neighboring source, header, split or configuration is taken by this investigation.

An independently written six-function graphics candidate now reproduces a complete natural object extent ending at `0x801C7538`, with closed private static state and complete native allocated-section proof. This supports a separate graphics unit before the model-resource factory's destroy helper at `0x801C7540`; the original filename remains unrecovered. Contiguous surveyed addresses alone did not establish that boundary. No split or source selection has been published.

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

All six graphics candidates now reproduce every native instruction after independently pairing every call and SDA relocation. They total 1,904 code bytes and 132 relocations. The complete relocated text extent, including native alignment gaps, is exactly `0x788` bytes at `0x801C6DB0`, ending at `0x801C7538`. The reset uses the canonical nw4r::ut::Color integer constructor for the white fog color and explicit calls for each texture coordinate and TEV stage. The conversion uses promoted integer color components and shifts without redundant masks; assignment to the byte fields provides the required truncation.

The initializer's earlier seventeen register-allocation differences were resolved by separating selection of the requested/default mode from the active mode retrieved after SetRenderModeObj. The active mode reuses the input pointer variable. Every native word then agrees without compiler or flag changes. All six eight-byte exception records and their twelve-byte indexes are also exact. The complete twelve-byte `.sdata` and sixteen-byte `.sdata2` payloads agree, and all twenty bytes of `.sbss` have the native field offsets and extent. These remain diagnostic results: no source-selected DOL, objdiff result or reconstruction credit is claimed.

Repeating the final identical source and flags with Wii/1.3 produces the same complete graphics code and metadata proof. Earlier Wii/1.3 probes also preserved the then-unresolved seventeen initializer differences. This does not justify changing the established Wii/1.1 game compiler or flags. The external default-render-mode callee at `0x801B66A0` copies a complete sixty-byte GXRenderModeObj into its first-argument storage. An explicit output-parameter declaration is ABI-compatible and reproduces the native call; a return-by-value assignment candidate instead emits an extra 152 code bytes. The original declaration and name remain unrecovered.

The two 200-byte reset routines construct twenty-byte function-pointer control blocks: common state occupies the first twelve bytes, the owned pointer is at twelve and the deleter at sixteen. Their exception guard has an owned pointer at zero and a reference to the stack deleter at four. The guard destructors at `0x801C85A0` and `0x801C8610` call through that reference only while the owned pointer is nonnull. This is native evidence for an ordinary custom-deleter reset responsibility; the original private guard class name has not been recovered.

A declaration-only extraction from the previously pinned MSL smart-pointer reference, blob `e57966d2bb1331d2b2b21137c9bc389e90c6c573`, confirms the public `template<class Y, class D> void shared_ptr<T>::reset(Y* p, D d)` interface. It also declares move_ptr custom-deleter families. These names do not prove the native private guard's identity, layout or implementation. Restricted implementation algorithms were not imported or used. The filtered extraction is reproducible with the external `326-smart-declarations.py` and its JSON output.

Reproduction files are outside the repository at `C:/MassStorage/GitHub/326-graphics-probe.cpp`, `326-compile-graphics.py`, `326-audit-graphics.py` and `326-graphics-proof.json`. The compile driver must exit zero before the audit is run. Native inventory reading aids are in `326-initial-native.json` and `326-tail-native.json`.

## Traps

- The decompiler omits some register arguments when its inferred prototype is wrong. Native `0x802D07B0` is GXSetZMode and consumes three arguments, including GX_LEQUAL; a one-argument interpretation loses eight instructions bytes and calls the wrong API. Canonical symbols and actual native call preparation establish the interface.
- The default GX umbrella header does not declare GXInit, GXSetClipMode or GXSetNumIndStages. Include their existing GXInit, GXTransform and GXBump headers directly; do not invent local declarations or replace the canonical header tree.
- A size match is insufficient. The diagnostic audit records unpaired or incorrect relocations separately from other instruction differences, and no matching claim follows from masking relocation words. The earlier eleven-word color-conversion mismatch was resolved using the ordinary promoted integer expression without redundant bit masks; the initializer required proper selected/active mode lifetimes.
- The native framebuffer size multiplies the aligned sixteen-bit width by height before doubling it. A mathematically equivalent differently associated expression changes instructions. Preserve the demonstrated arithmetic types and order without adding artificial padding or instructions.
- A temporary nw4r::ut::Color(0) argument reproduces initializer instructions but adds a destructor cleanup to its exception record. The ordinary zero-initialized GXColor local reproduces both native code and the native eight-byte exception record. Metadata comparison caught this before source selection; matching text alone would have missed it.

# F-326-2: custom-deleter reset and compressed-pair argument policy

## Native consumer and independently written source

The custom-deleter diagnostic reproduces the two reset routines at `0x801C7A30` and `0x801C7B00`, two ownership-guard destructors, two get_deleter methods, two dispose methods, two control-block destructors and the shared-pointer assignment at `0x801C8510`. These eleven owned functions total 1,072 code bytes. Every instruction matches after resolving all twenty text relocations to their actual native callees, vtables or function-pointer type information.

The ordinary reset constructs a temporary shared_ptr and swaps it with the destination. Its custom-deleter constructor protects the supplied pointer with a stack ownership guard while allocating the control block, then relinquishes the guard. The temporary shared_ptr destructor releases the old control block and has the native nonthrowing exception profile. The control block stores its owned pointer and function-pointer deleter in the nonempty compressed-pair policy. Its original public template name and deleter type are independently established by RTTI; the diagnostic guard name ResetGuard is descriptive and is not claimed as an original private class name.

The compiled exception records independently pair the ScnMdl guard destructor with `0x801C8610` and the ScnMdlSimple guard with `0x801C85A0`. All 200 owned exception bytes, including the two guard-destructor relocations, and all nine twelve-byte exception-index entries reproduce the native records. The initialized-data proof covers 248 bytes at `0x803C0124` through `0x803C021C` and sixteen RTTI bytes at `0x804B8310`; all fourteen embedded pointers resolve exactly. The native four-byte gap before the next BUI data cluster is not reconstructed as artificial storage.

The outside common-control destructor at `0x80055BB0` and shared_ptr destructor at `0x800770A0` also reproduce 196 code bytes, their four text relocations, 48 exception bytes and both indexes. They are controls for the generic interface and receive no #326 source credit. Extra weak definitions and diagnostic fixture wrappers are not treated as owned retained source. A full source-selected ELF and DOL comparison remains required.

## Declaration-only evidence and header ownership

The previously pinned smart-pointer declaration blob `e57966d2bb1331d2b2b21137c9bc389e90c6c573` declares `shared_ptr_deleter(element_type*, D&)`. Passing D by value instead initially caused six instruction differences per reset. The recovered reference interface reduces this to one swapped load/store pair.

The same declaration-only extraction confirms that shared_ptr's copy assignment has no nonthrowing specification. The independent ordinary copy-and-swap assignment reproduces the native 136 code bytes, two calls, forty exception bytes and index. Adding a nonthrowing specification instead produced 148 code bytes and another exception scope; that candidate is rejected. The shared_ptr destructor's separate nonthrowing specification is retained.

The compressed-pair declaration blob `c58d840db435e5ea772a13f07ecacc568e341f5c` at KooShnoo/pokemon-rumble revision `11c112f7548b26ab2cecfcf52e201b8d3b76120f` declares first_param and second_param through Metrowerks::call_traits and accepts those aliases in its two-argument constructor. Its companion declaration blob `f3c956bc7ff410613a30dae8aa95236741bc8cd6` declares a default size threshold of `sizeof(T) <= sizeof(void*)`, value parameters for that small case, const references for the large case, pointer values for pointer specializations and preserved references for reference specializations. Only declarations and aliases were extracted; no restricted implementation algorithms were imported or used.

The #327 header owner independently corrected the generic utility. Its exact candidate was copied unchanged to `C:/MassStorage/GitHub/326-owner-compressed-pair.h`, SHA-256 `a352c30a5f8cf31ff7698c8ad1c82f4cae403ee08dfc62884d93eebe9be1da09`. The final candidate separates the size selector from pointer/reference specialization to avoid ambiguous large-reference selection. Recompiling the reset consumers with that exact candidate removes the last two instruction differences in each routine and preserves all other native proof. This consumer exercises pointer/pointer construction and nonempty storage. It does not independently prove every call_traits selection or storage policy.

An additional independent vector consumer using the same exact header confirms four-byte `compressed_pair<size_t, allocator<T>>` storage and a complete twelve-byte vector extent, exercising the second-empty policy. Its destructor remains 196 bytes against the native 204-byte routine and is explicitly unaccepted. The call-traits correction does not resolve that separate vector algorithm or exception-lifetime question.

Initializing the base's capacity pair to zero and naturally allocating an array of vectors emits the real constructor callback. That callback reproduces all twenty native instruction bytes at `0x801C7BD0`, with no relocations or artificial deleting-flag parameter. This confirms zero initialization of storage, count and capacity through the authentic three-layer class hierarchy. It does not establish the destructor or resize body. `326-audit-vector-layout.py` records the constructor and physical-layout proof in the same layout JSON.

The existing declaration-only vector reference further exposes __vec_deleter's default constructor, explicit constructor taking allocator_param, destructor, `void clear()` and `size_type grow_by(size_type n)`. Its allocator_param aliases the capacity pair's second_param. No get_allocator helper is declared on that base in this revision. An invented allocator-return helper is therefore not introduced merely to obtain the missing stack byte; the native destructor and #326 resize operation still need faithful container-source proof.

## Reproduction and limits

The actual Wii/1.1 BController command drives `C:/MassStorage/GitHub/326-compile-reset.py`, which compiles the independently authored `326-reset-probe.cpp`. `326-audit-reset.py` asserts every resolved instruction, exception record, index and owned initialized-data byte. Outputs are `326-reset-native-proof.json`, `326-reset-metadata-proof.json` and `326-reset-owner-audit.log`. Both commands exit zero with the exact owner candidate above. The vector layout check is recorded in `326-vector-owner-layout-proof.json`; `326-compile-vector.py` compiles its fixture.

The generic typed shared_ptr and ownership guard remain external diagnostics under coordination with #327. No competing canonical smart-pointer or compressed-pair header has been introduced. The factory's precise result-pointee interface, private guard identity, complete translation-unit boundary, source emission order and full source-selected link still require acceptance. Issue #326 remains incomplete.

## Traps

- A plausible generic utility can preserve layout yet change source emission. The const-reference compressed-pair constructor preserved twenty-byte control-block storage but scheduled the deleter load after the pointer store. The recovered call_traits interface and an independently written owner correction reproduce the native order. Do not repair this with volatile, padding or instruction-shaped source.
- The custom-deleter constructor takes D by reference even though the public reset accepts it by value. Conflating those signatures changes native scheduling. Declarations establish the distinction; the native consumer verifies it.
- Diagnostic wrappers emit duplicate reset bodies and extra weak definitions. Count the eleven owned native functions once and retain the outside common/shared destructor proof solely as a control. Full source-selection and linked-artifact gates remain necessary before any reconstruction credit.

# F-326-3: canonical resource-wrapper consumers

Three ordinary diagnostic consumers of the existing complete NW4R headers reproduce the native ResFile no-argument Bind wrapper at `0x801C7A00` (44 bytes), ResMdl assignment at `0x801C8290` (twelve bytes) and ResMdl validity test at `0x801C82A0` (twenty bytes). All 76 instruction bytes and the Bind call relocation to `0x8024C080` agree exactly. No competing resource class or header is needed. Reproduction files are `C:/MassStorage/GitHub/326-resource-probe.cpp`, `326-compile-resource.py`, `326-audit-resource.py` and `326-resource-native-proof.json`.

The ResMdl constructor at `0x801C79F0` remains separate from this proof. A placement-new fixture adds a null-storage guard and emits sixteen bytes; a one-word class returned by value instead emits a four-byte return. Neither establishes the native eight-byte constructor callback. Source-context emission must be proven with the actual factory, rather than replacing that constructor with a hand-written initializer helper. No source-selected reconstruction credit is claimed for these diagnostics.
