# Game small-tail continuation, 2026-11-04

This pass initially owns `CPlayerPii.cpp`, `CMsgRes.cpp`, `CShadowCamera.cpp`, and `CDlgCollection.cpp`; the coordinator subsequently takes the remaining player-name work and authorizes the new `CSimpleGround.cpp` source. Experiments use the configured native compiler directly and complete objdiff JSON in isolated `tools/local/tail-20261104/smallgame/` directories. Shared configuration, headers, full builds, and frozen-artifact verification remain with the coordinator.

## Final source shape and native link

The coordinator's isolated native link of separately compiled Camera and Ground objects passes the frozen SHA-256 with zero differing bytes. Its artifacts are under `tools/local/tail-20261104/links/shadow-fragments/`. The final source uses the existing `nw4r::ut::Color` type and ordinary separate `CShadowCamera.cpp` and `CSimpleGround.cpp` translation units. All seventeen function instruction streams match.

The true split is now integrated into the normal configuration. The coordinator reports `ninja` exit 0 followed by frozen-manifest verification PASS: `main.dol` SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Both units are source-linked, with all eight Camera functions and all nine Ground functions exact. The corrected boundaries change the report denominator by twelve code-padding bytes and minus eleven data-padding bytes; the executable is unchanged. The configured unit count is 1,387.

`SetupShadowMaterial` takes `nw4r::ut::Color` by value. Passing this type directly to `GXSetTevKColor` reproduces the target address scheduling; the custom trivial color wrapper described below is removed. The single inline `ApplyShadowMaterial` helper takes a `const nw4r::ut::Color&`, forwards the actual alpha field by `const bool&`, and obtains the camera index directly in the final call. The outer temporary conversion and inner by-value copy reproduce both original Color cleanup actions. The extra forwarding level and local GXColor copy from the interim implementation are removed.

The final project-assigned local symbol at `0x8010D140` ends in `COffScreenTextureQ34nw4r2ut5Colorib`. The callee's eight-byte exception record is exactly `5008000000000000`. The caller's 32-byte record is exactly `280800000000017c00000010000000000200000c000000008200002800000000`, with `__dt__Q34nw4r2ut5ColorFv` relocations at record offsets `+0x14` and `+0x1C`. That destructor is already owned at `0x8005C9A0`. These checks distinguish the final source from the instruction-only interim signature.

The retained D-0016 scheduling control is the local `void*` receiver identity before the post-matrix calculation. The typed Color parameter and separate source files replace the earlier artificial color-copy type and extra inline layer. The field-reference forwarding helper remains documented as a source control rather than a recovered original helper definition.

### Source-file boundary

Combining the two classes into one translation unit pins extra type-name data through Camera's OSReport literal pool. The first combined source link was 416 bytes too large. The original Camera data ends with `void (*)(void *) throw()`; its source section is 601 bytes with eight-byte alignment. Ground has no such live literal-pool anchor, allowing its unused CModel, CPpuEngine, and base-class type metadata to be discarded. Its retained data is 200 bytes: the 132-byte vtable, a padded 16-byte name, and the 52-byte base list. Camera's 601 bytes pad to 608, then Ground's 200 bytes reproduce the original combined 808-byte extent.

The separate objects preserve every function body and all owned exception records, and the actual filenames with reviewed direct headers do so too. The exact proposed ownership ranges are:

| Section | CShadowCamera.cpp | CSimpleGround.cpp |
| --- | --- | --- |
| extab | `0x80028E54`–`0x80028F5C` | `0x80028F5C`–`0x800290E8` |
| extabindex | `0x8004BC9C`–`0x8004BCFC` | `0x8004BCFC`–`0x8004BD44` |
| .text | `0x8010C490`–`0x8010D8D4` | `0x8010D8E0`–`0x8010DDD8` |
| .rodata | none | `0x80354E20`–`0x80354E34` |
| .data | `0x803ADA30`–`0x803ADC89` | `0x803ADC90`–`0x803ADD58` |
| .sdata | `0x804B7358`–`0x804B7360` | `0x804B7360`–`0x804B7368` |
| .sdata2 | `0x804BA0D8`–`0x804BA108` | none |

Ends are exclusive. The gaps are ordinary alignment: twelve text bytes, seven data bytes, and four bytes after the twenty-byte `CModel::LoadParams` initializer. The former `lbl_803ADB90` extent included the last seven data-padding bytes, and `lbl_80354E20` included the last four rodata-padding bytes. Their proposed sizes are `0xF9` and `0x14`, respectively.

Reproducible inputs and records are `colorparity_0/`, `color_accepted/`, `fragment_camera/`, `fragment_ground/`, `actual_CShadowCamera/`, `actual_CSimpleGround/`, `actual_camera_installed/`, and `actual_ground_installed/`. The focused runners are `smallgame_shadow_color_cleanup.py`, `smallgame_shadow_colortype.py`, `smallgame_shadow_color_copy.py`, `smallgame_shadow_color_parity.py`, `smallgame_shadow_fragments.py`, and `smallgame_shadow_actualsplit.py`. `smallgame_elf.py` reads ELF records without modifying object files.

## Superseded first checkpoint: material instructions

At the first checkpoint, `SetupShadowMaterial` at `0x8010D140` compares at 100%, retaining its 1,252-byte size. A fresh control reproduces the prior 99.8754% result. The complete unit moves from 15/17 to 16/17 exact real functions; every other function score and the matched function order are unchanged. At that point `SetupShadowModel` remains 95.20192% at 416 bytes. This is function-comparison evidence, not completed source linkage for the unit.

Two local D-0016 controls resolve the five remaining instruction rows:

- A trivial `ShadowMaterialColor : GXColor` constructor copies the four-byte value through a `u32` view. Passing its temporary to `GXSetTevKColor` puts the argument address before the receiver address, matching the target offsets `+0x210` and `+0x218`.
- Immediately after the second-stage declaration, a `void*` local retains `&texObj` through `pTexObj = static_cast<void*>(pTexObj)`. Casting it back only for `GetTexObj` makes the receiver address live at the right point: the post-matrix subtraction uses `r5`, and the receiver address precedes the final matrix addition at offsets `+0x18C`, `+0x194`, and `+0x198`.

These are documented fake-match constructs, not recovered original source. Typed `ResTexObj*` identities and reassignments do not reproduce the second result. Moving the void-pointer declaration before the second-stage declaration gives 99.92971%, while placing it immediately afterward gives 100%.

The first color experiment used the existing `nw4r::ut::Color`. It reproduced the same exact instructions but emitted an additional 64-byte weak destructor, 24 exception bytes, and 12 exception-index bytes. It was replaced by the trivial local copy type before acceptance. In the retained version, every object section has the same size as the control. Raw section comparison changes only `.text` and compiler-generated names in `.strtab`; `extab`, `extabindex`, all data sections, all relocation sections, and the symbol table are byte-identical to the control. The `.text` difference consists of eleven bytes in the five corrected instructions.

The checkpoint runner is `tools/local/tail-20261104/smallgame_shadow_accept.py`. The compiled LF source snapshot SHA-256 at this checkpoint is `aa7db07872d1845d18e64bbb19896dcdc595211d86321b7317eefbedc22dcdf2`. `material_control_original/` and `material_accepted/` contain the full source snapshots, compiler logs, objects, and objdiff JSON. `material_acceptance.json` records the no-regression and section checks. The discovery runners are `smallgame_shadow_receiver.py` and `smallgame_shadow_trivialcolor.py`.

## Superseded intermediate checkpoint: model instructions

The subsequent candidate reproduces every instruction, register, branch, and frame offset of the 416-byte `SetupShadowModel` at `0x8010D630`. Against the saved pre-rename target object its score is 99.95192% solely because the call at `+0x178` still names the old reconstructed helper signature. The coordinator must rename that local symbol, regenerate the original split object, and verify the complete source link before accepting the unit.

`SetupShadowMaterial` now accepts `const GXColor&` instead of `GXColor`; the pointer ABI in `r6` is unchanged, and its complete instruction comparison remains 100% under an explicit private objdiff name mapping. This parameter category is a source-model choice, not a recovered original mangled name. The project-assigned local name changes from the suffix `COffScreenTexture8_GXColorib` to `COffScreenTextureRC8_GXColorib` without changing address, size, or local scope. A read-only scan of all original DOL text sections finds exactly one direct branch to this helper: `bl` at `0x8010D7A8`, inside `SetupShadowModel`. The helper is static and has no vtable entry. The original file SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Three related source controls reproduce the caller:

- `ApplyShadowMaterialCopy` takes the input color by reference and creates its own `GXColor` copy before reading the camera index. Passing that copy by reference to the out-of-line material helper preserves the target's color-copy, index-load, texture-multiply, and material-copy order.
- Passing `mUseTextureAlpha` directly into the inline wrapper as a `const bool&` places its load at the target instruction. Calling the by-value getter loads it too early. No shared camera getter or header changes are needed.
- One additional inline forwarding level reverses the final material/color frame-slot pair. Without it, the instruction stream is otherwise identical but those arguments use stack `+0xC` and `+0x8` in the opposite order. This forwarding level is a D-0016 control.

The installed formatted source was directly compiled into `model_accepted/`; its compiled LF source SHA-256 is `bb95caef872828fa66ef52c86a2884ef172d082cfd4f4eee0bd44a433b045b44`. Its only nonexact mapped function is the caller's expected old-name relocation. Compared with the accepted first checkpoint, the object has no new functions or section-size growth apart from the two-character symbol-name extension in `.strtab`. Raw `extab`, `extabindex`, data sections, and every relocation section are byte-identical. Only `.text`, symbol name offsets in `.symtab`, and `.strtab` change. The `.text` difference is 22 bytes in the corrected model argument-preparation sequence.

`smallgame_shadow_calltypes.py`, `smallgame_shadow_refcolor_frames.py`, `smallgame_shadow_refcolor_local.py`, and `smallgame_shadow_refcolor_depth.py` reproduce the discovery. `smallgame_shadow_mapped.py` uses an ordinary private objdiff project mapping; it does not alter target objects. `smallgame_shadow_callers.py` records the original branch audit in `callers.json`.

## Collection instruction completion

The final 2,284-byte `Calc__Q228@unnamed@CDlgCollection_cpp@14CDlgCollectionFv` now compares at 100%. All 113 real functions in the complete object are exact, and no previously exact function changes. The later source-link audit passed the frozen executable comparison in `tools/local/tail-20261104/links/collection-all-text-eh/verification.json`. The coordinator integrated normal source selection, then completed two clean byte-identical builds and checkpoint `1f6e0fc5`. No custom text section is retained. See [the complete linkage evidence](cdlgcollection-tail.md).

The baseline had only a register swap: the model pointer used `r29` instead of `r26`, while the loop's byte offset used `r26` instead of `r29`. The form masks already had the required `r28` and `r27`. The allocation trace shows that simply lifting a typed model pointer into the outer virtual function is insufficient. Giving the outer local a `void*` type, passing it through the existing inline `CalcModels` and item-model `Calc` functions by reference, and casting only at member accesses preserves the outer named local. This puts the item pointer in `r26`, improving the result to 99.842384%, while leaving the two masks and byte offset in a three-register rotation.

The accepted source also lifts `normalBit` and `rareBit` into the outer virtual function, in that order before `pItem`, and forwards mutable `u32&` references through the same inline calls. Each assignment retains an explicit `static_cast<u32>` around `GetFormMask`. Together these source controls reproduce the target order: byte offset `r29`, normal bit `r28`, rare bit `r27`, item pointer `r26`. Removing only the normal-bit cast gives 99.85989%; removing only the rare-bit cast gives 99.842384%. Initializing or explicitly recasting the opaque pointer is unnecessary and has been removed. These lifetime and cast controls are D-0016 fake-match constructs, not recovered original helper signatures.

The installed source's LF SHA-256 is `6631c8d1b99e5a0b9e90810504b87c91943463da9270ce5654df3ea5d355bced`. Compared with the baseline, `.text` retains 35,668 bytes and changes exactly 57 bytes in the corrected instruction operands. Exception records, exception indexes, all data sections, all relocation sections, and the symbol table are byte-identical. Only compiler-generated names in `.strtab` differ in addition to `.text`.

The baseline and improved traces are `smallgame/collection-trace.txt` and `smallgame/collection-opaqueouter-trace.txt`. Reproduction uses `smallgame_collection_opaqueouter.py`, `smallgame_collection_maskouter.py`, `smallgame_collection_accept.py`, and `smallgame_collection_simplify.py`. The installed candidate is `smallgame/CDlgCollection/collection_simplify_0/`, with complete compiler log, object, source snapshot, and objdiff JSON. All eleven register/restrict storage-qualifier controls were unchanged at the original 99.66725%.

## Message escape instruction completion

`CMsgRes::ConvertEscape` at `0x801EE530` now compares at 100%, retaining its 552-byte size. The complete object has all fourteen real functions exact; this function is the only score change. The installed source's LF SHA-256 is `af87c1ac08759e9cd6f6f79c9b6c9faaf29f59c273d649d61f6e570ef1024e32`.

The coordinator's full source-link trial passes the frozen executable comparison with zero differing bytes. Its artifacts are in `tools/local/tail-20261104/links/cmsgres-complete/`; `main.dol` reproduces SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Normal `Matching` promotion remains the coordinator's integration step.

The original reconstruction already matched the loop body and every register, but copied the source pointer to the destination before the target's SJIS-result shift and locale-address completion. Ordinary declaration-order, pointer-identity, boolean-type, locale-helper, and optimizer controls did not change that schedule. All tested Wii revisions from `0x4201_127` through 1.7 produced the same 96.57971% result. GC 3.0a3 and 3.0a5.2 regressed to 82.87681%. Private non-POD `FileHeader`, `BlockHeader`, and `DataBlock` declarations did not transfer the successful player-table alias control to this function.

The successful D-0016 control retains an actual compiler `OR` operation whose two operands have identical pointer bits. A local union provides the pointer and `u32` views of `pSrc`; `pDst` receives the OR of those views after the SJIS flag calculation. PowerPC encodes this operation as the same register move required by the target. The union prevents the front end from replacing it with its earlier `MR` pseudo-operation. Delaying the end-pointer assignment until after that expression yields exactly the target order in the pre-allocation scheduler:

```text
lwz data
subi encoding
cntlzw comparison
lis locale
lwz size
addi source
srwi comparison
addi locale
or destination, source, source
add end, data, size
```

This schedule shortens the destination's live range by one interference edge, from degree 36 to 35. With the original declaration positions, it falls below the allocator's 29-register simplify threshold and both pointers move from `r3`/`r4` to `r9`/`r10`. Declaring the existing `char c` before `pSrc` prevents its low-degree node from being removed before the pointer nodes are visited. Both pointers again wait for the later simplify pass and receive the target `r3` and `r4`; every remaining register and the final schedule then agree. This is a measured allocation control, not a recovered original declaration order.

Compared with the control, the 4,732-byte `.text` changes sixteen bytes. The only changed relocation position is the low-half `_current_locale` reference, from section offset `0x106` to `0xFE`, reflecting the corrected address-completion instruction. Exception records, exception indexes, all data sections, every other relocation section, and the symbol table are byte-identical. Compiler-generated local names advance by two in `.strtab`; the indexed relocation targets and their storage are otherwise unchanged.

The accepted source, object, compiler log, full objdiff JSON, and metadata audit are in `smallgame/CMsgRes/msg_accepted/`. `smallgame_msg_orcopy.py`, `smallgame_msg_ordelayed.py`, `smallgame_msg_orlocals.py`, and `smallgame_msg_accept.py` reproduce discovery and acceptance. `smallgame_msg_trace_ranks.py` adds read-only opcode-rank reporting to the existing Frida hook. The final complete trace is `smallgame/msg-accepted-trace.txt`.

### Superseded helper diagnostic

The earlier whole-loop helper reached 98.115944% and appeared to have only register differences. Fresh traces show that it has the same pre-allocation schedule as the baseline. Its apparent final scheduling success comes from the wrong end-pointer allocation to `r0`, which adds physical-register dependencies. Restoring the original named-local allocation also restores the scheduling failure. It is therefore not an independently solved schedule followed by a remaining register problem. The accepted OR control instead changes the actual pre-allocation schedule.

## Earlier failed probes

Before the result above, explicit byte-offset loops, full-range enum offsets, model references, wrapper storage, inline accessors, and same-scope pointer identities did not produce the target ordering. A same-type model-pointer reassignment moved several additional values and scored 99.45709%. The successful void-pointer control required the outer named-local lifetime; placing it only inside the inlined item-model loop failed.

Before the OR result above, local locale accessor helpers, extra inline depth, integer/void identity forms, scalar-reference and class flag lifetimes, and scheduling-factor controls did not solve `CMsgRes::ConvertEscape`. These complement the prior exhaustive whole-loop helper probes in [the CLayout continuation](329-clayout-final.md#cmsgres-follow-up). Explicit locale-component locals restore the target's single component load but retain the bad preheader schedule. Opaque cursor storage, standalone trail-state initialization helpers, volatile pointer views, zero dependencies, and direct `__cntlzw` spellings also failed. No rejected probe was installed.

The initial `CPlayerPii::GetShadowName` probes remained at 88.85714%. Additional scheduling-factor, aggressive load/store scheduling, and prologue-helper controls left that 84-byte result unchanged. The later player owner closed this function using a non-POD table view and a pointer/enum union; the initial failures here are superseded. Prior array, aggregate, ABI-view, and compiler-family investigations remain in [the player tail findings](player-shadow-tail.md).

Before the signature/lifetime result above, fresh private reference-return getter headers and a reference index local reproduced the existing 97.40385% model diagnostic in [the shadow findings](207-cshadowcamera.md#post-window-lifetime-probes). The material-copy store still occurred too early, allowing the texture multiply to reuse `r0` instead of `r5`. Typed color construction, extra material/index temporaries, deferred field-conversion wrappers, and thirty field-reference parameter orders did not close it. Those approaches are superseded by the header-free source now installed.

## Traps

- An existing wrapper with an empty user-defined destructor can match all instructions while adding exception metadata and a weak function. Compare complete object sections before retaining a scheduling-only success. The trivial local color wrapper preserves the control's metadata in this case.
- A pointer identity's static type and declaration position can affect scheduling even when typed-pointer identities are optimized away. The successful void-pointer control is local evidence; its direct transfer to collection failed.
- A stubborn argument schedule can come from the private callee's reconstructed parameter category. A reference color parameter exposes the target's copy lifetime here; pointer ABI equivalence alone did not distinguish it from the former by-value declaration. Retain explicit call-site and metadata evidence when renaming such a project-assigned local symbol.
- Exact instructions and unchanged interim metadata can still preserve an old reconstruction error. The target's two Color cleanup actions established the real class-copy lifetime and superseded the interim GXColor-reference signature.
- Excess retained RTTI strings can reveal an incorrect source-file boundary. Separating Camera's live literal pool from Ground's otherwise-discardable type metadata restores the frozen executable without suppressing RTTI or inserting copied data.
- An opaque pointer's scope matters as well as its type. Collection's direct inner-loop `void*` conversion failed, while an outer named `void*` forwarded by reference preserved the needed register. Explicit same-type casts on two outer scalar assignments then preserved their relative priority. These are measured controls for this inlined loop, not a general allocation rule.
- A register move can have a different compiler opcode. Message conversion retains `OR source, source` through a union representation view; the emitted bits equal `MR`, but its generation position changes the pre-allocation schedule. One named-local move then preserves the pointer nodes' simplify-pass priority. This is a source-only D-0016 mechanism.
- A register-masked exact instruction stream can be a misleading scheduling milestone. The old message helper's wrong `r0` reuse supplied dependencies that disappeared when its registers were corrected. Compare pre-allocation schedules before treating final scheduling and register allocation as independent remaining tasks.
