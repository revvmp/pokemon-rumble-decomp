# F-221-1: Vector erase count snapshot required by controller destruction

Issue #221 begins with a controller-table unit at `0x801248C0`–`0x80124C80`, rather than the survey's proposed filename helper. The following unit begins with Pokémon species/form validation at `0x80124C80`. The controller table has four concrete controller arrays and one `CControllerComposition`; its original class names, complete layouts and controller interfaces are supplied by the #57 owner. The composition is 64 bytes, with a pointer vector at offset 52. Its implicit destructor is `0x80124C00`, 128 bytes.

A scratch reconstruction using the canonical controller headers reproduces twelve of the thirteen controller-table function bodies immediately. The remaining composition destructor is four bytes short: the native clear path loads the current vector count and subtracts that count, while the current header folds the decrement directly to a zero immediate. This is an internal erase operation, not a different vector layout.

The bounded prerequisite snapshots `size_` into a local `size_type n` before passing it to the existing `erase_at_end(n, pod_type())`. The existing erase implementation, allocator interface, overload selection, object layout and public vector API are unchanged. No self-subtraction expression, forced register, special flag, literal executable bytes or target-specific condition is added. The snapshot is an ordinary erase-count parameter. With it, the complete 128-byte composition destructor reproduces the native instruction sequence in the scratch controller-table producer.

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
