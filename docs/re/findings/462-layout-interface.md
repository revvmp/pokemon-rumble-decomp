# F-462-1: Shared Layout interface for layout utilities

Target `wpse-v1`, frozen `orig/wpse-v1/main.dol`. This header prerequisite supports #462 without taking the source, split, or configuration ownership of #457, where `Layout::Layout` starts at `0x802A6310`.

## Provenance and layout

The interface follows CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe` and CC0 xbret/xenoblade `ea25691b2029394ab777bda7060139e517a13cba`. Neither reused header carries a separate file license. Existing canonical NW4R math, list, rectangle, and SDK allocator headers are reused.

Native constructor `0x802A6310–0x802A6348` initializes the vptr at zero, the animation list at 4, root pane at `0x10`, group container at `0x14`, and layout size at `0x18`. A standalone constructor compiled with the project's Wii/1.1 NW4R flags reproduces all 14 instruction words except the three relocation-bearing words, whose targets are the zero float and Layout vtable. The compiler confirms `sizeof(Layout) == 32`.

The vtable at `0x803C6EF8` contains two initial zero words followed by fourteen entries: destructor, Build, three CreateAnimTransform overloads, BindAnimation, UnbindAnimation, UnbindAllAnimation, BindAnimationAuto, SetAnimationEnable, CalculateMtx, Draw, Animate, and SetTagProcessor. `BindAnimationAuto` at `0x802A69D0` returns false on absent root/resource and true after binding; it occupies slot `0x28`. The overload at `0x802A68C0` returns the created animation pointer. These observed return values correct inconsistent reference declarations.

`GetLayoutRect` at `0x802A6D00` always constructs the rectangle from half the width and height. The old reference's trailing origin byte is absent from both this routine and the native constructor and is omitted from the header. The animation list uses the reference's four-byte node offset; native iterator accesses subtract four bytes to recover the transform.

`Size` contains two floats, in width/height order. The sixteen-byte resource file header follows the reference; `TestFileHeader` at `0x802B0C90` reads its signature word and byte-order halfword at zero and four.

## Validation

The header branch adds no source unit and claims no reconstructed code. `python configure.py`, `.venv/Scripts/ninja`, and the explicit frozen-manifest verifier exit zero. Before rebasing, all 342 existing compiled source objects were unchanged against the pre-header baseline. After the #514 integration, 341 still match that baseline; `bta_dm_api.o` changes with the merged source completion on main. The standalone layout-constructor probe is diagnostic only and is not linked or committed.

## Traps

- Reference Layout revisions differ in both vtable shape and instance size. Copying the older interface puts CalculateMtx in the wrong virtual slot, while copying its origin member makes the instance four bytes too large. Check the target constructor and complete vtable before defining the shared class.
- An issue's broad source hint does not establish its file ownership. The Layout constructor is in #457, while #456 begins with Pane code; the prerequisite does not modify either issue's source or split.
