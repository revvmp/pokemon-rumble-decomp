# Issue 140: minimap and marker reconstruction

## F-140-1: Original inventory and file ownership

The immutable original issue inventory is 22 functions and 8,724 code bytes. Its first nineteen entries belong to CMiniMap, from the factory at 0x800C26D0 through the shared deleter destructor ending at 0x800C4810. The source file begins earlier with the 76-byte marker predicate at 0x800C2680, the final original entry of owned issue #139. Consequently #139 owns the complete CMiniMap split and declaration; the same agent legitimately claimed the unassigned #140 continuation through the helper. Original inventories remain separate. Issue #141 starts CMoneyLifeDisplay and is not needed for this minimap continuation.

The final three #140 functions form the separate CMiniMapMarker unit: constructor 0x800C4810/72, destructor 0x800C4860/92 and Initialize 0x800C48C0/156. Native calls and original deleter RTTI identify the class. F-141-1 independently established this boundary and the eight-aligned root-string section start. The existing complete declaration from F-54-5 is preserved: signed type at zero, CLayout at four and pane pointer at 236, total 240 bytes.

The marker split spans text 0x800C4810–0x800C495C, extab 0x8001D4A8–0x8001D4F0, extabindex 0x80048C00–0x80048C24 and sdata 0x804B6AB8–0x804B6AC0. Its root literal is five bytes; section alignment accounts for the remaining gap. Text ends at the last instruction, leaving following alignment outside. Source and method spellings are descriptive except the original class name.

## F-140-2: Typed native producer and consumer evidence

The marker constructor initializes type zero, constructs its layout and clears the pane pointer. Initialize first acquires the default font map, assigns the requested type, loads the named layout through the canonical shared archive list and supplied background flag, then stores FindPane("root"). All three outside producer bodies have exact native sizes and every resolved instruction byte and target matches; the proof records twelve relocation sites. This preliminary producer comparison does not establish source-selected acceptance by itself.

CMiniMap is a complete 568-byte object: ICalcDraw-derived IMiniMap at zero, CDisplayPhaseCtrl at four (28 bytes), shared CFileData at 32, CLayout at 40, VEC2 map dimensions at 272, CaptureTexture at 280 (44 bytes), CLayout display at 324, weak BWorld at 556 and float height at 564. The display controller points to the display layout. Native factory allocation, all member constructors and destruction independently establish these boundaries; no opaque span is required. Class names are original RTTI; member spellings are inferred.

The primary vtable orders destructor, Calc, inherited CalcMtx, CalcView, Draw, Show and Hide. The large capture-rendering routine 0x800C30D0 is CalcView; Draw at 0x800C3080 draws the display layout only when the phase controller is visible. Outside typed destructor, Show, Hide, Calc and Draw reproduce all 420 native instruction bytes and eleven resolved target sites. The #139 predicate separately reproduces all 76 raw bytes with no relocations: equal pointers compare false, distinct signed marker types sort ascending, equal types use pointer order. These bodies remain outside pending complete unit reconstruction.

## F-140-3: Complete marker source acceptance

The complete marker unit is source-selected Matching. The real split-first build and frozen verification pass, followed by source selection and another fresh link with frozen verification. unitdiff reports all three functions and 320 instruction bytes at 100 percent. The final ELF audit separately checks every original function address, extent, instruction byte and exact MAP owner against the immutable original. All twelve original target sites resolve naturally from the typed source. Retained text spans 332 bytes including native inter-function alignment; extab is 72 bytes, extabindex 36 bytes, and the five-byte root literal plus three linker-alignment bytes preserve the eight-byte sdata extent. All four intervals equal the frozen original. The real Ninja graph selects src/game/CMiniMapMarker.o and contains no original marker fallback dependency. Earlier weak font-map shared-pointer cleanup code emitted by the producer is discarded in favor of the earlier canonical instance.

The per-head source audit is 140-marker-source-audit.py. Acceptance does not close #140: nineteen original minimap functions, 8,404 instruction bytes, remain. The source body, full declarations and compiler options introduce no new shared-header changes.

## Traps

A matching survey boundary can omit a source unit's first function. The minimap factory follows a file-local comparator owned by #139; treating the factory as the start would assign split and header ownership incorrectly. The sole comparator reference is the native sort callback in CalcView. Full first-function ownership therefore remains with #139, while the marker helper starts a separate unit entirely owned by #140.

Scratch compiler inputs and resolved-byte receipts are in C:/MassStorage/GitHub/140-reference, with the immutable issue REST response in 140-original-issue.json. No issue completion is claimed by these preliminary controls.
