# F-54-1: Boss and color-button tag processor boundaries

Target: `wpse-v1`, immutable original DOL and frozen manifest. The issue's survey boundary divides two source units. `CBossPii` begins at `0x8006BEE0` and ends after its secondary-base destructor thunk at `0x8006C7A8`. Its RTTI, two shared-pointer control types, and default-deleter names occupy the preceding data cluster. The class extends `CEnemyPii`, whose constructor, destructor and initialization body begin at `0x800A47E0`, `0x800A4890` and `0x800A48F0` in #115.

The next retained function, `0x8006C7B0`, installs the singleton `CColorBtnTagProc` on an NW4R TextBox after checking its runtime type. `Pane::GetRuntimeTypeInfo` at `0x8006C860` is the following weak helper. The class's remaining functions lie in neighboring #55. Its RTTI supplies the exact spelling `CColorBtnTagProc`; the descriptive source filename follows that class name. First-function ownership assigns the complete unit to #54. No #115 split is taken.

| Section | Complete color-tag unit extent |
| --- | --- |
| Text | `0x8006C7B0` to `0x8006CE2C` |
| Exception tables | `0x8000AF60` to `0x8000B030` |
| Exception index | `0x80044F40` to `0x80044F70` |
| Constructor entry | `0x803529C4` to `0x803529C8`, naming `0x8006CD00` |
| Data | `0x8039A960` to `0x8039A9E8` |
| Small data | `0x804B5C90` to `0x804B5CA0` |
| BSS | `0x803E6580` to `0x803E65E0` |

The first split checkpoint used BSS `0x803E6590` to `0x803E65EC`. That hypothesis is superseded by the initializer's exact address arithmetic. Its pool base is `0x803E6580`: the first 12-byte registration record precedes four padding bytes and the 16-byte processor at `0x803E6590`. The next record is at `0x803E65A0`, followed by five colors at `0x803E65AC`; the final record is at `0x803E65C0`, followed by five colors at `0x803E65CC`. The unit ends at `0x803E65E0`. The original symbol extent at that next address is restored. The data contains the Process jump table, class vtable, class and base RTTI names, and its base list. The next literal, `PIINAME_NORMAL`, begins the following unit.

With only the complete split and that owned BSS symbol correction, configure/map, default Ninja after deleting the prior DOL, and frozen-manifest verification each exit 0. The original split object is selected. No reconstructed-source credit or issue completion is claimed by this checkpoint.

# F-54-2: Complete color-tag source acceptance

`game/CColorBtnTagProc.cpp` is reconstructed as ordinary C++ with the established Wii/1.1 game flags. No reference implementation, executable bytes, inline assembly or compiler override is imported. The complete 16-byte class extends the canonical `nw4r::ut::TagProcessorBase<wchar_t>`. Its saved gradation, start color and end color occupy offsets 4, 8 and 12. `Process`, `CalcRect` and the destructor retain the inherited SDK virtual interface and order. The class spelling is original RTTI evidence. `Attach`, `SetColor`, static-storage and member spellings are descriptive, with medium confidence.

Three additive `CharWriter` accessors expose the already established start/end colors and gradation mode without changing layout. Their names are inferred. Both agents working on NW4R/font headers confirmed no conflicting change. Compiling each of the four rebuilt existing consumers with the original header and the candidate header preserves all allocated section payloads/metadata and normalized relocation targets: `ut_TagProcessorBase`, `ut_CharWriter`, `db_DbgPrintBase`, and `lyt_textBox`. The outside receipt is `54-reference/54-charwriter-control-proof.json`.

The processor consumes a tag after control character 1. Tag 1 restores the saved colors and then gradation. Tag 2 saves that state, enables vertical gradation, and peeks the next glyph without consuming it. Glyphs `E103`, `E104`, `E109` and `E10C` select color pairs 0 through 3; `E102`, `E105`, `E106`, `E107`, `E108`, `E10A` and `E10B` select pair 4. An unknown glyph still completes the tag operation; an unknown tag delegates to the SDK processor after consuming the tag. Rectangle calculation consumes tags 1 and 2 without drawing and otherwise delegates. Attachment checks the pane and SDK runtime type before installing the singleton. The color setter preserves the native unchecked array index.

| Native function | Address | Instruction bytes |
| --- | --- | --- |
| `Attach` | `0x8006C7B0` | 168 |
| `Pane::GetRuntimeTypeInfo` | `0x8006C860` | 8 |
| Destructor | `0x8006C870` | 88 |
| `Process` | `0x8006C8D0` | 904 |
| `CalcRect` | `0x8006CC60` | 52 |
| `SetColor` | `0x8006CCA0` | 96 |
| Static initializer | `0x8006CD00` | 216 |
| `Color` constructor | `0x8006CDE0` | 12 |
| Start-color array destructor | `0x8006CDF0` | 28 |
| End-color array destructor | `0x8006CE10` | 28 |

All ten retained functions, 1,600 instruction bytes, compare at 100%. With the unit source-selected, configure/map, default Ninja after deleting the previous DOL, and the explicit frozen verifier exit 0. The DOL is byte-identical with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The independent outside `54-source-audit.py` additionally verifies every function's actual ELF address/size and raw resolved bytes, source ownership in the final map, all native exception/index/constructor/data/small-data bytes, the three BSS globals and complete 96-byte BSS contribution, and selection of the compiled object without its original fallback.

The source object also emits the already existing weak `Color` destructor. The map reports an unreferenced duplicate; the actual definition stays at `0x8005C9A0` in `BWorld.o`. Its extra source-object exception record and index entry are discarded. The retained unit contributes exactly 208 exception bytes and 48 index bytes at their native addresses. The source's 132 data bytes match the native payload; four linker padding bytes complete the 136-byte native range. The BSS has 92 storage bytes plus four internal alignment bytes. No fabricated padding or credit for the outside destructor is added.

The first two accepted functions account for 176 instruction bytes in #54. The remaining eight account for 1,424 bytes in #55. The twenty `CBossPii` functions, 2,108 instruction bytes, remain original fallback dependencies, so this partial delivery does not complete #54. The distinct `CColorConst` unit remains with #55's owner.

# F-54-3: Boss prerequisite contracts

The script factory at `0x80153470` allocates 332 bytes for either `CBossPii` or `CEnemyPii`. Both have the existing `CPii` extent, `0x14C`; neither adds instance storage. The boss constructor adds type bit `0x200`, the enemy constructor adds `0x100`, and the `CPii` constructor adds `0x40` before calling `BModelObj`. The two-word argument is copied and retained as a by-value shared pointer at each derived-constructor call.

The shared pointer stored at `CPii+0xFC` targets `CPiiPersonalData`: the getter at `0x8006C310` feeds the HP ratio routine `0x800EC880`, which reads the initialized personal-data integer fields at `0x54` and `0x6C`; the constructor at `0x800EC3D0` initializes these along with the rare-color seed, time and personal ID. The HUD initializer reads the same object's classification at offset 8. The getter and member spellings remain inferred; the pointee class spelling is supported by the surrounding original default-deleter RTTI.

The destructor at `0x800DCBE0` proves strong shared-pointer pairs at offsets `0xFC`, `0x104`, `0x114`, `0x11C`, `0x130` and `0x138`, a weak pair at `0x124`, and a three-word dynamic array of eight-byte strong-pointer elements at `0x140`. This does not yet establish every pointee type. In particular, the pointer returned by the inline PPU accessor is the first word of the strong pair at `0x114`, not an independently owned raw pointer. No shared header is changed from these incomplete contracts.

The copied IDA database cannot currently decompile `0x800DCF40`; native disassembly remains available. In this function `r24` holds the object and `r31` holds the stack pointer. Large stack stores at offsets `0x140` and beyond therefore do not enlarge the class. Use the constructor, destructor and object-base register when validating field accesses.

The initialization argument is `bool background`. The native `CPii` initializer forwards it through the model loader at `0x801B8BB0` and scene loader at `0x801C75E0` to the existing `LoadResourceFile__FPCcUlb` interface at `0x80066930`; its canonical declaration is in `CFileData.h`. It is not a context pointer. The following complete header layout replaces the prior opaque `0x54`-byte array without changing `sizeof(CPii)`.

| Offset | Type and descriptive role | Native evidence |
| --- | --- | --- |
| `0xF8` | `u32` state flags | Constructor, marker-enable accessor and shadow update |
| `0xFC` | `shared_ptr<CPiiPersonalData>` | Constructor, getter and HP-ratio consumer |
| `0x104` | `shared_ptr<nw4r::g3d::AnmObjTexSrtRes>` | `0x800DE1E0`, creation at `0x802576B0`, exact deleter RTTI `0x803A80D0` |
| `0x10C`, `0x110` | `f32` texture-animation frame and rate | Float initialization and texture-animation virtual calls in `0x800DE1E0` |
| `0x114` | `shared_ptr<CPpuEngine>` | Constructor `0x800FE5F0`, exact deleter RTTI `0x8039BB00`, getter `0x8006C390` |
| `0x11C` | `shared_ptr<CHPBar>` | Getter `0x8006C2E0`, setter `0x800DE060`, boss and enemy HUD allocation |
| `0x124` | `weak_ptr<BWorldObj>` follow target | Weak release, lock in `0x800DF990`, target position and optional model-node lookup |
| `0x12C` | `int` follow-node index | Constructor sets `-1`; calculation uses it as the optional scene-node index |
| `0x130` | `shared_ptr<CMiniMapMarker>` | Setter `0x800DE140`, HUD constructor and exact deleter RTTI `0x8039A930` |
| `0x138` | `shared_ptr<nw4r::g3d::ScnObj>` shadow | Scene loader and canonical `ScnObj::SetMtx` at `0x802681F0` |
| `0x140` | `vector<shared_ptr<CFileData>>` shadow resources | Scene loader's resource-file acquisition and vector assignment/destruction |

The canonical base header's previously inferred `BWorldObjEffect*` return type is corrected to `CMiniMapMarker*`; the old invented forward declaration has no other uses. The inherited `GetEffect` method spelling and slot are preserved. The new `CPii` overrides follow the original 144-byte boss vtable exactly, including four appended slots: initialization, shadow-name query, visibility query and shadow-matrix update. These method and member spellings are descriptive with medium confidence, except class names pinned by original RTTI. `CEnemyPii` adds no fields and overrides initialization only.

An independently compiled boss consumer proves the new contract before any boss source selection. Its resolved constructor (168 bytes), destructor (88), PPU getter (8), personal-data shared-pointer destructor (132), secondary destructor thunk (8), and complete 144-byte vtable equal the immutable executable at their native addresses. The consumer compiles with the established game flags and checks the `0x14C` extent. This is prerequisite evidence, not source-completion credit.

All 34 existing compiled game objects retain identical allocated section payloads, section metadata and normalized relocation semantics after the header change. Compiler-generated local numeric identifiers change in `ScriptCamera.o`; normalization uses their section, value, size and ELF type rather than spelling. The fresh source-selected-game control recompiles all 16 current linked game sources against `origin/main`'s old headers in a separate include overlay and compares them to the candidate objects; all pass. This control is repeated after every rebase inside the official landing verifier, together with the boss contract probe and full frozen-DOL verification. No #115 split, source body or configure object is taken.

# F-54-4: Complete HUD helper header prerequisites

The canonical `CLayout` declaration from #329/#1040 is reused unchanged. Native consumers independently prove its `0xE8` extent inside both HUD helpers. `CHPBar` is `0x13C` bytes and derives from the original `IWorldObjStat` interface, whose class spelling is pinned by RTTI at `0x803A4564`. Its six virtual slots are destructor, world-object update, view/projection calculation, visibility query, draw, and type query. The descriptive method names and const qualifications are inferred; the pointer and matrix parameter types follow native accesses and canonical matrix calls. The interface occupies four bytes and has no data members or inherited base.

| CHPBar offset | Complete member type and role |
| --- | --- |
| `0x04`, `0x08` | `u32` flags and type |
| `0x0C` | Four-byte kind enum: player, enemy, boss, uninitialized |
| `0x10`, `0x14` | `int` player number and animation state |
| `0x18`, `0x1C` | `f32` current and delayed HP ratios |
| `0x20` | `int` HP delay frames |
| `0x24` | `VEC3` world position |
| `0x30` | Complete `CLayout` |
| `0x118`, `0x11C` | Bar pane pointer and `f32` original width |
| `0x120`, `0x124`, `0x128` | Yellow, green and level pane pointers |
| `0x12C` | `list<shared_ptr<CHPBar::CDamage>>`; exact nested class spelling from deleter RTTI `0x803A44C4` |
| `0x138` | `bool` damage-sound ownership; destructor releases the shared sound when the final owner leaves |

`CMiniMapMarker` is `0xF0` bytes: an `int` type at zero, a `CLayout` at four, and a pane pointer at `0xEC`. It has no vtable. Both helpers' initialization calls forward `bool background` to the same canonical layout/resource loader; `CHPBar` also takes the player index and a `CPiiPersonalData*`, while the marker takes its type and layout-name pointer. No opaque member array or fixed-size allocation substitute is introduced.

Independent consumers built against these canonical headers reproduce every resolved byte of the `CHPBar` constructor (164), `IWorldObjStat` destructor (64), damage-list destructor (220), all 32 bytes of the `CHPBar` vtable, and the marker constructor/destructor (72/92). The HP constructor also reproduces the exact original `1.0f` and `0.0f` pool values and SDA references. The HP destructor used solely to emit its consumer vtable is a probe stub and is explicitly excluded from source or native-code credit; its native body is not reconstructed by this prerequisite.

The recovered base type also corrects the earlier inferred `IDrawLayer` spelling. `CPii::SetHPBar` converts a `shared_ptr<CHPBar>` and dispatches to the `BModelObj::AddDrawLayer` slot. The canonical declaration and `DrawLayerSet` now use `IWorldObjStat`. A temporary `typedef IWorldObjStat IDrawLayer` preserves the external #39 owner's existing source signature without editing that source. It denotes the same complete original interface, not a second opaque class. The outside callee name at `0x800570D0` is updated to the actual compiler-emitted `AddDrawLayer__9BModelObjFRCQ33std3tr127shared_ptr<13IWorldObjStat>`; its size, split, body and type attributes are unchanged. The existing source body emits the same 144-byte instruction stream; objdiff reports 99.72% because two tree-helper calls still carry the old inferred pointee names. Those helper renames remain with the source-unit owner; no matching claim is added for that outside function.

The current source-selected game-object controls are repeated against `origin/main`'s prior headers in an isolated include overlay after every rebase, together with all three native consumer probes and frozen full-DOL verification. These headers add no source-completion credit and take no outside split or source body.

# F-54-5: Complete boss source and original issue acceptance

`game/CBossPii.cpp` is reconstructed with the existing Wii/1.1 game compiler and flags. The class extends the canonical `CEnemyPii`, adds no instance fields, and initializes the native boss type bit. Its initializer creates the missing HP bar and minimap marker through ordinary typed `shared_ptr` ownership, then calls the enemy initializer. The complete native unit is source selected; no original `CBossPii.o` fallback remains in the link graph.

| Section | Native accepted extent | Bytes |
| --- | --- | --- |
| Text | `0x8006BEE0`–`0x8006C7A8` | 2,248 allocated; 2,108 instructions |
| Exception tables | `0x8000ACD8`–`0x8000AF60` | 648 |
| Exception index | `0x80044EB0`–`0x80044F40` | 144 |
| Data | `0x8039A6A8`–`0x8039A960` | 696 |
| Small data | `0x804B5C50`–`0x804B5C90` | 64 |

| Address | Instruction bytes | Reconstructed function |
| --- | --- | --- |
| `0x8006BEE0` | 168 | Boss constructor |
| `0x8006BF90` | 88 | Boss destructor |
| `0x8006BFF0` | 748 | Boss initialization |
| `0x8006C2E0` | 40 | `CPii::GetHPBar` |
| `0x8006C310` | 40 | `CPii::GetPersonalData` |
| `0x8006C340` | 28 | `CPii::GetEffect` |
| `0x8006C360` | 40 | `CPii::SetEffectEnable` |
| `0x8006C390` | 8 | `CPii::GetPpuEngine` |
| `0x8006C3A0` | 76 | HP-bar control-block deleter query |
| `0x8006C3F0` | 36 | HP-bar disposal |
| `0x8006C420` | 76 | Marker control-block deleter query |
| `0x8006C470` | 12 | Marker disposal |
| `0x8006C480` | 132 | Marker shared-pointer destructor |
| `0x8006C510` | 132 | HP-bar shared-pointer destructor |
| `0x8006C5A0` | 132 | Personal-data shared-pointer destructor |
| `0x8006C630` | 116 | HP-bar move-pointer destructor |
| `0x8006C6B0` | 100 | Marker move-pointer destructor |
| `0x8006C720` | 64 | Marker control-block destructor |
| `0x8006C760` | 64 | HP-bar control-block destructor |
| `0x8006C7A0` | 8 | Secondary-base boss destructor thunk |

All twenty functions score 100%, including relocation targets. Actual linked bytes, addresses, native sizes and source-map ownership are independently compared against the immutable original executable. The original-only split checkpoint and subsequent source-selected build both complete configure, Ninja and frozen verification with exit 0 after removing the prior DOL. The final DOL SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The unlinked object also emits the existing 64-byte common deleter destructor, its eight-byte exception record and twelve-byte index entry. The map discards those duplicates and keeps the original earlier definition at `0x80055BB0` in `BModelObj.o`. Earlier base RTTI metadata is likewise discarded in favor of the existing named definitions. Consequently raw object exception/data fuzzy scores include metadata that is not retained by this unit. The final linked native ranges above are exact. The retained data payload is 692 bytes plus four linker padding bytes; no fabricated padding source is added. There are no boss BSS, constructor-table or small-zero-data ranges.

The initializer's copies establish that `SetHPBar` and `SetMiniMapMarker` take their shared pointers by value. The earlier header-only declarations used const references; those are corrected here before source selection. Const-reference parameters produced a 612-byte initializer; the by-value contract reproduces all 748 native bytes. The HP-bar constructor argument is the evidenced `0x6A`, and the marker flag is `0x4`. These values come from native immediates and bit masks, not from interpreting decimal decompiler output as hexadecimal.

The two shared-pointer getters are non-const. With otherwise identical bodies, const-qualified getters load the control pointer before storing the returned object pointer, while the non-const declarations reproduce the native interleaved load/store order and register use. This is a bounded compiler observation for these getters, not a general claim about C++ const qualification. The inherited marker accessors remain inline and emit at their native positions under the established IPA behavior.

Outside callee names for enemy construction/destruction/initialization, HUD helper calls, `CPii` overrides/setters and the already declared `BModelObj` methods follow the actual typed compiler interfaces. Their source bodies, splits, sizes and type attributes are untouched. Class names are original RTTI evidence; descriptive method/member spellings retain medium confidence.

The frozen original #54 inventory is 22 functions and 2,284 instruction bytes. Its twenty boss functions contribute 2,108 bytes; the first two functions of the complete color-tag unit accepted in F-54-2 contribute the other 176 bytes. Both complete units remain source selected, with all native code, data, EH, layout and fallback checks passing. This supersedes the earlier partial accounting: the original #54 scope is complete. The other eight color-tag functions remain the previously delivered contribution to neighboring #55.

## Traps

The issue's last two functions belong to the next class's unit. Preserve that class's initializer, registration records and neighboring #55 methods together; the survey issue boundary is not a source-file boundary.

- A decompiler's returned address can be incidental. Attachment is a `void` operation: forcing a pointer return adds a branch and explicit null result. The ordinary void method reproduces every native instruction.
- The setter's color parameters are values. Reference parameters retain the same ABI registers but schedule their byte copies differently; by-value `Color` parameters reproduce all 96 native bytes.
- A successful original-only split cannot prove global ownership. The first BSS estimate still verified because the original bytes remained linked. The initializer's base/addend arithmetic establishes the registration record before the singleton and the true end after the second color array.
- A shared-pointer setter can require a caller-side temporary even when its own body resembles assignment from a reference. Preserve the caller's retain/release pair with the evidenced by-value signature; the boss initializer is 136 bytes too short with const-reference setters.
- Identical getter bodies with different const qualification can schedule member loads differently in this compiler. The native interleaved stores establish non-const getters here; do not compensate with register declarations or compiler flags.
