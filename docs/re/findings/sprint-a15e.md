# Sprint a15e: WorldObj movement and query bindings

Text 0x8014B910-0x8014D754 is split as game/ScriptWorldObjMove.cpp (NonMatching). Its .sdata2 starts at 0x804BA28C, but the original unit shares the pool with game/ScriptWorldObj.cpp (0.0f at 0x804BA280, 1.0f at 0x804BA284), so every float load in this source scores below 100% in isolation. Merge the two sources into one unit to match the pool.

Status: every function in the range compiles; all non-pool instructions match except the items below.

## Open items

- BF00 CDividedGround::GetBSPTree: the original inlines shared_ptr::get into the accessor (24 bytes with a `b` over the next instruction); with dont_inline on, the call to get() stays out of line (16 bytes plus a get symbol).
- findMostVitalPii*InCircleZX/InArcZX: the original std::list iterator is a two-level class. list::begin/end build a temporary from __list_deleter::begin/end and copy it with a generic_iterator copy constructor that calls a separate 12-byte pointer-copy constructor (0x8014D100); operator*, operator++ and operator!= are 4-byte or 0x30 forwarding wrappers (0x8014D110, 0x8014D130, 0x8014D1B0) over the plain generic_iterator bodies (0x8014D120, 0x8014D160, 0x8014D1C0). include/list does not model this, and changing it affects other units.
- The same two functions compute `GetHP() == 0` with `cntlzw` plus `rlwnm rD, rOne, rCnt, 31, 31` (a hoisted `li rOne, 1`) where this compiler emits `cntlzw; srwi 5`. Tried: separate bool, int local, `!`, `0 ==`, `continue` chains.

## Traps

- Binding functions use `NPpu::CEngine* pEngine` directly and cast at each `PopWorldObj`/`GetWorld` call. Declaring a `CPpuEngine*` local changes register allocation (engine lands in r31 instead of r27).
- Nested out-of-line inline functions are emitted in order of first parse: `VEC2LenSq(VEC2Sub(&d, &a, &b))` emits LenSq before Sub, matching the original order.
- `a || (a != nullptr && c)` reproduces a redundant second `beq` that the plain `||` does not.
- Computing the Z difference into a local before the X difference reproduces the original load order in getDirDeg/getDistance helpers.
- The binary keeps separate copies of the same std::list<BWorldObj*> weak functions in ScriptWorldObjMove (0x8014CF80-0x8014D1C0) and ScriptWorldObjQuery (0x8014E3xx). Two NonMatching split objects that define the same mangled name fail to link (multiply-defined), so the copies in this unit keep fn_ names in symbols.txt until the units are merged or the Query copies are renamed.
