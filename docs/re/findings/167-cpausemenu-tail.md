# CPauseMenu remaining native instruction tails

## 2026-11-04 isolated register probes

Baseline is 35/37 exact functions. LeaveStage is 98.72596%, with mode/area extraction and hub-index bitfield insertion ordering differences. TakeScreenshot is 99.83075%, with register-only differences.

Declaring both render-mode dimensions as `const u16` fixes the initial work pointer, render-mode pointer, and dimension register rotation. This raises TakeScreenshot to 99.95769%. Ordinary locals, references, aggregate containers, volatile pointed-to render-mode views, restrict, and most scalar qualifier controls do not achieve this. A local inline EncodeCapture helper with a const GXTexFmt local fixes the first encoder's remaining format register. The best complete candidate, tools/local/session/pause-tail/encodehelper3.cpp, scores 99.971794%; only four operands remain, exchanging r26/r27 for the JPEG buffer pointer and texture format in the second encoder call. All other 35 functions retain their existing exact code.

The RA trace for that candidate is tools/local/session/pause-tail/ra-best.txt. In its allocation input, the second JPEG pointer load is r57 and the format return value is r81; descending coloring assigns format r27 before pointer r26, whereas the original uses pointer r27 and format r26. A void-pointer view or pointer/integer identity cast changes this pair to the target but rotates earlier dimensions, picture-pane pointer, and first-format registers. A struct with const dimension members restores the initial dimension pair in that variant, leaving the picture and first-format registers wrong. None of these incomplete candidates is retained in main.

Rejected controls include const/nonconst references for the encoder locals, local declarations moved before assignment, reference-output dimension helpers, dependent template encoder wrappers, nested scalar-argument encoder wrappers, alternative pointer types, namespace-level format helpers, direct GXGetTexObjFmt calls, scoped optimizer pragmas on the encoder helper, and per-caller propagation/lifetime/scalarization flags. The helper pragmas do not affect the inlined caller's output; caller-wide flags regress other instructions.

LeaveStage controls reproduce earlier negative findings: zero-then-area/mode assignment changes extraction order but retains the wrong insertion order; const and narrowed scalar locals are folded; source-local output initializers, derived packed-index classes, and explicit packed bitfield constructors either preserve the original mismatch or introduce extra copies/frame effects. Mask-and-shift extraction using the original signed nine-bit rotate shape canonicalizes back to the baseline. No tracked CPauseMenu source/header edit has been applied.

## Traps

- A const scalar local can survive register-allocation preparation where the ordinary named local was coalesced away; the u16 render dimensions are a verified example. This is not a universal const-pointer rule: pointer constness alone did not change the relevant registers.
- A pointer identity cast can fix a late register pair while rotating earlier unrelated lifetimes. Compare every instruction in the function, not only the intended local region.

## TakeScreenshot solved

The retained source now reaches 100% for TakeScreenshot, with 36/37 unit functions exact. The final arrangement is two small inline encoder helpers: EncodeCapture has a const GXTexFmt local; EncodeCaptureToBuffer has a `void* const` JPEG destination local and leaves GetFormat directly in the encoder argument expression. The two dimensions remain const u16. The destination type is the encoder's actual void-pointer interface, without a pointer/integer roundtrip or const_cast.

The decisive control removed only the second helper's named format local. Keeping it gives that inline variable coloring priority above earlier picture/first-format lifetimes; leaving the second format as a call temporary preserves the target priority while the named void-pointer destination gets the required register. The isolated exact candidate is tools/local/session/pause-tail/screenshotexact.cpp/.o; the applied source was formatted and its new helper parameters renamed descriptively, then recompiled with the same 100% result. No shared header was changed.

This helper/const-local arrangement is a D-0016 register-allocation fake following the documented ordinary-variable and type controls. It retains the actual encoder calls, arguments, temporary shared-owner lifetimes, and operations. Main's CPauseMenu remains NonMatching until LeaveStage and the complete source-selected link are accepted. Parent broad validation of the retained TakeScreenshot gain is pending.

## LeaveStage solved

A source-local HubArea enum covering the entire signed eight-bit field range (-128 through 127) preserves the area extraction as a distinct typed live value. GetHubIndex declares const HubArea area first, const int mode second, then constructs the original MapIndex aggregate in mode/area/zero/zero order. This gives the target area-first extraction and mode-first insertion without additional operations or frame storage. Replacing the enum local with int canonicalizes back to the previous 98.72596% mismatch. An enum on mode alone also fails; the area enum is the decisive control.

The retained explicit bounds cover every value representable in the original signed eight-bit area field. This is a documented D-0016 type adaptation, not a claim that these enum names were recovered. The final formatted main source directly compiles to 37/37 exact functions in original order. No shared header changes were needed. Full source-linked validation is pending.

## Native link residual and layout correction

The first full source-selected link differed in only 25 bytes: three instructions referenced reordered constant initializer records, and the remaining bytes were those records in .rodata. All code addresses, exception tables, .data, and unrelated sections otherwise matched. Original .rodata order is the remote-menu item pair (0x10 bytes), remote-menu UIMenuParam (0x20), then the ordinary menu's base UIMenuParam (0x20). The producer emitted the base parameter first because ShowMenu's in-class body is parsed earlier.

Moving the whole ShowMenu body late fixed the constants but changed template instantiation order and some functions, so that candidate was rejected. The retained fix declares a real const sBaseMenuParam early and defines it after the other methods. ShowMenu copies that named constant instead of its compiler-generated equivalent. A real five-byte `sMenuMotionName` array preserves the original first .sdata string while both parameter records refer to it. A const char array would instead enter .sdata2, so the retained char storage matches the original writable string section. No runtime code mutates it.

The resulting isolated candidate staticmenu0.o keeps all 37 exact functions and their original order. Its .data, .sdata, .sdata2, and extab contents are byte-identical to the previous source object; only the three genuine .rodata objects change to the proven target order. This named storage arrangement is included in the D-0016 emission adaptation. Full linked validation of the corrected layout is pending.

## Accepted result

The parent agent reran a fresh full native build with CPauseMenu configured Matching after the .rodata fix: ninja exited 0 and the frozen full-DOL verifier passed. The unit is accepted at 37/37 functions and complete source-linked code, exception tables, and data. No expected hashes were changed.
