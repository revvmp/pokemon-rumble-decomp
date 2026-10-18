# F-51-1: BWorldLightSet.cpp unit split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #51 covers `0x80068610`–`0x8006A100`, which is exactly one source unit, `game/BWorldLightSet.cpp`. The unit was initially split and registered `NonMatching`; complete source acceptance is recorded in F-51-5 below.

| Section | Range |
| --- | --- |
| `extab` | `0x8000A26C`–`0x8000A6DC` |
| `extabindex` | `0x80044C04`–`0x80044D00` |
| `.text` | `0x80068610`–`0x8006A100` |
| `.data` | `0x8039A160`–`0x8039A2B8` |
| `.sdata` | `0x804B5B98`–`0x804B5BB0` |
| `.sdata2` | `0x804B9D10`–`0x804B9D28` |

## Evidence

- `0x80068610` stores the `BWorldLightSet` vtable (`.data` `0x8039A208`, 0x48 bytes, RTTI `.sdata` `0x804B5BA0`) and calls the `BWorldObj` constructor at `0x8006A100`. RTTI bases: `BWorldObj`, `ICalc`, and `std::tr1::enable_shared_from_this<BWorldObj>`.
- The previous unit, `BWorldCamera.cpp`, ends at `.text` `0x80068610` and `.data` `0x8039A160` ([F-50-1](50-bworldcamera-unit.md)). This unit's data begins with the `shared_ptr_deleter<BWorldLightSet::CMotionProgram, default_delete<...>>` vtable at `0x8039A160`. Its entries are the destructor `0x8006A0C0`, `get_deleter` `0x80069E80` and `dispose` `0x80069ED0`. The data then holds the `BWorldLightSet` and `enable_shared_from_this<BWorldObj>` vtables and the `default_delete<BWorldLightSet::CMotionProgram>` type name, which ends at `0x8039A2B8`. That address is the `BWorldObj` vtable, used by the `BWorldObj` constructor `0x8006A100`, which starts #52's unit.
- `.sdata` holds the deleter type info (`0x804B5B98`), the `BWorldLightSet` type info (`0x804B5BA0`) and the `default_delete` type info (`0x804B5BA8`, used by `0x80069E80`). `.sdata2` `0x804B9D10`–`0x804B9D20` is used only by `0x80068B50`, `0x800696A0` and `0x80069D80`. There is no `.bss`, `.sbss` or `.ctors` contribution.
- The only foreign data reference is the `shared_ptr_deleter<nw4r::g3d::AnmScnRes, ...>` vtable at `0x80399A88`, which is `BWorld.cpp`'s weak first copy.

## Historical split-only blocker (resolved by F-51-5)

Reconstruction needs the same MSL C++ `tr1::shared_ptr` and `enable_shared_from_this` templates as `BWorldCamera.cpp` (#53, #327), plus the `ICalc` and `BWorldObj` declarations (PR #906 for `ICalc`). Source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

No new trap beyond [F-50-1](50-bworldcamera-unit.md#traps).

# F-51-2: Shared interfaces required by light-set reconstruction

The shared smart-pointer, list, `ICalc` and `BWorldObj` declarations now exist. Native instruction review establishes two additional shared interfaces before adding the light-set implementation. This prerequisite adds no source unit or code credit and leaves both existing source selections and all original inputs unchanged.

`CFileData::IsValid() const` is an inline descriptive accessor returning `!mFailed && mData != nullptr`. Both the light-set loader at `0x80068880` and length query at `0x80069600` first require a nonempty shared pointer, then read the existing failed byte at `0x28` and data pointer at `0x1C`. It adds no storage or virtual slot. The existing complete `CFileData` storage declaration is preserved.

`CShadowCamera` derives from the existing 0x68-byte `BWorldCamera`. Its constructor `0x8010C490` ORs type bit4, calls the base constructor and constructs three 0x2C-byte `CaptureTexture` objects at `0x6C`. Its destructor `0x8010C520` destroys that exact array before calling the base destructor. The complete directly evidenced storage is:

| Offset | Type and descriptive member | Native evidence |
| --- | --- | --- |
| `0x68` | `int mTextureLevels` | `0x8010C5A0` stores an integer level count, then loops over that many textures with 0x2C stride |
| `0x6C-0xF0` | `CaptureTexture mTextures[3]` | Constructor/destructor array records and texture-initialization calls |
| `0xF0` | `bool mUseTextureAlpha` | Initialized true; passed to the shadow-material helper and bypasses alpha minimization when true |
| `0xF1` | `u8 mAlpha` | Initialized 255; material update minimizes it against the observed material alpha when texture-alpha mode is false |
| `0xF2` | `bool mAutoSelectLight` | Initialized true; both `BWorldLightSet::Calc` and its frame-transition path test it before `0x8010D7D0` |
| `0xF4` | `int mLightSetIndex` | Written by the light-selection scan and passed to `ScnRoot::GetLightSet` |
| `0xF8` | `int mLightIndex` | Written by the same scan and passed to `LightSet::GetLightObj` |
| `0xFC` | `f32 mPlaneHeight` | Initialized zero; the visibility calculation negates it into the fourth component of the `(0,1,0,d)` projection plane |

Implicit alignment after the bool places the first light index at `0xF4`; the complete class is 0x100 bytes. No padding array or opaque offset class is added. All member spellings and new accessor/method names are descriptive, medium confidence in behavior and low in original spelling. Existing class names are original RTTI spellings.

The vtable at `0x803ADB34` has the unchanged `BWorldCamera` slot order. Only its destructor at `0x8010C520` and `UpdateVisible` at `0x8010C750` override inherited entries. The direct `SelectLight` method at `0x8010D7D0` scans light sets and selects the first diffuse light, storing its set/light indexes; on failure it stores both indexes zero. It does not add a virtual slot. #207 owns the constructor and #208 owns this selection routine; neither routine is implemented or credited by this header prerequisite.

## Validation

The established game compiler and flags remain unchanged. `python configure.py --map`, the normal default Ninja build after deleting only `main.dol`, and the explicit frozen-manifest verifier all exit0. Of703 existing source objects,700 retain identical whole-file hashes. The three consumers BWorld, SaveData and sound have different compiler metadata/local labels; independent control compilations with only the two original headers substituted prove every allocated section and every relocation target identical. Local relocation targets are compared by section plus offset, external ones by name plus addend, without dropping opcode/register bits. The complete DOL is unchanged. The later light-set implementation must prove native uses of the two accessors and the selection call, in addition to its whole-unit source acceptance.

## Traps

- An earlier split-only blocker can become stale after shared templates land, but inspect the full unit before assuming it is independent: `BWorldLightSet` also consumes the file readiness byte and the complete shadow-camera interface.

# F-51-3: List operations consumed by the light-motion queue

The existing shared list lacks `erase(iterator)`, `pop_front()` and `swap(list&)`. The light-set request-next path at `0x80068E70` uses all three operations: preserve the first motion in a temporary list, erase the current head, stop and clear the remaining old queue, then swap the preserved head back. Frame advancement at `0x800696A0` also removes the current head before proceeding to the next request. The native ring repair exchanges size and both sentinel links, reconnects nonempty rings to their new sentinels and reinitializes empty rings.

The three ordinary list methods are added as a shared-header prerequisite, without modifying any previously available method or the container's twelve-byte layout. No source selection changes and no new coverage is claimed. The actual light-set consumer must still prove the generated inline operations against the target. Configure, default Ninja after removing only the old DOL, and frozen executable verification exit0. Of703 existing source objects,696 retain their whole-file hash. Original-header control compilations prove every allocated section and normalized relocation identical for the seven recompiled consumers: BWorld, CDividedGround, CMsgBalloon, CPpuEngine, CStageCamera, SaveData and ScriptCamera. The comparison preserves opcode/register bits and resolves local symbols to section offsets. These gates repeat on the header merge head.

## Traps

- Swapping just the two endpoint pointers leaves a list linked to the other object's sentinel. Native code reconnects both endpoints and separately initializes empty rings; preserve that behavior in the standard container operation.

# F-51-4: Exception-safe list insertion and C-string assignment

The motion queue establishes the remaining inline standard-library interfaces against all 27 native functions, totaling 6,740 instruction bytes, before accepting the game unit itself. Function comparison is provisional here; the unit remains `NonMatching` until allocated data, exception metadata, relocation targets, retained native positions and the complete executable pass independently.

The list has two allocator subobjects: the value allocator shares the size word at offset 0, while the node allocator shares the sentinel at offset 4. Both use empty-base compression, preserving the twelve-byte container. The native request-next routine stores the sentinel address in its allocation guard, establishing the second allocator's address independently of container size. Typed node links and iterator storage preserve the observed ring. Clearing passes the begin and end pointers directly to the existing range eraser; single-node removal destroys its value through the node member before deallocation. Swapping repairs both rings and initializes an empty ring in predecessor/successor order.

Insertion holds the allocated node in `Metrowerks::alloc_ptr` until value construction succeeds, then releases ownership before linking it into the ring. The guard stores an allocator reference and a compressed capacity/pointer pair. A stateless `number<size_t,1>` supplies the single-node capacity. Both the temporary pair copies and the exceptional deallocation branch are present in the native 304-byte request-next routine. A simpler raw allocation or an ad-hoc pointer/reference holder elides required cleanup instructions. The independent guard implementation is noncopyable and is used only for allocation ownership; it never destroys an unconstructed value.

The CC0 reference reconstruction in [Swiftshine/key](https://github.com/Swiftshine/key/tree/e17d094e477bf094c03544a709ea3922192f993a/src/PowerPC_EABI_Support/MSL/MSL_C%2B%2B) documents list insertion with an allocation guard, both allocator subobjects, and the nested compressed-pair interface. It is a structural reference, not a replacement for native comparison. Its incomplete or explicit ABI/decompiler-style declarations are not imported. The small storage-capacity policy is independently implemented and verified against this target; no proprietary header is vendored.

An explicit iterator copy constructor reproduces native iterator lifetime handling. C-string assignment captures the existing string size before evaluating the new string's length and delegates to the established range replacement. Keeping the length expression inside the replacement call reproduces the native empty iterator-tag evaluation order. Computing the new length in another local or using a range `assign` changes the 516-byte motion factory. All27 native functions compare at 100% with these shared interfaces, including allocation failure cleanup and the existing string destructor.

## Validation

`python configure.py --map`, default Ninja after deleting only the old DOL, and the explicit frozen-manifest verifier all exit 0. Of 704 pre-existing source objects, 694 retain their whole-file hash. Seven changed objects have identical allocated bytes and normalized local/external relocation targets under original-header control compilations. Three remain explicitly `NonMatching`: CDividedGround changes register selection in its already-unmatched destructor; CPpuEngine changes its unmatched destructor, map-tree destructor and allocator destroy helper; SaveData's unmatched `WriteBanner` gains the allocation guard and its unused destructor specialization. The strict control audit permits only those named candidate functions and requires all three object lines to remain `NonMatching`. None supplies linked code or new coverage. The complete executable remains byte-identical to the frozen original. The same audit repeats after rebasing the header prerequisite.

## Traps

- Node allocator placement matters even when both allocators are empty: the native allocation guard holds the sentinel address, not the size-word address. Preserve the two compressed allocator subobjects instead of relying only on total container size.
- Empty capacity policies and iterator tags can produce observable constructor-copy instructions. Preserve their actual owning interfaces and evaluation order; dummy stores or padding are not a reconstruction.

# F-51-5: Complete source-selected light animation unit

Issue #51 is the complete BWorldLightSet unit:27 functions and6740 instruction bytes. Its idiomatic C++ implementation and complete class header now replace the original object using the established Wii/1.1 game flags. No assembly, embedded executable bytes, binary rewriting, expected-hash changes or source emitter is involved.

## Behavior and interface evidence

The loader rejects a null path or an invalid file, preserves an already-loaded file, requires successful resource binding and scene animation availability, stops the old queue and then adopts the resource. The path overload delegates to a private file-handle overload. Playback stops old requests, queues a motion and either starts at its requested frame or carries forward the previous frame. Request preserves the current motion while replacing pending motions; request-next appends. Stopping marks each old request stopped before clearing ownership. Pause changes only the current motion's pause flag. Frame advancement applies rates, carries surplus time into the next request, handles looping and its count, and applies the final light frame before removing a completed one-shot motion. Light updates also select a shadow-camera light when automatic selection is enabled.

Native script bindings and constructor/factory accesses establish the motion flags, rate, frame pointer, resource and frame length. Mutable shared-pointer references are required by all four direct motion operations: treating the factory parameter as const collapses pointer reloads that the original preserves. The path wrappers construct an ordinary local handle and pass it by reference. The nested class name and both class sizes are independently corroborated by RTTI, native allocation and destructor behavior; detailed storage and naming confidence are in the symbol record.

The private file overload and `ApplyFrame(MotionPtr&)` factor coherent validation and repeated lighting behavior. They are descriptive reconstruction helpers, not recovered original spellings. The linker map proves both are fully inlined and discarded, with no retained address or additional coverage. In the one-shot path, SetFrame reads the owner's stored frame, preserving the original single-precision conversion; replacing that access with the arithmetic local removes an instruction. Initializing a queued start frame from zero before the negative-frame test preserves the native operand order and NaN behavior.

Outside symbols are named without changing their sizes or ownership: the existing general-purpose `LoadResourceFile` at 0x80066930, the existing scene-animation pointer-holder destructor at 0x80065980, its vtable at 0x80399A88, and `CShadowCamera::SelectLight` at 0x8010D7D0. The loader name reuses the canonical declaration already in CFileData.h. The pointer-holder destructor is independently identified by the original motion factory's exception-action relocation and its 108-byte body; it remains linked from the earlier original BWorld unit. No code from neighboring issues is reconstructed or credited.

## Exact acceptance

Configure with `--map`, default Ninja after deleting only the old DOL, and the explicit frozen-manifest verifier exit 0 with the source unit selected. Function comparison reports 27/27 functions at 100%, covering 6,740/6,740 native instruction bytes. Independent acceptance also checks every raw retained compiler block without masking opcode/register or relocation bits, all 228 paired relocation types/offsets and resolved targets, final native addresses in the actual linker map, and every retained allocated section:

| Section | Retained source payload bytes | Native zero alignment bytes |
| --- | --- | --- |
| `extab` |1136|0|
| `extabindex` |252|0|
| `.text` |6740|156|
| `.data` |337|7|
| `.sdata` |24|0|
| `.sdata2` |16|8|

The eight constant-section alignment bytes comprise four bytes before the eight-byte conversion bias and four before the following unit's constant contribution. The existing split is preserved; no padding is fabricated or credited. Every native relocation lies in a source-produced retained block. Unretained compiler helpers either resolve to an earlier existing definition or have no final address. The Ninja dependency graph includes the source object and excludes the original BWorldLightSet object. The complete 4,094,880-byte main.dol is byte-identical to the original and passes the unchanged frozen SHA-256 manifest. These gates repeat on the exact merge head after rebasing.

The original split-only blocker in F-51-1 is resolved by the shared templates and the prerequisites recorded above. No remaining function or data fallback belongs to #51. The rest of the game retains its explicitly documented original objects; this result is limited to the main executable's light-set unit.

## Traps

- An exception-action callee can keep an unwanted weak cleanup helper even when every function comparison is 100%. Naming the existing earlier scene-animation holder destructor eliminates the duplicate 108-byte function and its exception records; raw code comparison alone does not check that dependency.
- Two equivalent-looking source expressions can differ in both rounding and operand ordering. Preserve the owner's stored frame access and the native start-frame clamp, including NaN behavior, then prove the complete link.
