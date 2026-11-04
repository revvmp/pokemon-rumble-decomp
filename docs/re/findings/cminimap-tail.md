# CMiniMap final function and emission work

## CalcView instruction reconstruction

The baseline had 19 of 20 exact functions. CalcView retained the target 2,464-byte instruction length but scored 93.74675%, with an oversized frame, extensive stack-slot differences, floating-point scheduling differences, and an incorrect pixel-format argument.

The retained source now uses GX_PF_RGBA6_Z24, matching the target immediate 1. The previous GX_PF_RGB8_Z24 passed 0. One named translation vector is reused for map and marker placement, assigning its scalar components before SetTranslate; constructing separate vectors at each call retained unnecessary temporaries. Named mapWidth and mapHeight intermediates before filling the layout Size reproduce the target floating-point register allocation and scheduling.

The camera and marker projection body is an inline CalcMarkers member. This fixes the stack-object creation order, including the vector insertion machinery. Within it the view matrix precedes the inverse matrix, followed by direction, position, plane and intersection. Its single helper declaration is private and does not change the class layout. The helper and associated declaration/order choices are documented D-0016 source shaping, not a claim that the helper name or original source structure was recovered.

The helper owns the existing four-byte map literal at 0x804B6AB0. Its inferred symbol was renamed to the compiler-emitted helper string name. Passing that literal from CalcView instead introduced a retained nonvolatile register and additional instruction, so that probe was rejected.

## Function emission

Making Show, Hide, Calc, Draw and CalcView inline defers their emission to the virtual-method walk, reproducing the original reverse declaration order. Removing the empty explicit CMiniMap destructor permits the compiler-generated destructor to emit after the sort templates. The native scratch object now matches all 20 functions and their complete order.

Ignored reproducible evidence is tools/local/session/minimap-ppu.py and minimap-ppu/implicitdtor.cpp/.json. Earlier candidates document the progression: reusescalar 97.469154%, membergeometry 98.06493%, and sizenamed0 99.99188% with only the subsequently corrected literal relocation. An initial whole-function inline wrapper and constructor-only Size changes did not solve the frame or floating-point block.

The final native build exited successfully and the frozen DOL verifier passed. CMiniMap is accepted as Matching with all 20 functions, data and exception metadata linked from source. Exact function matches and order alone were not used as acceptance evidence.

## Small-data timing

The first ordered native link still differed in 9,451 bytes because the CalcMarkers map literal preceded the Initialize literals. This consumed eight bytes of original alignment padding and shifted all following small-data references. The .data RTTI and vtables already occupied the correct addresses. Moving the complete inline CalcMarkers definition before Initialize makes its literal emit after the Initialize group and preserves all 20 function matches and their order. Moving only the first declaration did not change string emission. Evidence is minimap-linked2.elf/.dol and minimap-ppu/bodyorder.cpp/.json; this is a definition-order effect specific to these inline literal pools, not a general change to class-template emission rules.

## Final acceptance

After the inline-definition ordering correction, the parent build coordinator confirmed ninja exit 0 and frozen executable verification PASS. No original hashes changed. The final source uses the original native compiler/linker workflow, with no injected executable bytes or post-link patches.
