# main.cpp continuation (#226)

## F-226-1: ownership and complete inventory

This claim owns all 26 original functions / 4,068 instruction bytes from `0x8012D8B0` through `0x8012E934`. First-function owner #225 retains the complete `main.cpp` split, configuration, original anonymous class declaration and final integration. The retained RTTI spelling at `0x803B5F10` is `@unnamed@main_cpp@::CMain`; this continuation introduces no new source-unit boundary. Owners #228 and #229 are recovering later retained helpers from the actual main producers.

| Address | Bytes | Native role / current evidence |
| --- | ---: | --- |
| 8012D8B0 | 24 | Font-map constructor helper; producer pairing pending |
| 8012D8D0 | 16 | Empty font-owner constructor helper; producer pairing pending |
| 8012D8E0 | 32 | Narrow string non-const indexing |
| 8012D900 | 8 | Font producer owner accessor; typed pairing pending |
| 8012D910 | 72 | CResFont constructor; strict code, targets and EH exact |
| 8012D960 | 8 | Font producer owner accessor; typed pairing pending |
| 8012D970 | 120 | Narrow string reverse character search; complete code exact |
| 8012D9F0 | 8 | Font producer owner accessor; typed pairing pending |
| 8012DA00 | 600 | Font-map indexing / insertion |
| 8012DC60 | 8 | Font producer owner accessor; typed pairing pending |
| 8012DC70 | 8 | Font producer owner accessor; typed pairing pending |
| 8012DC80 | 40 | Weak font-map owner construction from shared owner |
| 8012DCB0 | 8 | Strap-reminder owner access from CMain offset 36 |
| 8012DCC0 | 8 | Save-manager owner access from CMain offset 28 |
| 8012DCD0 | 408 | CMain session sequence; strict code, targets and EH exact |
| 8012DE70 | 372 | CMain destructor; strict code, targets and EH exact |
| 8012DFF0 | 136 | PairFont-to-Font shared-owner assignment |
| 8012E080 | 140 | PairFont raw-pointer shared-owner constructor |
| 8012E110 | 352 | Font-map insertion search |
| 8012E270 | 168 | Font-map value-pair destructor |
| 8012E320 | 136 | CResFont-to-Font shared-owner assignment |
| 8012E3B0 | 140 | CResFont raw-pointer shared-owner constructor |
| 8012E440 | 136 | CArchiveFontWithBuf-to-Font shared-owner assignment |
| 8012E4D0 | 140 | CArchiveFontWithBuf raw-pointer shared-owner constructor |
| 8012E560 | 864 | Font-map raw-pointer shared-owner constructor, including dead cleanup |
| 8012E8C0 | 116 | CDisableHBM raw-pointer shared-owner constructor |

The font constructor identities follow their actual deleter vtables: PairFont `0x803B5D70`, CResFont `0x803B5CF8`, CArchiveFontWithBuf `0x803B5C6C`, FontMap `0x803B5958`, CDisableHBM `0x803B58DC`. Reading each table's typeinfo and its original type-name string independently establishes those types. The whole font loader at `0x8012D4A0` supplies these helpers; its natural producer is being reconstructed with #225/#229 rather than fabricated explicit instantiations.

## F-226-2: CMain member and cleanup contracts

Native stack allocation and five producer assignments establish the complete 44-byte CMain layout: IGameLoopCallback vptr at zero; shared owners ISession at 4, CFiber at 12, IDisconMsg at 20, ISaveManager at 28, IStrapReminder at 36. The base has nine virtual functions and no virtual destructor. Its 44-byte vtable at `0x803B5EE4` contains two header words followed by IsRunning, ResetTiming, Update, ShouldRender, Prepare, Calc, Draw, Render, Present. These method names are descriptive, supplied by the established GameLoop interface and owner #225.

The session sequence is ordinary assignment, `Run`, reset for the title session, followed by the same sequence for the game session. It passes `mDisconMsg.get()` to the game-session factory. The old `int pendingMapChange` API is an inferred interface needing correction: native CMain owns an IDisconMsg pointer at 20, the CGameSession factory forwards it unchanged to constructor field `0xC70`, and the selector consumer casts that field's getter back to IDisconMsg before creating its suppression guard. Scratch headers correct only the factory parameter to preserve the actual pointer contract; no canonical interface change is published here.

The destructor explicitly resets the fiber, finalizes color constants, shuts down the icon collection and shuts down the home-button state. Automatic reverse member destruction handles the five owners. The last function's descriptive `ShutdownHomeButton()` name follows the no-argument native teardown at `0x801E40B0`: it destroys the active HBM controller, releases its owners and clears initialization state. No private raw-storage view or hand-written cleanup replaces those member operations.

`C:/MassStorage/GitHub/226-session-control.py` builds the actual member definitions against owner #225's complete declaration, retaining the methods through ordinary diagnostic callers outside the repository. `226-code-audit.py` applies every actual branch and address relocation before comparing all instructions. RunSession reproduces 408 bytes, 14 code relocations, 232 EH bytes and four typed cleanup relocations. The destructor reproduces 372 bytes, all code targets, 416 EH bytes and sixteen typed cleanup relocations. Its cleanup addresses independently confirm all five owner types. These are function-level proofs, not final placement or whole-unit acceptance.

## F-226-3: CResFont and reverse search

The canonical complete CResFont header has the 28-byte nw4r::ut::ResFont base followed by an eight-byte CFileData owner. The ordinary empty constructor reproduces all 72 native bytes at `0x8012D910`, including the call to `0x80295580`, both relocations to vtable `0x803B5F38`, and its eight-byte exception record at `0x8002D414`. The immutable probe is `226-reference/constructor/main.cpp` and its object; `226-constructor-audit.py` verifies exact code and actual targets. Any eventual inline-definition publication remains coordinated with the class header owner and complete current consumer controls.

The canonical string lacks `rfind`. A bounded outside addition uses the existing representation-aware `get_buffer`, an explicit `pos > length - 1` clamp, and backward character-traits equality, returning the all-ones size value when absent. Its entire 120-byte stream equals native `0x8012D970`, including signed character extension and the loop alignment nop. The isolated wrapper only exposes that genuine standard-library algorithm for comparison; its spelling is not substituted for the native template symbol. The actual font-loader producer must retain and select the canonical method during final integration. Additive `char_traits::eq` supplies ordinary character equality. No shared string representation, constructor, flags or registers are changed.

The recovered four bodies total 972 instruction bytes and 656 EH bytes, with strict target-resolved comparisons. The other 22 inventory entries still require actual producer pairing and the complete source-only link. No source selector or split has been changed by this continuation. The bounded pointer-contract prerequisite below changes the interface but adds no accepted main source coverage. Neither #226 nor the enclosing main unit is complete.

## F-226-4: canonical disconnect-message pointer prerequisite

The public IGameSession factory argument, CGameSession constructor argument, stored member at `0xC70`, and slot-40 getter now carry `IDisconMsg*`. The descriptive names are `pDisconMsg`, `mpDisconMsg`, and `GetDisconMsg`; no original method-name claim is made. The selector uses that typed getter directly when constructing its existing disconnect-message suppression guard. No pointer/integer compatibility shim is introduced. The original factory and getter addresses remain `0x800BAC40` and `0x800BABC0`; the original 792-byte constructor remains at `0x800B2040`. Symbols change only to their correct compiler spellings. The CGameSession extent, member order and virtual slots remain unchanged.

The complete native chain proves the contract: CMain receives CreateDisconMsg's shared owner at offset 20, passes its stored object pointer to IGameSession::Create in the exact RunSession body, the factory forwards that pointer to CGameSession, its constructor stores it unchanged at `0xC70`, and the eight-byte getter returns that same word. The selector's existing cast back to IDisconMsg independently confirms the consumer type. CMain retains ownership while each session uses the pointer.

Same-base controls use main `ae91fbb8785d8f350473a0c02f4224690f588033`. Actual `all_source` builds compile all 756 current registered source objects before and after the proposal. `C:/MassStorage/GitHub/226-contract-control.py` compares every section, symbol attribute and raw relocation, normalizing only the intended constructor/factory/getter aliases and numeric compiler labels. 755 objects are whole-file identical; CGameSession.o differs only in the intended symbol spellings. The final ELF has only those normalized metadata changes, with all compared sections unchanged. Fresh default builds after deleting only `main.dol`, and explicit frozen-manifest verification, pass for both baseline and proposal. The final DOLs are byte-identical. Controls are repeated inside the official landing verification after any rebase; the initial report is `226-contract-ae91fbb8-report.json` outside the repository.

## Traps

- **A small reachable body can hide large compiler cleanup.** The FontMap owner constructor at `0x8012E560` has 864 original bytes even though a decompiler prints only its short live allocation path. Its unreachable exception cleanup recursively destroys the tree and pair values; every original byte remains in the inventory.
- **An inlined diagnostic wrapper is not the original member body.** Inlining RunSession into a free wrapper preserved its 408-byte size but changed fourteen stack and cleanup instructions. Retaining the actual CMain member through an ordinary member-pointer reference reproduces every instruction and all 232 EH bytes. Only the actual member proof is accepted.
- **A reference-returning minimum changes this reverse search.** The existing generic `std::min` creates stack slots and grows the stream. The natural explicit bounds clamp produces the native register-only search; no shared algorithm template was modified.
