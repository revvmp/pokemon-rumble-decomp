# F-45-1: BWorld long-tail reconstruction

The 2026-11-04 baseline has 191/196 real target functions exact. Direct Wii/1.1 compilation with the configured flags and original split comparison now gives 194/196, with no previously exact function regressing. BWorld remains NonMatching pending full source-link acceptance.

## Retained D-0016 fake matches

- `std::vector<BWorld::ObjEntry>(size_type)` at `0x8005B950`: a TU-local `__vec_constructor` specialization forwards through `InitializeWorldEntries`, then separate `AllocateWorldEntries` and `ConstructWorldEntries` helpers. The combined extra inline levels reproduce the empty-tag slots, exception object stack slots, and element pointer register. A single wrapper, or separate allocation/construction wrappers without their enclosing wrapper, did not match.
- `BWorld::AddObject` at `0x8005F230`: a pointer-to-u32-to-pointer conversion makes the entry-array base survive as a named register-allocation value; declaring the entry pointer before the base pointer gives the target register order. The target is 32-bit, so the round trip preserves the pointer value.
- `BWorld::RemoveObject` at `0x8005F450`: calculate next before previous, with the same pointer round trip on next; keep the ground-identity comparison in a named bool. This reproduces evaluation order and both register allocation choices without altering behavior.

The AddObject/RemoveObject conversion shapes are allocation fakes; no original source form is claimed. No shared header or generic STL change is retained. Direct evidence is under ignored `tools/local/session/45-world/combined3.json`.

## Remaining Calc and CalcMtx

Both inline GetSkipStates with one missing redundant zero load: target initializes a temporary with zero inside the first flag branch, then ORs bit 2 into the output. A redundant `states = 0` in that branch restores the instruction count but uses the output register for both instructions (CalcMtx 99.94%). Scalar casts, integer widths, helper wrappers, local state structs, full-width bitfields, repeated conditions, and caller optimization pragmas have not closed this. Calc additionally differs in one later loop iterator register; declaring a fresh iterator in the inner while-loop for initializer fixes that difference (combined with redundant zero assignment, Calc reaches 99.979%, leaving only the same two zero/OR register operands as CalcMtx). No incomplete probes were retained.

## Traps

- A pointer round trip can change which named local survives optimization despite producing no instructions. Once it survives, declaration order controls relative allocation; adding the cast alone swapped the wrong two AddObject registers.
- Merely assigning a comparison to a named bool can move the result from an inline temporary to the target register. Widening the same result to int/u32 did not work in RemoveObject.
- Stack parity fixes require examining every frame object, not just the empty tags: BWorld vector(size_type) needed an outer forwarding helper and separate inner allocation/construction helpers together.

## 2026-11-04 enum zero-value identity

The CPauseMenu enum-local result closes both remaining BWorld functions. Inside the first GetSkipStates branch, a redundant zero assignment followed by a const BWorldObj::StateFlag view of that zero preserves a distinct value for the target's zero-load/OR pair. The branch then combines that typed zero with the existing STATE_PAUSE_FOR_UI enumerator. The source enters this first branch with states already zero, so behavior is unchanged. Unlike a u32 temporary, the enum value does not coalesce into the result register. The existing public state enum avoids inventing another type.

Calc also uses the previously diagnosed fresh iterator declaration in the inner for loop, instead of reassigning the earlier loop's iterator. Isolated stateexisting0 compilation measures all 196 functions at 100%, with no previous exact function lost. The narrow shared-header edit was coordinated and approved by the parent agent, then applied with the source iterator edit. This redundant assignment/typed-zero construct is a D-0016 fake; it is included in the same ledger scope as the existing BWorld allocation/order adaptations.

Whole-unit native linking is not yet accepted. The current source still has weak-function emission-order differences and 208 non-relocation .data bytes that need ordering work. Individual 196/196 function coverage does not close that link requirement.

## 2026-11-04 source emission reconstruction

The next isolated candidate has all 196 target functions exact, in complete target order, after ten evidence-backed symbol mappings. Every exception-table record and cleanup relocation also matches after these mappings. This is object evidence only; native source-link acceptance is still pending. Scratch evidence is `tools/local/session/world-tail/finalcandidate.o`, `finalcandidate-target.o`, and `finalcandidate-renames.json`; the target copy was renamed only for diagnostic objdiff comparison and is not a build input.

The DrawShadow signature was incorrect. Draw's original exception table protects a temporary `nw4r::ut::Color`, whereas the reconstructed GXColor-by-value declaration generated no cleanup. Changing the private parameter to Color restores both exact code and the missing exception record. Simply changing the local black variable, or passing an explicit Color temporary to the old GXColor signature, changes stack allocation and does not match.

The retained emission adaptations are restricted by BWORLD_EMISSION_LAYOUT:

- Move the inline LoadFile definition before its LoadLightScene caller. Moving only its first declaration or adding inline at its old definition did not restore the weak-function pair.
- Explicit sizeof owner/weak-owner type views establish the instance order without emitting runtime data or instructions.
- Delay EffectData's full nested definition until after BWorld's declaration. Other translation units retain the original header view.
- Make the pointer/integer vector storage base dependent through a trait whose result remains void*. The original nondependent void* base eagerly instantiated cleanup helpers while reading vector; this was why moving GetObjects and adding in-class sizeof triggers had no effect. The dependent base restores both the plain vec_deleter pair and the final split-buffer groups without changing their layout or instructions.
- Use the existing CFileData map_leaf_search pattern to delay the compact map's leaf-search body. Its original algorithm is retained, with a scoped inferred-symbol rename.
- Use one world_owner_guard template for the seven owned default/custom cleanup helpers. Value/default deleters and reference/custom deleters retain their original compressed-pair layouts. The default constructor must keep its templated pointer parameter and unused nat argument: dropping those changed eight callers' stack allocation. The char-array custom deleter retains shared_ptr_holder through a type trait, since its cleanup is owned by another unit.
- Delay creation of shared_ptr<CEffect> through a source-local WorldEffectOwner result view for unused CEffect weak_ptr::lock/shared_from_this APIs. The view derives from shared_ptr<CEffect> and preserves shared ownership through its conversion constructor; no such API is called or emitted in BWorld. AddEffect is a source-local member-template specialization so its declaration also does not create that owner instance early. Both changes are necessary: removing either early type-use preserves the wrong owner cleanup order. A function-template lock alternative moved lock bodies into the middle of strong code and was rejected.

These emission views, explicit instantiation triggers, unified cleanup guard, dependent vector base, and delayed owner-result view are documented D-0016 fakes. They preserve executed behavior and compile real C++ bodies; no original bytes or assembly are substituted. DrawShadow's Color parameter is a type correction supported by exception metadata rather than a fake.

### Cross-unit cleanup identity

The first native trial exposed two external owners of BWorld's inferred cleanup names. Renaming the AnmScnRes and ScnRoot holder destructors caused BWorldLightSet and CPostEffect to retain fresh 108-byte weak copies under the old names, adding 0xE0 aligned text bytes. Returning BWorld to fallback did not remove them, because the symbol-name changes still changed global weak resolution.

WorldOwnerGuard.h now shares the identical cleanup implementation. BWorldLightSet and CPostEffect use source-local constructor specializations only for AnmScnRes and ScnRoot with their G3dObj deleter. Isolated comparison confirms their existing instructions, owned function order, and exception records remain unchanged; only those cleanup relocations select the renamed guard. All other ownership, including char-array cleanup, retains its existing helper. Native acceptance remains pending this correction.

## Accepted native source link

After the two external ownership specializations, the parent ran the real shared build with BWorld marked Matching. Ninja exited 0 and the frozen-manifest verifier passed against the newly linked DOL. BWorld's 196/196 source bodies, exact emission order, original exception metadata, and linked data are accepted. The original hashes were not changed. Parent broad comparison checks shared-header regressions separately.
