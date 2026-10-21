# F-221-1: Vector erase count snapshot required by controller destruction

Issue #221 begins with a controller-table unit at `0x801248C0`–`0x80124C80`, rather than the survey's proposed filename helper. The following unit begins with Pokémon species/form validation at `0x80124C80`. The controller table has four concrete controller arrays and one `CControllerComposition`; its original class names, complete layouts and controller interfaces are supplied by the #57 owner. The composition is 64 bytes, with a pointer vector at offset 52. Its implicit destructor is `0x80124C00`, 128 bytes.

A scratch reconstruction using the canonical controller headers reproduces twelve of the thirteen controller-table function bodies immediately. The remaining composition destructor is four bytes short: the native clear path loads the current vector count and subtracts that count, while the current header folds the decrement directly to a zero immediate. This is an internal erase operation, not a different vector layout.

The independently tested prerequisite snapshots `size_` into a local `size_type n` before passing it to the existing `erase_at_end(n, pod_type())`. The existing erase implementation, allocator interface, overload selection, object layout and public vector API are unchanged. No self-subtraction expression, forced register, special flag, literal executable bytes or target-specific condition is added. The snapshot is an ordinary erase-count parameter. With it, the complete 128-byte composition destructor reproduces the native instruction sequence in the scratch controller-table producer.

## Independent native producer checks

Four existing actual source-object producers independently confirm the same header correction. Every instruction word is compared, resolving all eight call relocations to the canonical global delete at `0x801B1580`. Each producer's complete eight-byte exception record and twelve-byte exception-index contract also agrees with the native entry. The native artifact is read directly from the frozen DOL, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

| Actual source object | Native producer | Address | Instruction bytes |
| --- | --- | --- | --- |
| `game/CDividedGround.o` | `std::__vec_deleter<void*, std::allocator<void*> >` destructor | `0x80064840` | 112 |
| `game/CDividedGround.o` | POD pointer `std::__vec_constructor` destructor | `0x8005FDA0` | 116 |
| `game/CDividedGround.o` | `std::vector<const char*>` destructor | `0x800714F0` | 124 |
| `game/CPpuEngine.o` | `std::vector<nw4r::math::MTX34>` destructor | `0x800FF020` | 120 |

These checks cover 472 instruction bytes, 32 exception bytes and four index records. The unchanged exception payload is `1008000000000000`. Before the correction, these source bodies are respectively 108, 112, 120 and 116 bytes. The actual `CDividedGround` destructor also reaches its native 836-byte extent with no unequal non-relocation instruction words; the actual message-vector owning-pointer destructor reaches the same diagnostic result at 228 bytes. Those two additional checks are diagnostics, not independent resolved-unit acceptance.

## Same-base current-consumer controls

At base `d07bff51bffb04fb3799f73ad8d7803e2b606440`, both baseline and candidate compile every object declared by the real Ninja `all_source` target: 718 objects. Eighteen consumers rebuild after the header change. Of the complete object files, 712 are byte-identical. Six change: `BWorld`, `CDividedGround`, `CPpuEngine`, `Message`, `SaveData` and `ScriptCamera`. All six remain explicitly `NonMatching`; their original objects remain selected by the normal link. Twenty-one emitted function bodies change within these six objects, confined to vector clear/destruction and enclosing destruction paths. Their changed code is not reported as new source coverage.

The full linked ELF and DOL are byte-identical between baseline and candidate. ELF SHA-256 is `4bb3451d517073a4310394ee7fff88c608f6ad64e56a7a7d1fa594d0e87e073e`; DOL SHA-256 remains the frozen value above. Both runs execute `python configure.py`, `.venv/Scripts/ninja all_source`, the default Ninja build after removing only `build/wpse-v1/main.dol`, and explicit frozen-manifest verification, each with exit zero. No source selection, split, symbol boundary or unit status changes in this prerequisite. Issue #221 remains incomplete until its controller and species units pass their complete native and source-only link gates.

## Traps

A direct member expression passed into an inlined erase helper can fold to a zero immediate, while a normal count snapshot preserves the native read/subtract sequence. This was confirmed in four actual producers and the controller probe; it is not a general instruction to add temporaries. A const accessor produced two count loads and a 132-byte composition destructor; pointer-distance and reference forms also failed the native extent and were rejected. Shared-header acceptance still requires the full current-consumer and frozen-artifact controls.


The same snapshot was independently merged by the #60 owner in PR #1066 (`fbab3bbc`) before this continuation's publication. The #221 branch keeps that canonical implementation and publishes no competing vector change. The controller interfaces and `SetChannel` dependency were merged by their #57 owner in PR #1065 (`ba40c800`).

# F-221-2: Complete controller-table translation unit

The thirteen functions from `0x801248C0` through `0x80124C80` form one complete unit. They account for the first 888 instruction bytes of the original #221 inventory. All class declarations and motor/input interfaces are canonical headers from #57. `ControllerTable.cpp`, its manager-function spellings and its private static names are descriptive; the concrete class names are original RTTI identities.

| Function or native role | Address | Instruction bytes |
| --- | --- | --- |
| `InitControllers` | `0x801248C0` | 152 |
| `SetExtensionControllerEnabled` | `0x80124960` | 8 |
| `IsExtensionControllerEnabled` | `0x80124970` | 8 |
| `UpdateControllers` | `0x80124980` | 144 |
| `ShutdownControllers` | `0x80124A10` | 4 |
| `GetController` | `0x80124A20` | 28 |
| `GetUIInput` | `0x80124A40` | 12 |
| `GetControllerTable` | `0x80124A50` | 12 |
| Compiler static initializer | `0x80124A60` | 212 |
| Nunchaku implicit constructor | `0x80124B40` | 60 |
| Classic implicit constructor | `0x80124B80` | 60 |
| GC Pad implicit constructor | `0x80124BC0` | 60 |
| Composition implicit destructor | `0x80124C00` | 128 |

The initializer assigns channels zero through three to each concrete controller type. The update path calls all sixteen controllers and then the composition input. The empty shutdown hook is called from `main+0x858`, `0x8012C508`, in its final cleanup path. The table's four rows follow `IController::Type`: GC, Wii Remote, Classic and Nunchaku. The public input getters return the same composition object through its two supported interface types.

## Native sections and layout

| Section | Owned range | Bytes |
| --- | --- | --- |
| `extab` | `0x8002C200`–`0x8002C230` | 48 |
| `extabindex` | `0x8004C818`–`0x8004C860` | 72 |
| `.text` | `0x801248C0`–`0x80124C80` | 960 including 72 alignment bytes |
| `.ctors` | `0x803529E4`–`0x803529E8` | 4 |
| `.data` | `0x803B3030`–`0x803B3070` | 64 |
| `.bss` | `0x803E8D28`–`0x803E90C8` | 928 |
| `.sdata` | `0x804B77D8`–`0x804B77D9` | 1 |

The four arrays occupy 224, 208, 208 and 208 bytes in declaration order: Wii Remote at `0x803E8D28`, Nunchaku at `0x803E8E08`, Classic at `0x803E8ED8` and GC Pad at `0x803E8FA8`. The compiler's twelve-byte global-destructor registration record is at `0x803E9078`, followed by four alignment bytes. The 64-byte composition begins at `0x803E9088`. The static initializer's four `__construct_array` calls prove the array counts and element sizes; its ordinary composition construction and `__register_global_object` call prove the remaining allocation and destructor.

The previous eight-byte flag symbol was an inferred extent extending to the next unit. Its actual object is the one-byte boolean at `0x804B77D8`; native reads and writes use byte operations, and the actual source object's `.sdata` has size one and alignment eight. Seven native zero bytes before `0x804B77E0` are alignment, represented separately by `ControllerTableAlignmentPadding` with alignment one. They have no source-credit claim and require no source padding array. Merely ending the split at the byte boundary made dtk reject its automatic padding object's odd start; the explicit alignment entry preserves the proper source boundary and full binary identity.

## Acceptance evidence

The real `unitdiff.py` entry point reports thirteen of thirteen original functions at 100%, 888 instruction bytes, with `.ctors`, `.data`, `.bss` and `.sdata` at 100%. The compiler additionally emits four weak vector helper destructors and their exception records. The real linker discards those duplicate helpers; its fresh map retains exactly the native 960-byte text allocation, 48 exception bytes and 72 exception-index bytes for this unit. Raw object exception scores include the discarded helpers and therefore report 75%; retained native exception identity is checked directly.

A strict supplemental audit resolves every relocation in every original function, the complete pointer table, the static-constructor pointer and the flag. It compares every resulting byte to the frozen DOL, verifies all six exception records and index contracts, and checks every source BSS object's address and size against the linked ELF. The fresh map locates all thirteen original functions at their native addresses in `ControllerTable.o`; the actual Ninja link graph selects `build/wpse-v1/src/game/ControllerTable.o` and contains no `build/wpse-v1/obj/game/ControllerTable.o`. The separate seven-byte alignment object is disclosed above.

The split-only build and the subsequent real source-selected build both pass the default Ninja checksum and explicit frozen-manifest verification. The source-selected DOL is exactly 4,094,880 bytes with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Configure, unitdiff, Ninja and verification all exit zero. The supplemental native/map audit is repeated inside the official landing verifier after every rebase. This completes this unit, not issue #221: ten original functions and 824 instruction bytes remain in the following species/form metadata unit.

## Traps

An inferred scalar extent can include padding for the following source unit. Preserve the actual one-byte flag and represent alignment as metadata; do not enlarge the source variable or add a byte array. dtk's automatic odd-address `.sdata` padding split needs explicit alignment metadata here. Extra weak object functions and exception records must be distinguished from the retained linked allocation through the real map and exact artifact comparison.
