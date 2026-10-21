# F-227-1: Main-loop continuation producers

Issue #227 retains 25 functions and 3,908 code bytes at `0x8012E940`–`0x8012F8F4`. The first-function owner of `main.cpp` is #225; #226, #228, and #230 own neighboring original inventories. No new source split or shared class header is proposed by this continuation.

The independently established `@unnamed@main_cpp@::CMain` contract is 44 bytes: `IGameLoopCallback` at zero, then shared owners of `ISession`, `CFiber`, `IDisconMsg`, `ISaveManager`, and `IStrapReminder` at offsets 4, 12, 20, 28, and 36. The native virtual methods appear in reverse interface declaration order, consistent with inline definitions and the established game IPA context.

| Native address | Method | Native size | Current producer |
| --- | --- | --- | --- |
| `0x8012EBD0` | Present | 4 | Exact instructions and direct target |
| `0x8012EBE0` | Render | 268 | Exact instructions and direct targets |
| `0x8012ECF0` | Draw | 152 | Exact instructions |
| `0x8012ED90` | Calc | 152 | Exact instructions |
| `0x8012EE30` | Prepare | 120 | Exact instructions and direct target |
| `0x8012EEB0` | ShouldRender | 160 | Two prologue instructions exchange order |
| `0x8012EF50` | Update | 244 | Exact instructions and direct targets |
| `0x8012F050` | ResetTiming | 8 | Exact instructions and direct target |
| `0x8012F060` | IsRunning | 8 | Exact instructions |

Eight methods reproduce 956 original code bytes. All 23 direct branch relocations across the nine methods resolve to the original targets. All six eight-byte exception records reproduce the original bytes. These are outside producer results, not whole-unit or issue acceptance. Both Wii/1.1 and Wii/1.3 retain the same two-instruction `ShouldRender` difference. A raw-pointer test removes the original shared-owner boolean conversion and is rejected.

`Render` draws the active map's pause menu through `IGameMap::GetPauseMenu`, whose native virtual slot is `0x58`. `Present` tail-calls `Graphics::DisplayFrameBuffer`; its decompiler view incorrectly appeared empty. `Update` gives the disconnection message priority over the pause menu and always processes the save manager and pending reset request afterward. Callback and subsystem names without recovered original spelling remain descriptive.

## F-227-2: Retained template identities

The native font map is `std::map<std::string, std::tr1::shared_ptr<nw4r::ut::Font>>`, as proved by the original RTTI string. Its insertion producer at `0x8012F080` allocates 32 bytes and constructs the 20-byte pair at node offset 12. The current canonical insertion producer has the native 600-byte extent with five differing instruction words; the tiny diagnostic's recursive destruction producer is 1,040 bytes versus native 912 and needs the actual complete translation-unit context. No map declaration changes follow from this diagnostic.

The clone functions at `0x8012F850` and `0x8012F8A0` store vtable `0x803B52E0`, identifying the plain callback `void (*)(const CLayout::CInitializeException&)`. Its invocation at `0x8012F8E0` therefore returns void and tail-calls the stored pointer. It is distinct from the neighboring HBM callback returning bool, whose invocation normalizes the result.

Consuming #228's outside bounded functional overlay reproduced eight isolated callback bodies totaling 588 bytes, including all 11 relocation sites. The isolated diagnostic incorrectly associated the zero-argument function-base destructor with `0x8012EAA0`: identical destructor bytes did not establish specialization identity. The complete producer and original vtables below supersede that provisional association. These diagnostics do not substitute for emission in the actual complete `main.cpp` producer or full frozen verification.

## F-227-3: Main owner archive dependencies

At #225's request, two original #225 helper producers were traced outside the repository to unblock the complete translation unit. They do not extend #227's original inventory or ownership.

`0x8012D310` returns `shared_ptr<FileList>` by hidden result pointer. It builds seven local eight-byte requests, each containing a filename pointer and a zero `u32` flag word, then calls `0x80128600` with count 7 and background true. That four-byte thunk branches to the batch loader at `0x801A6510`, which produces a shared list of successfully loaded files. This API is distinct from the existing five-argument path-expansion `LoadResourceFileList` interface. Descriptive outside names are `ResourceFileRequest`, `LoadResourceFiles`, and `LoadStartupArchives`; original spellings remain unknown.

The seven paths are `5:/pii_1-151_387-492.arc`, `5:/item.arc`, `5:/game2d.arc`, `5:/font.arc`, `5:/effect/main.breff`, `5:/effect/main.breft`, and `5:/sound/waza_p.brsar`. Ordinary aggregate initialization reproduces all 176 native function bytes after resolving five relocations, the 56-byte constant initializer at `0x8035BBE8`, all seven string payloads, and its eight-byte exception record.

`0x8012D3C0` appends one `const shared_ptr<CFileData>&` to a `FileList`, using a 16-byte list node with the shared owner at offset 8. There are no additional registration effects. The natural `rFiles.push_back(rFile)` producer reproduces its native 224-byte extent, allocation/deallocation targets, and eight-byte exception record. Two stores of a temporary allocation pointer at function offsets `0x44` and `0x48` exchange order; Wii/1.3 retains that difference. The diagnostic therefore establishes semantics and types but does not claim complete byte acceptance for this helper.

## F-227-4: Complete combined producer audit

The actual combined startup, session, runtime, and font producer emits all 25 original #227 functions without diagnostic retainers. A snapshot is identified by object SHA-256 `6f289fa2a7b0b42260b21f3bcf8bbf86ee53f7ce7592781c2f9831ed95688dbd`. Its independently audited results are 22 functions and 2,236 original code bytes exactly reproduced, including all 41 relocation sites in those functions. No retained function is missing.

| Native function | Code remaining | Exception data remaining |
| --- | --- | --- |
| `0x8012EEB0`, ShouldRender | Native size 160; two prologue instructions exchange order | Exact 8 bytes |
| `0x8012F080`, font-map insert_node_at | Native size 600; five differing words | Candidate 104 bytes versus original 96 |
| `0x8012F2E0`, font-map destroy | Candidate 1,040 bytes versus original 912 | Candidate 104 bytes versus original 40 |

The inventory has 20 exception records totaling 296 original bytes. Eighteen records and 160 bytes reproduce exactly; the two map records above remain unaccepted. The separate #225 append helper still has its two temporary-pointer store-order differences in the combined context, with its targets and exception record exact.

Original virtual tables establish the three retained function-base destructors: HBM bool at `0x803B5888` points to `0x8012E990`; path-error callback at `0x803B54D0` points to `0x8012EAA0`; layout-exception callback at `0x803B5398` points to `0x8012EAE0`. The preceding `0x803B54C4` is a base-RTTI array, not the bound-main callback vtable. The bound-main callback vtable is `0x803B5548`, whose destructor is outside #227 at `0x801316C0`.

The original entry-point exception record independently identifies the remaining destructor specializations: offsets `0x10C`, `0x11C`, and `0x12C` refer to the layout callback destructor `0x8012F710`, path callback destructor `0x8012F7B0`, and `ResourceFileConfig` destructor `0x8012EB20`. Candidate record relocations reproduce those original typed associations. This audit reports producer evidence, not complete issue acceptance or frozen-build verification.

## F-227-5: Bounded ShouldRender source-context controls

Two additional faithful controls were compiled against a copied complete producer with the same compiler arguments and copied outside header inputs. The baseline nested `if` was compared with `if (mDisconMsg && mDisconMsg->IsActive())`, and with a local `IDisconMsg* pDisconMsg = mDisconMsg.get()` used only for the virtual call while retaining the shared-owner boolean test. All three emitted the same 160-byte method and the same two prologue differences: the candidate stores LR at offset `0x08` and loads the member at `0x0C`; the original loads the member first and stores LR second. Neither source control fixes the gap. No flags, attributes, shared headers, or #225 files were changed.

## F-227-6: Single Wii/1.0a discriminator

Following F-735-1's compiler-version evidence, the copied complete producer was compiled once with only the Wii/1.0a compiler executable substituted for Wii/1.1. Canonical flags and copied outside headers were preserved. Compilation succeeded; no configuration changes followed.

All nine main-loop callback instruction streams and extents were unchanged. `ShouldRender` remained 160 bytes with the same two prologue differences. `Run` remained 1,488 bytes and `LoadFonts` remained 1,028 bytes, with no non-relocation instruction changes relative to the Wii/1.1 control. Font-map insertion remained 600 bytes with a 104-byte exception record, and recursive destruction remained 1,040 bytes with a 104-byte exception record; the original cleanup sizes are 96 and 40. The append helper regressed from the native 224-byte extent to 220 bytes. This single version control is negative and does not justify a compiler change or further version sweep.

## Traps

- The source methods are inline in the original class context: individually exact out-of-line bodies alone do not reproduce physical function order. Preserve the native reverse-interface order in the complete producer.
- Identify callback invocation types through the actual vtable and RTTI before interpreting a short indirect tail call; the exception callback and HBM bool callback have different return contracts.
- Native font-map allocation is 32 bytes with value offset 12. Treating the allocator's value pointer as a node pointer would incorrectly suggest a 36-byte node.
- Identical small destructor bodies do not identify a template specialization. Decode the original vtable's RTTI and confirm its start before associating an address; a base-RTTI array can precede the real vtable.
