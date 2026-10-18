# Script camera adjustment (#235)

Target `wpse-v1`. Issue #235 lists 25 functions in `.text` `0x80133EA0`–`0x80134BC4`, inside `game/ScriptCamera.cpp` ([F-231-1](231-script-camera-unit.md#f-231-1-unit-boundaries)). The first three were written by #234. `src/game/ScriptCamera.cpp` now reconstructs the rest of the unit, `0x80133F00`–`0x801352EC`, which also covers the functions of #236. objdiff shows every function of the unit at 100% except `AdjustPii` (99.96%, see F-235-4). The unit stays `NonMatching`: linked as `Matching` it differs only in the order of eleven trailing weak functions (F-235-4).

## F-235-1: `method_camAdjustPos` and its helper class

| Address | Identity | Evidence |
| --- | --- | --- |
| `0x80133F00` | File-local `ProjectToHeight(const MTX34&, const VEC3&, f32)` | Transforms the eye and a view-space point by the inverse view matrix and intersects the ray with the plane y = height; returns the point directly when the ray is horizontal |
| `0x80134000` | `method_camAdjustPos` | Binding string. Pops a margin, a maximum depth, three references (position, depth, clip flag) and a camera, runs the adjuster with `SCGetAspectRatio()`, and on success stores the position, depth and flag and pushes 1 |
| `0x80134150` | `CCameraAdjuster::Init(BWorldCamera*, u32 aspect, f32 maxDepth, f32 margin)` | Stores the view matrix and its inverse, the four safe-frame slopes at unit depth (`GetSafeFrame`, divided by `-near`) and the same slopes times the margin, clears the accumulators, takes the ground as `CDividedGround` and calls `Adjust` |
| `0x80134370` | `CCameraAdjuster::Adjust()` | For each of the four joined players' Piis, clamps the view position into the frame with `AdjustPii`; moves the Pii when the clamped ground position is in bounds, otherwise retries with older entries of the Pii's position history; Piis with no usable position are collected in a `vector<CPlayerPii*>` and moved under the first adjusted Pii. The result is the mean clamped view position projected to y = 0 |
| `0x80134890` | `CCameraAdjuster::AdjustPii(VEC3*, VEC3*, const VEC3&)` | Returns behind-camera (1), inside (0), moved (2) or blocked (3); raises the tracked depth to the deepest slope violation, bounded by the maximum depth, and sets the clip flag when it is positive |
| `0x80134CA0`–`0x80134CC0` | `GetPosition`, `GetDepth`, `IsClipped` | One load each, offsets 0x9C, 0x84, 0x98 |
| `0x80134D60` | `CCameraAdjuster()` | Default-constructs the two `MTX34` and two `VEC3` members |
| `0x80134310`, `0x80134360` | `BWorld::GetGround()`, `BWorldObj::GetWorld()` | `shared_ptr<BWorldObj>(mGround).get()` (offset 0x50, F-44); `mWorld` at 0x14 |
| `0x80134B00` | `CPlayerPii::GetPositionHistory()` | Returns the `deque<VEC3>` at 0x150 |
| `0x80134CD0`, `0x80134D10` | `DynamicCast<CDividedGround>`, `IsKindOf<CDividedGround>` | `GetType() & 0x8000` (`CDividedGround::TYPE`) |
| `0x80134AC0`, `0x80134AE0`, `0x80134C90` | `std::max<f32>`, `std::min<f32>`, `VEC3::operator/(f32)` | Reference `max`/`min` from `<algorithm>`; `1 / x` then `operator*` |

`CCameraAdjuster` is 0xAC bytes: view matrix 0, inverse 0x30, unit-depth slopes top, bottom, left, right at 0x60–0x6C, margin-scaled slopes at 0x70–0x7C, maximum depth 0x80, depth 0x84, count 0x88, view-position sum 0x8C, clip flag 0x98, result 0x9C and the `CDividedGround*` at 0xA8. The class and member names are descriptive (medium). The binding name is original.

Outside the unit, for the link: `CPlayer::IsJoined() const` `0x80092980` (byte at 0x10, low), `CPlayer::GetPii()` `0x800F8320` (locks the `weak_ptr` at 0x60, medium), `CDividedGround::IsOutOfBounds(const VEC3&)` `0x80075FB0` (casts a vertical ray through `mBSPTrees[0]` and returns true when nothing is hit, low), `VEC3::operator*(f32) const` `0x8009CA60`, and the `shared_ptr<BWorldObj>` constructor from `weak_ptr` `0x80065170` and `get` `0x800C99C0`. `IGameSession` slot 0x64 is `GetPlayer`, as in `CDlgPiiSelector.cpp`.

`CPlayerPii` derives from `CPii`, which derives from `BModelObj` (RTTI base list `0x803A94E4`); the constructors OR `0x40` and `0x80` into the type, so `CPii::TYPE` is 0x40 and `CPlayerPii::TYPE` 0x80. `CPlayerPii` is 0x168 bytes: the constructor's integer argument at 0x14C and the `deque<VEC3>` at 0x150. `CPii` adds 0x54 bytes after the 0xF8-byte `BModelObj`; their types are not established, so `include/game/CPii.h` keeps them as one storage view.

## F-235-2: pointer vectors share one `void*` implementation

`vector<T*>` calls reach functions that other units also call for unrelated pointer types: the `vector<const char*>` of `CDividedGround`, `BWorld`, `CGameSession`, `CPii` and `0x80121750` all use the destructor `0x80064840`, and `0x801089B0` and `0x80108A60` are a second type's analogues of `0x80134740` and `0x80134B20`. The constructor chain has four levels (`0x80134710` → `0x80134740` → `0x801089E0` → `0x80108A10`) and the destructor chain the same, the inlined `vector<const char*>` destructor checks `this` four times. `include/vector` therefore gains a partial specialization `vector<T*>` over a type-specific layer `__vec_pointer<T>` that derives from the shared `__vec_constructor<void*, allocator<void*>, true>`. The names of the two new layers are descriptive. Shared functions named for the link: `__vec_constructor<void*>` constructor `0x801089E0`, destructor `0x8005FDA0`, `push_back` `0x80108A80`, `back` `0x80108B30`, `size` `0x800E3E40`, `begin` `0x8010C360`, `end` `0x8010C370`, `clear` `0x8010C050`; `__vec_deleter<void*>` destructor `0x80064840`, `cap` `0x80108B20`, `alloc` `0x8010B910`, `max_size` `0x8010C3A0`; `allocator<void*>::allocate(n, hint)` `0x800FD480`, the `compressed_pair<unsigned long, allocator<void*>>` constructor `0x800FC390`, `swap<unsigned long>` `0x80087730`, `swap<void**>` `0x800FADE0` and `__throw_vector_length_error()` `0x80088550`, which is a shared weak function rather than a file-local one.

`reserve` (`0x80134840`) reallocates through `0x80134DB0`: a temporary `__vec_deleter<void*>` holding the allocator by value, `allocate`, a byte copy of the old elements that then zeroes the source, `clear`, and three `swap`s.

`deque<VEC3>` (new `include/deque`) keeps its start and size at 0 and 4 and a `compressed_pair` of a block-pointer ring buffer and the allocator at 8. `operator[]` adds the start, divides by 85 (1024 / 12) to pick a block through a three-level pointer ring buffer (`0x80134BB0` → `0x80134BC0` → `0x80134BD0`, data, start, size and capacity at 0, 4, 8, 0xC). Named for the link: `deque<VEC3>::size` `0x800F9B10`, `__deque_imp::buf` `0x800FCD50` and `__cdeque_buf<void*>::cap` `0x800FC080`. Container and layer names in `<deque>` are descriptive.

## F-235-3: emission order

The binding's helper functions are emitted in the original order only when `Init`, `Adjust`, `AdjustPii` and the three getters are defined inside the class and the constructor is defined after the binding. Inline functions defined after their first caller are emitted at the end in definition order, and template instances referenced from an in-class member body are deferred to the end (`DynamicCast<CDividedGround>`). `reallocate` must be defined outside `__vec_constructor` so that it follows the constructor. With this, the first 127 functions of the unit, through `0x80134DB0`, are in the original order.

## F-235-4: what remains

Linked as `Matching`, the DOL differs only inside the unit's last 0x4F0 bytes and in the relocations that point into them (two calls in #231, the deleter vtable, `extab` and `extabindex`). The original order after `reallocate` is the two `construct_at_end` functions, `get_deleter`, `dispose`, `~shared_ptr<CStageCamera>`, `~move_ptr<CStageCamera>`, `default_delete::operator()`, both `second` accessors, the `__vec_deleter(allocator)` constructor, `allocate(n)`, the `shared_ptr_deleter<CStageCamera>` destructor and `allocate(n, tag)`. The compiler emits `reallocate`'s callees immediately in body order (constructor and both `allocate` first), and the two template destructors before the virtual functions; Wii/1.3 gives the same order. In-class or out-of-class definitions of the constructor, `allocate` and `construct_at_end`, in either header position, change nothing. The original apparently registered these functions earlier, probably from the `push_back` reallocation path, which the linker strips; its contents are not recoverable from this unit. Every function of #236 is at 100% in objdiff.

`AdjustPii` loads -1.0f from `.sdata2` `0x804BA1E0`. In the unlinked object the `tanf` constants of the weak `TanFIdx` precede it, so objdiff reports 99.96%; the linker drops them with `TanFIdx`, and the linked `.sdata2` matches. `lbl_804BA1E0` is now 4 bytes, its real size.

Also fixed for the link: `.sdata` `0x804B5AB8` is `__RTTI__Q43std3tr16detail25shared_ptr_deleter_common`. The deleter base list of this unit references it by name; unnamed, the reference resolved to the weak copy in `CStageCamera.o`, which pulled its 0x2C-byte type-name string into `.data` and shifted every later object by 0x28.

## Method

GNU objdump and nm on the split and compiled objects and on `build/wpse-v1/main.elf`, `tools/re/unitdiff.py`, a Python comparison of the compiled object's function order with `symbols.txt`, a linker map (`configure.py --map`), and `configure.py`, `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **Weak functions dropped at link still decide the order.** A unit's emission order includes weak functions the linker later removes because another unit defines them. Their bodies are compiled from library headers, so differences in those headers can reorder the functions that remain.
- **In-class template references are deferred.** A template instance referenced from a member function defined inside a class body is emitted at the end of the unit, not after its caller; the same call from an out-of-class definition is emitted at once.
- **An unnamed RTTI object can pull in another unit's weak copy.** Data symbols named only `lbl_` in the original resolve, from compiled code, to the first weak definition in link order. Check the map for unexpected `(object,weak)` entries when `.data` shifts after a unit is linked.
- **objdiff relocation names on `.sdata2`.** A pooled constant can sit at a different offset before linking because of constants of weak functions that the linker strips; compare the linked image rather than chasing the 99.9%.
- **Ninja targets need backslashes on Windows.** A forward-slash object path builds nothing and prints nothing.
