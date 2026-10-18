# Issue 282: NPpu script engine, second block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. The block `0x80170930`–`0x801738A4` belongs to `game/NPpuVM.cpp`, whose split, object line and header come from #281 ([F-281-1](281-nppu-vm.md#f-281-1-the-first-nppu-unit-ends-at-0x80174d64)). Compiler and flags are the game settings of [F-735-1](735-game-compiler.md).

## F-282-1: the interpreter class is `NPpu::CEngine`

The `CPpuEngine` RTTI base list at `0x803A9760` names three bases: `std::tr1::enable_shared_from_this<CPpuEngine>` at offset 0x68, `ICalc` at 0x64 and `NPpu::CEngine` at offset 0 (type-info name at `0x804B7160`). The `CPpuEngine` constructor `0x800FE5F0` calls the constructor at `0x801704C0` on `this` first, and its exception table destroys the same base with `0x80170510`. The class #281 called `CVM` is therefore `NPpu::CEngine`, 0x64 bytes, with no vtable. The source, header, `symbols.txt` and [symbols](../SYMBOLS.md#game-nppu-engine-282) use the original name.

## F-282-2: identities of the 27 functions

`0x80170930` is `CEnvironment::AddRef`, already matched by #281. Opcodes are the top byte of each code word; the 78-entry jump table at `0x803B82E0` maps them to the cases of `Execute`.

| Address | Function | Evidence |
| --- | --- | --- |
| `0x80170940`, `0x80170960` | `SetThis(const CValue&)`, `GetThis(CValue&) const` | Copy to and from member 0x48; called by `ScriptBinding_GetThisObject` and `ScriptBinding_SendToThis` |
| `0x80170980` | `PopArray(int*)` | Pops a count through `ToInt`, then returns the payload of the next popped value |
| `0x80170A20`, `0x80170A60` | `IsIntArg`, `IsStringArg` | Type test of the top of the stack |
| `0x80170AA0`, `0x80170AD0` | `GetArg(int, CValue&) const`, `GetArgInt(int) const` | Read `mStack[mSP - index - 1]`; 32 and 5 callers in the bindings |
| `0x80170B50`, `0x80170BD0` | `PushFrame(int)`, `PopFrame()` | Frames hold the stack pointer, base index and saved base; `PopFrame` drops the handlers of the frame and restores the return address |
| `0x80170BB0`, `0x80170BC0` | `OnLineEnd`, `OnLineBegin` | Empty inline hooks that `Execute` calls around opcode 1 and before native calls |
| `0x80170C70`, `0x80170CE0` | `Run()`, `Reset()` | State machine over idle (1), suspended (2), stopped (3) and error (4) |
| `0x80170D20` | `Call(CValue*, int, int, const CValue*)` | Pushes the arguments and a return address, runs `Execute(1)` and pops the result when it suspends |
| `0x80170FD0` | `PushReturn()` | Stores the program counter twice before incrementing |
| `0x80171010` | `Interrupt(const CValue&)` | `ScriptBinding_Interrupt`; `Execute` throws the value on the next resume |
| `0x80171030` | `GetVariable(int, int)` | Base -1 selects the environment globals; other negative bases fall off the end without a return |
| `0x80171080`, `0x801713F0` | `UnaryOp(int)`, `CValue::SetFunction(int)` | Opcodes 3, 4 and 0x11; the setter is the inline copy emitted after its first user |
| `0x80171400` | `BinaryOp(int)` | Opcodes 5 to 9 with reference arithmetic and float shortcuts for 0 and ±1 |
| `0x80171CE0` | `BitwiseOp(int)` | Opcodes 0x14 to 0x18 |
| `0x80172170` | `CompareOp(int)` | Opcodes 0xB to 0x10, 0x12 and 0x13; jump tables `0x803B8298` (int) and `0x803B82BC` (float) |
| `0x80172980` | `Execute(int)` | The interpreter loop |
| `0x80173780`, `0x80173790` | `CValue::SetString(u32)`, `SetWString(u32)` | Inline copies emitted after `Execute` |
| `0x801737A0` | `Unwind()` | Pops frames to the innermost handler, resumes it and pushes 0 |

`Execute` calls inline functions whose weak copies an earlier file keeps. They are named in `symbols.txt` with the mangled names the source produces: `CValue::operator=` `0x801100D0`, `IsString` `0x801100A0`, `IsWString` `0x80110070`, `SetInt` `0x800C98C0`, `SetFloat` `0x801328F0`, and `CEngine::Push` `0x801319F0`, `Pop` `0x801323D0`, `Fetch` `0x80132380`, `PushInt` `0x801319B0`, `PushFloat` `0x801328B0`, `PopInt` `0x80133500`, `PopFloat` `0x801323A0`, `PopReference` `0x801334D0` and `Drop` `0x8014FB00`. `Pop` and `GetArg` write through a reference: this compiler returns an 8-byte trivially copyable `CValue` in `r3:r4`, but the originals take `this` in `r3` and the destination in `r4` or `r5`. An int `CValue` is built by `SetInt`, not by a constructor.

Method and opcode names are descriptive; confidence is high for the stack, frame and handler operations and medium for the opcode names.

## F-282-3: reconstruction state

objdiff for the 26 new functions:

| Function | Match |
| --- | --- |
| all except the four below | 100% |
| `Execute` | 99.78% |
| `CompareOp` | 97.94% |
| `BitwiseOp` | 97.23% |
| `BinaryOp` | 93.79% |

Remaining differences:
- **`Execute`.** Opcode 0x36 calls `GetArgInt(0)` and compares the result with zero, but both branches go to the loop head. Every source form tried removes the compare, including empty branches, `continue`, `goto`, dead stores and self-assignments.
- **Operator functions.** After the string conversion, the original loads each operand type twice: once for the immediate compare and once into a register that stays live through the reference, float and `ToInt` tests. The source loads it once and reloads it later. The operands are popped into a two-element array. Separate locals, copies, pointer helpers and by-value helpers all scored lower.

Two shapes were confirmed while matching:
- `CValue(x).ToInt()` on a temporary inlines `IsInt` with a different branch pattern than a call on a named local. `GetArgInt` and `PopArray` need the temporary form.
- `ParseInt` and `ParseWInt` reach 100% only when they read the `CCode` string tables directly. `CEngine` is a friend of `CCode` and `CEnvironment`, and the unused `GetString` accessors are removed. This fixes the two #281 near misses.

`Pop`, the conversion helper and the push helpers are inline. `SetFunction`, `SetString`, `SetWString`, `OnLineBegin` and `OnLineEnd` are emitted in the original positions.

The unit stays `NonMatching`. Its remaining #281 near misses and the #283 functions are still open, and `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original object.

## Method

`tools/re/unitdiff.py` with Wii/1.1 and `cflags_game`. The generated assembly under `build/wpse-v1/asm/` was used for target reading, together with a Python reader of the DOL for the RTTI base list. No decompiler output was used.

## Traps

- **Small structs come back in registers.** A member function returning `CValue` by value compiled to an `r3:r4` return. The originals pass a destination pointer, so write such accessors with an output reference.
- **Inline `bool` helpers have two branch shapes.** On a named local the inlined `||` test starts with `li 0`; on a temporary, reference or standalone object it starts with `li 1`. Match the object form before changing register-allocation details.
- **Local declaration order decides stack slots.** The first declared `CValue` gets the higher stack address. `Execute` needed operands declared before they are popped, in reverse pop order.
- **Wrong declaration orders cost several percent.** In `Execute`, permuting `code`, `op`, `returnCount` and `modifier`, and the switch-case locals, moved the match by up to 1%.
