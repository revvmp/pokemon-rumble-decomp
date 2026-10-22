# Sprint r1: CodeWarrior register allocation and scheduling rules

Compiler: Wii/1.1 `mwcceppc -O4,p` with the game unit flags. Rules were derived in `-S`/objdump sandboxes on minimal reproductions and confirmed on the real units named.

## Rules

1. **One variable, one register, when a single definition reaches every use.** CodeWarrior splits a local into separate live ranges (webs) when each branch fully redefines it, and each web is coloured on its own. To make two branches share a register, give the variable one definition that reaches both branches (initialise it before the `if`, then use compound assignment in each branch). RenderUtilities `NormalizeSignedAngle`: `f32 adjusted = value;` before the `if`, `adjusted += 360.0f;` / `adjusted -= 360.0f;` inside, moved the second branch's result from f0 to f2 and flipped the unit.
2. **`x + c` and `x += c` emit different operand orders.** `adjusted = value + 360.0f` emits `fadds fD, fConst, fValue`; `adjusted = value; adjusted += 360.0f` emits `fadds fD, fValue, fConst`.
3. **`bCOND skip; b end` comes from the last operand of an `||` early return.** `if (a == 0) return; if (b < n) { ... }` emits `bge end`. `if (a == 0 || b >= n) { return; }` emits `beq end; blt body; b end`. CEffect `FreeEffectGroupID` went from 98.0% to 100%.
