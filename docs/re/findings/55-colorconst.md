# F-55-1: CColorConst unit and native color interfaces

The first eight functions of original issue #55 continue the CColorBtnTagProc source file whose first retained function is in #54. That owner reconstructs and accepts the complete tag processor separately. The remaining seventeen functions start CColorConst.cpp at0x8006CE30 and end with its static initializer at0x8006DDB0–0x8006DDD8. The following0x8006DDE0 function constructs the distinct combo UI class. The anonymous-namespace class name and source spelling are independently embedded in its RTTI and shared-pointer deleter type names.

## Proven class contract

CColorConst derives from IColorConst. Its native vtable at0x8039AD00 has six entries, in order: text-color getter, vertex-color getter, text-color application to a pane, its named-layout overload, vertex-color application to a pane, and its named-layout overload. The operation names are inferred from behavior; the class names are recovered. The getters return nw4r::ut::Color by value. The object is468 bytes: a four-byte vptr followed by116 ordinary four-byte Color objects, arranged as29 palettes of four vertex colors. The text-color getter selects palette slots0 and2; initialization duplicates each text start/end color into its adjacent vertex pair.

A current outside candidate independently reproduces five complete original functions and1048 instruction bytes: GetTextColor28, ApplyVertexColor(Pane*)492, GetVertexColor28, ApplyVertexColor(CLayout*)108 and ApplyTextColor(Pane*)392. Every actual relocation operand is resolved to the frozen native target, including SDK RTTI and save/restore helpers; no opcode or register field is masked. The raw source/object hashes, complete per-function comparisons and targets are retained under `55-reference/55-five-function-proof.json`. These are bounded function proofs, not complete unit layout or source-selection evidence.

## Remaining work and interfaces

Initialize at0x8006CE30 creates the anonymous concrete instance, loads the colorconst layout, reads text-box and picture colors, and installs five tag-processor color pairs. Its native stack-local CLayout requires the complete class contract owned by #329. The load call passes an archive-name pointer, layout name, shared FontMap owner and boolean flag; the sole observed caller passes1. The existing minimal CLayout header is not expanded with guessed storage. The #329 owner is reconstructing its complete layout and shared map support independently.

The named text-color overload first tries the pane name with suffix `_c`, then the original name. Its two temporary strings are destroyed before deciding whether to fall back. The current basic_string construction/concatenation definitions do not yet reproduce this function's328-byte native stream. A named pair of strings prolongs their lifetime and regresses the code; expression temporaries recover the observed cleanup order but still leave a constructor-register difference. Independent generic reserve/concatenation candidates remain outside and unaccepted.

The outside pointer-only CLayout declaration and extra class-retention factory exist solely for diagnostic compilation. Neither is a proposed source contract or accepted implementation. No split, compiler flag, canonical shared header, object classification or source-selected credit is changed by this finding.

## Traps

An explicit null-pane return before the SDK DynamicCast is present in both native pane application functions. Omitting it produces the right color operations but drops16 bytes. After the cast, retaining the positive typed-pane block followed by a final false return recovers the original branch ordering; an early negative return changes the instruction stream. All five bounded proofs retain the SDK class interfaces and ordinary virtual dispatch.
