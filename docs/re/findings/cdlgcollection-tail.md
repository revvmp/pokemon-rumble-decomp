# CDlgCollection complete source link

The 2026-11-04 continuation completes the source linkage of `game/CDlgCollection.cpp`. The earlier instruction work is recorded in [the original unit findings](66-dlg-collection-info-units.md) and [the final Calc reconstruction](20261104-game-small-tail.md#collection-instruction-completion). This pass preserves all 113 exact function bodies and restores their original order, all 96 exception records, and the retained data.

## Cleanup identities

Several identical cleanup bodies had inferred names belonging to different template instances. Their original exception-table callers establish the corrected identities:

| Address | Correct template instance | Original callers |
| --- | --- | --- |
| `0x8007BFC0` | `shared_ptr<CDlgCollectionImpl>` destructor | `CDlgCollection` destructor |
| `0x8007C050` | `shared_ptr<CDlgCollection>` destructor | `Calc`, `shared_from_this`, `Close`, and `Show` |
| `0x8007C0E0` | `shared_ptr<CDlgCollectionItem>` destructor | `Show` |
| `0x8007CE90` | `__vec_split_buffer<shared_ptr<CDlgCollectionItemModel::Model>>` destructor | model `Load` |
| `0x8007CFB0` | `__vec_split_buffer<shared_ptr<IUIListItem>>` destructor | item-vector `push_back` |
| `0x8007E430` | `__vec_deleter<shared_ptr<IUIListItem>, allocator<shared_ptr<IUIListItem>>&>` destructor | item-vector `push_back` |
| `0x8007E500` | `__vec_deleter<shared_ptr<CDlgCollectionItemModel::Model>, allocator<shared_ptr<CDlgCollectionItemModel::Model>>&>` destructor | model `Load` |

The source cleanup type and original exception relocation agree for every occurrence. Original vtable destructor relocations already agree with their source types; no virtual-destructor identity correction is needed. The coordinator applied the inferred symbol corrections before final verification. The read-only audit is `tools/local/tail-20261104/collection_identity_audit.py`.

## Declaration and emission order

Removing the explicit empty destructors of `CDlgCollectionItem` and `CDlgCollection` lets their implicit virtual destructors emit in the original later generation pass. The bodies and exception cleanup remain unchanged.

The unchanged inline definition of `BUI::CCallbackListOwner::Add` belongs after `CDlgCollectionImpl` and before `CDlgCollection` in the reconstructed source. Defining it near the top requests its callback template instances too soon. Moving it to the accepted position reduces the mismatched function positions from 46 to eight without changing any function body. Moving it after the entire source also gives eight mismatches; positions before the item model or implementation class leave 32 or 25, respectively.

The header order then determines six shared-pointer cleanup positions. Keeping `IDlgCollection.h` first and placing `IDlgMsgWin.h`, `IGameMap.h`, and `CUIList.h` in that order restores the target sequence: IUIListItem, BUILayout, callback function, BUI, IDlg, IDlgMsgWin. These header bodies request template instances at include time. Prepending the message-window header before the main interface creates a broader template-order regression.

Two unchanged destructor specializations, after the model class, establish the final split-buffer order: model elements, IUIListItem elements, unsigned shorts. Their declarations name the existing types, and their bodies perform the same reverse destruction and size reset as the generic template. This is a D-0016 source emission control, not evidence that the original library specialized these destructors. Repositioning the existing unsigned-short copy helpers or specializing the unsigned-short buffer itself emits its destructor too early. Specializing the model and UI buffers in the opposite order also fails. The accepted source requires no assembly, object rewriting, injected executable bytes, or custom text sections.

## Exception contract

With all function bodies and positions exact, `CDlgCollection::Show` still differs at one byte of its exception record. At record offset `0x168`, the original range descriptor is `0x000004E0`; the reconstruction used `0x000904E0`. Both ranges begin at function-relative `0x7B4`, the return from the unsigned-short vector `push_back`. The extra nine instructions extend coverage through the return from `CPiiCollectionBox::GetCollectionType` at `0x7D8`.

The exact `GetCollectionType` implementation at `0x800E1C10` only indexes the entries vector and reads `mCollectionType`. A first declaration with CodeWarrior's `__attribute__((nothrow))` allows the compiler to infer the original range. An empty `throw()` specification has no effect. Adding an attribute to set erasure or tree balancing is unnecessary and was rejected from the final change.

The declaration is scoped by `CDLG_COLLECTION_EMISSION_LAYOUT` for this source, leaving existing consumers' exception contracts intact. The accepted private overlay changes only `include/game/CPiiCollectionBox.h`. This is a truthful nonthrowing contract for the proven accessor; the scope controls its adoption by already reconstructed consumers.

## Acceptance

The final direct-compiler candidate is `tools/local/tail-20261104/game_CDlgCollection/collection_all_text_eh/`, containing the source, object, compiler log, and objdiff JSON. All 113 owned functions compare at 100%, occur in the original order, and preserve all 96 exception records including cleanup relocations. The source has been installed as `src/game/CDlgCollection.cpp` with its local emission macro.

The coordinator's native final link in `tools/local/tail-20261104/links/collection-all-text-eh/` exits zero. Its `verification.json` reports PASS, reproducing all 4,094,880 bytes of the frozen `main.dol` with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. ELF, map, DOL, input list, and linker log are retained alongside the result. The coordinator owns production configuration, broad build verification, and Matching selection.

## Traps

- Identical cleanup instructions do not establish a template identity. Compare every original exception-table caller before adding source-order controls; the Collection shared-pointer and vector cleanup names were partly transposed.
- A nonthrowing C++ accessor can extend a caller's exception range when its body is unavailable and its declaration lacks the compiler attribute. Map exception descriptors using return PCs; `throw()` is not equivalent to `__attribute__((nothrow))` in this compiler.
- Header and inline-definition placement can change template emission without changing instructions. Preserve first declarations while testing order, and verify the full executable after function-order comparison passes.
