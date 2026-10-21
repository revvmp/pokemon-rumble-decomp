# Issue 251: WorldObj binding feasibility

## F-251-1: First-owner boundary and native outline context

The survey block begins with the preceding NullObj motion binding at `0x8014AF20`. The first WorldObj binding is original `method_alive` at `0x8014B030`, inside this issue. Native NullObj motion bindings share floating-point literals at `0x804BA268` through exclusive `0x804BA280`. WorldObj direction, scale, collision and movement bindings instead use a distinct pool beginning at `0x804BA280`; the zero and one values are independently duplicated. This establishes a separate source-unit boundary, rather than the survey's shared bind_6 label. The later candidate endpoint at `0x801504C0` remains provisional pending complete exception/data ownership proof; no split was published. The preceding NullObj function was not edited.

The first normal producer gate reconstructs ordinary `method_alive`, `method_getDirDeg`, and `method_setDirDeg` with the complete canonical CPpuEngine and BWorldObj types. It uses the current Wii/1.1 game flags unchanged. Native extents are 80, 76 and 104 bytes; candidates are 100, 116 and 172. Native retains direct calls to GetId, GetRotation, CEngine::PushInt, PushFloat, PopFloat, and PushNull. Current header bodies inline these operations. The actual PopWorldObj target and virtual SetRotation slot agree; source field offsets and parameter semantics do not explain the different outline policy.

The required bounded Wii/1.3 identification control changes only the compiler executable. All three allocated function byte streams and their resolved relocation identities remain identical to Wii/1.1. No compiler flags, pragmas, synthetic declarations, source-local ABI views or register constraints were introduced. The evidence is preserved outside at `251-reference/251-ScriptWorldObj.cpp`, both compiler objects, the canonical command/compile result, `251-native-caller-context.json`, and `251-compiler13-native-comparison.json`. No native function, data, exception or selected-source acceptance is claimed.

## Traps

A compact scalar binding need not be a cheap reconstruction under the current declaration context. This group retains even two-instruction object accessors as calls. Check the first ordinary native producer before reconstructing the large movement/tree continuation; current inline headers expand these bodies under both evidenced game compilers. The remaining dependency is original per-unit declaration/inlining evidence, not an excuse to force an outline policy with a guessed pragma.
