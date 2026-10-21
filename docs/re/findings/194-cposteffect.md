# F-194-1: CPostEffect ownership and remaining string construction contract

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #194 contains the complete CPostEffect translation unit: thirteen functions, 4,256 code bytes, from `0x800FD500` through `0x800FE5F0`. The following source investigation remains outside the build. No function is registered as reconstructed, no split is changed, and this finding does not establish issue completion.

## Recovered class and behavior

The native RTTI names CPostEffect and IPostEffect. IPostEffect derives from ICalcDraw, adds Show(int) and Hide(), and has a virtual destructor. CPostEffect is 80 bytes: the inherited vptr at zero, shared CFileData owner at four, integer mode at twelve, list of shared CModel owners at sixteen, shared ScnRoot owner at twenty-eight, and the complete 44-byte CaptureTexture at thirty-six. The list stores its count before its sentinel links, as the canonical list declaration already specifies.

CreatePostEffect takes an integer mode and background-loading flag. It loads `5:/post_effect.arc`, retains that file owner in a local, then constructs CPostEffect with the file owner by value. Passing a nested LoadFile expression or changing the constructor argument to a reference does not reproduce its lifetime sequence.

The constructor creates a one-camera ScnRoot with four scene slots, selects camera zero, initializes an aiming camera at `(0, 0, 5)` looking at zero, sets the native orthographic projection, and initializes RGB565 capture storage from the existing work buffer. The camera target is a VEC3 temporary copied into the posture record. Assigning three individual target floats removes sixteen native code bytes.

Draw processes each model separately. Its scene operations are Clear, PushBack, CalcWorld, CalcVtx, CalcMaterial, CalcView, GatherDrawScnObj, ZSort, DrawOpa, and DrawXlu. In particular, the native CalcVtx call precedes CalcMaterial. Calc advances each model by one frame. Hide clears the owning list.

The 428-byte texture-binding routine takes its shared CModel owner by value. It obtains a ScnMdl, traverses every material's texture entries, compares GetTexResName().GetName() against `dummy_capture`, assigns the captured GXTexObj through CopiedMatAccess, and validates that texture slot. GetTexName() has a different null-name path and generates a different function. An additive `const GXTexObj& CaptureTexture::GetTexObj() const` accessor reproduces the observed copy, but has not been published as a shared-header change.

## Exact outside object controls

The outside C++ candidate was compiled with the canonical Wii/1.1 game flags, with no flag overrides or assembly. Each comparison resolves every code relocation to its evidenced native target, and the reading ELF is checked against the frozen DOL before comparison.

| Native address | Function | Code bytes | Result |
| --- | --- | ---: | --- |
| `0x800FD500` | CreatePostEffect | 396 | Exact |
| `0x800FD690` | CPostEffect constructor | 592 | Exact |
| `0x800FD8E0` | IPostEffect destructor | 64 | Exact |
| `0x800FD920` | Draw | 316 | Exact |
| `0x800FDA60` | Calc | 92 | Exact |
| `0x800FDAC0` | Hide | 196 | Exact |
| `0x800FDB90` | Show | 1,556 | Twenty-five register-allocation instruction differences |
| `0x800FE1B0` | BindCaptureTexture | 428 | Exact |
| `0x800FE360` | CPostEffect shared-owner get_deleter | 76 | Exact |
| `0x800FE3B0` | CPostEffect shared-owner dispose | 36 | Exact |
| `0x800FE3E0` | CPostEffect move_ptr destructor | 116 | Exact |
| `0x800FE460` | CPostEffect destructor | 324 | Exact |
| `0x800FE5B0` | CPostEffect shared-owner deleter destructor | 64 | Exact |

The twelve exact functions total 2,700 bytes and ninety-four resolved code relocation sites. Their eleven exception records total 712 bytes and also match exactly. Show's 296-byte exception record differs only in its first word: candidate `0x285A0000`, native `0x305A0000`, reflecting the different saved-register count. All cleanup actions, offsets, and resolved destructor addresses agree. These are isolated-object results, not a source-selected link or full-unit acceptance.

The preserved local investigation is `C:/MassStorage/GitHub/194-reference`: `194-post-probe.cpp`, its include overlay, `194-compile.py`, `194-audit.py`, `194-native-proof.json`, and `194-eh-proof.json`. The audit reports the unresolved Show differences explicitly and must not be interpreted as passing merely because the script exits zero.

## Show and shared model evidence

Show clears the prior list, returns for effect eight, and indexes eight 28-byte definition records at `0x8035B8E0`. Each record supplies model and motion names, width, height, and scaling mode. It allocates a complete 220-byte CModel, constructs a path beginning `5:/post_effect.arc/`, appends the model name, and builds a null-terminated filename array.

The observed twenty-byte LoadInfo record contains a filename-array pointer at zero, manual-buffer-options byte at four, buffer flags at eight, view count at twelve, and model-name pointer at sixteen. Its aggregate initializer copies the native constant at `0x80352EA0`. Individual field assignments remove that copy. The native Load producer at `0x801B8BB0` consumes the record through its address; a by-value candidate adds twenty-four code bytes. The current candidate uses a const reference, but no unsettled Load declaration was included in the complete CModel header prerequisite, PR #1098.

Load failure is an early return, with its own destruction path. Wrapping the successful remainder in an if statement merges cleanup paths and removes native code. After successful loading, Show binds the capture texture, plays either the named motion or motion zero, computes the native aspect-dependent scale matrix, sets the scene object's local matrix, and appends the shared model to the list. In scaling mode zero, the aspect-ratio call precedes division by width; precomputing the reciprocal before that call adds a scheduling instruction. The current complete body has the exact 1,556-byte extent and agrees with all native load, playback, matrix, and list operation targets.

The remaining register difference begins at the narrow-string constructor. Native code retains the address of the local string in r27 across strlen and reserve, keeps the prefix's high address in r28, and uses r26 for the length. The candidate rematerializes the string address, retains the complete prefix pointer in r28, and uses r27 for the length. Subsequent mismatches are the corresponding register choices, including the later list sentinel. The independently investigated CItem loader in #139 shows the same persistent-string-address distinction. This is evidence for further investigation of the shared string-construction contract; it is not permission to alter the standard library merely to allocate another register.

The native function2 RTTI at `0x803A81C8` and function_base name at `0x803A83C0` specify `void(CModel*, const shared_ptr<CModel::CMotionProgram>&)` and the implementation's `false, true` flags. The complete ModelAim and CModel contract investigation was shared with #139 and incorporated into its separately controlled header prerequisite. This finding claims neither the original CModel constructor issue nor any model unit as completed.

## Declaration context and allocator diagnostic

The native owned data orders the archive literal first, then the CPostEffect shared-owner deleter vtable and type information, then the class vtables and type information, then the model-path prefix and capture-texture name. Ordinary out-of-line Show and BindCaptureTexture definitions emit their literals too early. Deferring those two bodies as inline definitions, including a complete inside-class fixture, reproduces the required literal placement. In that context Show is 1,552 bytes: it retains the prefix's high address as native code does, but still lacks the cached local-string address. The earlier out-of-line 1,556-byte extent was therefore coincidental.

An ordinary empty allocator destructor, tested against the current allocator with its converting constructor preserved, makes all thirteen functions' code and resolved targets exact in the inside-class fixture. That diagnostic is nevertheless rejected: Show's exception record grows from the native 296 bytes to 316 bytes and adds an allocator<char> cleanup; the CPostEffect destructor's record grows from 196 to 220 bytes and adds list-allocator compressed-pair cleanups. A throw() specification on the empty destructor does not correct those records. No allocator-destructor change is accepted, and this code-only result does not establish matching. The fixture is preserved as `194-inline-class-probe.cpp` outside the repository while the original library contract is investigated.

The independent `194-data-audit.py` control resolves and compares sixteen retained data objects from the inside-class fixture: the two owned vtables, four small-data RTTI records, six type-name/base-list objects, and three literals, together with the default-deleter type name. Every logical byte and every resolved data relocation matches. Their source ordering agrees with native ordering separately in `.data` and `.sdata`. The owned CPostEffect vtable is at `0x803A9618`, its RTTI at `0x804B7130`, IPostEffect RTTI at `0x804B7138`, shared-owner deleter RTTI at `0x804B7128`, and default-delete RTTI at `0x804B7140`. The data control is recorded in `194-data-proof.json`; it does not establish final split padding or a source-selected full link.

## Traps

- Identical function extent and cleanup tables still permit register differences. Show has the right size and every cleanup action, but remains unmatched because its twenty-five instruction words differ.
- Constructor spelling alone did not resolve the string lifetime: direct, copy, and temporary initialization produced identical unmatched code. Default construction followed by assignment generated a different operation sequence.
- Moving the string constructor out of class, factoring its reserve/replace sequence into a helper, adding allocator arguments, and adding empty allocator lifetime functions did not reproduce the native constructor. All such header variants were rejected and remain outside the repository. No allocator change is proposed.
- Replacing the prefix literal with a named array changed compiler knowledge and reduced the function extent; that diagnostic was rejected. No mutable prefix or artificial data object is proposed.
- Wii/1.3 produces the same 1,556-byte Show and the same twenty-five register differences as Wii/1.1. Changing compiler versions does not resolve this case.
- The proposed narrow-string operator+= is independently readable append behavior, but its shared-header publication still requires actual native callers and complete current consumer controls. No string-header change is accepted by this finding.
