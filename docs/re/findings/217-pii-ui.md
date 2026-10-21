# Pii user interfaces

## F-217-1: Scope and native player contract

Issue #217 inventories 29 functions and 9,180 instruction bytes at `0x8011D7E0`–`0x8011FC78`: six CUIPiiListItem functions, nineteen CUIPiiSpec functions, and four CUIPlayerID functions. Interface prerequisites do not reconstruct or close that inventory.

CPlayer is an original RTTI name. Its native constructor at `0x800F7840` and allocation caller at `0x800B21B4` establish a 116-byte ICalc subclass, rather than the former method-only declaration. Its vtable at `0x803A9320` contains the destructor, Calc, and inherited CalcMtx, in that order. The 12-byte base list at `0x803A9334` names ICalc; the name and RTTI occupy 16 bytes at `0x804B7108`.

| Offset | Type and descriptive member | Evidence |
| --- | --- | --- |
| `0x00` | ICalc base | Constructor and vtable |
| `0x04` | `u32 mFlags` | Constructor and Calc flag tests |
| `0x08` | `int mIndex` | Constructor argument |
| `0x0C` | `IController* mpController` | Native GetController at `0x800859B0` |
| `0x10` | `CState mCurrentState` | Native state constructor and IsJoined |
| `0x38` | `CState mSavedState` | Second constructor and reverse destructor order |
| `0x60` | `weak_ptr<CPlayerPii> mPii` | Native weak-pointer constructor/destructor calls |
| `0x68` | `u32 mReservedInput` | Four-byte zero initialization; additional meaning unresolved |
| `0x6C` | `u32 mHeldButtons` | Player Calc and script GetPadHold |
| `0x70` | `u32 mPreviousButtons` | Player Calc and script GetPadTrig |

Each 40-byte CState has a bool at zero, a CPiiBox at four, and shared pointers to selected and entrance CPiiPersonalData at `0x18` and `0x20`. The selected pointer is changed by SetPii; the entrance pointer is cleared independently. State/member names are descriptive. The native state constructor is `0x800F77E0`; its destructor is `0x800F78D0`.

CPiiBox is 20 bytes: two integer constructor arguments at zero and four, then `list<shared_ptr<CPiiPersonalData> >` at eight. Constructor `0x800E0200` initializes a zero-size, self-linked list. Destructor `0x800E0220` releases the shared pointer in each 16-byte node before freeing it. The integer names index and capacity are descriptive; CPlayer constructs its state boxes with `(0, 1200)` and CRemoteMemory uses `(2, 12)`.

An ordinary external producer compiled with the established Wii/1.1 game flags reproduces six native functions, 476 instruction bytes: CPlayer constructor/destructor, CState constructor/destructor, GetController and IsJoined. Every relocation target is resolved to its independently identified native address. Four exception records total 176 bytes, and all native vtable/base-list/name/RTTI bytes match. Native getter instructions alone cannot establish const qualification. CPlayer/CPiiBox publication is held for the existing first-source owner's draft #1107, discovered by the overlap check; the independently recovered descriptive member names are not competing canonical declarations. No outside function bodies or split changes are published by the interface prerequisite.

The original executable SHA-256 for these comparisons is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Reproducible external probes are retained under `217-reference`, with `217-player-native-proof.py` and its JSON receipt.

## F-217-2: Remote memory and icon contracts

CRemoteMemory's 64-byte allocation, constructor `0x80106770`, destructor `0x80106880`, read completion `0x80106D70`, write preparation `0x80107CA0`, and write completion `0x80107EB0` establish its complete member layout. The flags word at zero supplies IsBusy's bit-zero test. Source box and channel occupy four and eight. A list of shared Pii personal data starts at `0x0C`; the remote CPiiBox starts at `0x18`. Retry and wait counters are integers at `0x2C`/`0x30`. Transfer size, transfer offset and block offset are unsigned shorts at `0x34`/`0x36`/`0x38`; the active block is a byte at `0x3A`. The write-mode word is at `0x3C`. Write preparation selects offsets zero or 536, lengths four or 536, and a 544-byte block stride. Counter, transfer and mode names are descriptive. Its complete header proposal is held until the owner's CPiiBox declaration is canonical; the existing WaitUntilIdle declaration remains unchanged.

IIconCollection is an original RTTI name. Its concrete anonymous-namespace class embeds the original `CIconCollection_cpp` filename and has a 468-byte allocation: its vptr and two 232-byte CLayout members. Initialization at `0x800C0490` loads `icons_ct` and `icons_button`. GetIconCollection at `0x800C0610` returns the singleton.

The concrete vtable at `0x803A4880` has exactly four method slots and no destructor slot. They return the native 28-byte `nw4r::lyt::TexMap` by value: controller icon for type/channel (`0x800C0930`), controller icon for controller (`0x800C0850`), button icon for type/button (`0x800C0740`), and button icon for controller/button (`0x800C0680`). Method names and pointer const qualification are descriptive inferences. Pointer overloads obtain the type/channel through IController; native Wii Remote handling also inspects the complete CControllerWiiRemote contract. A virtual destructor would shift every call and is incorrect.

CUIPlayerID Update is an independent consumer of these contracts. Its reconstructed 696-byte body at `0x8011F9C0` reproduces every instruction and relocation target, including by-value texture returns, the remote busy flag, player controller and joined-state accesses, layout calls, pane strings, and message identifiers. Together with its 76-byte constructor and 88-byte destructor, the proof covers 860 instruction bytes and three exact eight-byte exception records. The strict receipt is retained as `217-reference/217-ui-native-proof.json`; this is a native contract proof, not source-selection credit.

## F-217-3: Embedded layout accessor

Native BUILayout::GetLayout at `0x8008B7D0` consists of adding `0x4C` to this and returning. The complete shared BUILayout header already places its CLayout at that offset. Returning `&mLayout` gives the natural inline definition. The first-source owner of the BUI/CUIList work delegated this bounded prerequisite, which is also required by CUIList Load and OnFocus.

## Traps

- Native eight-byte getters prove accesses but do not distinguish pointer/member const qualification. Preserve the canonical GetController API rather than renaming its symbol from that evidence alone.
- CUIPlayerID Initialize remains 276 bytes against 264 native bytes. Its native short-string buffer address is hoisted across the four-iteration pane loop, while the current standard string accessor rematerializes it. Constructor, destructor, and Update remain exact. Do not add a retention-only pointer local, forced register, or speculative compiler flag to conceal this scheduling residual.
- IIconCollection has no virtual destructor. Adding a conventional destructor shifts the four evidenced slots and breaks the by-value texture consumer contract.

## F-217-4: Current-consumer prerequisite control

The initial combined header candidate was compared against exact main `df7d203100d2c03d3ceb4728d39b4bf9db14c134`. Both sides ran the real configure, all_source, default Ninja link, and frozen-manifest verification, with the old DOL removed before linking. All commands exited zero. The all_source graph contains 734 actual source objects. This first control includes the subsequently held player/box/remote headers; the smaller published candidate is controlled independently before merging.

730 objects are whole-file identical. CDlgPiiSelector, CUIList and ScriptCamera retain every allocated section byte, semantic relocation and named symbol attribute; only compiler-generated numerical labels change. CMsgBalloon retains every existing function's raw instructions and every relocation target/addend, with offsets compared relative to their owning functions. Its sole added function is weak BUILayout::GetLayout, exactly `addi r3,r3,0x4C; blr`, eight bytes plus alignment within a 16-byte text increase. All other allocated sections remain raw-identical. All four affected units are already NonMatching and retain their original link selection. No reconstructed source count is credited here.

The complete linked ELF is raw-identical, SHA-256 `ebd2d88abfc1d6856446b39eff536a2c1cd43d6bc521db8b1d919e490d8e1fe1`; the DOL remains the frozen `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. External scripts `217-header-control.py` and `217-header-object-audit.py` retain the baseline objects and receipts and reject unexplained instruction, allocated-byte, relocation, symbol-attribute, ELF or DOL changes. The official merge wrapper repeats these controls after each rebase, in addition to the normal land verification.

The narrower published IIconCollection/GetLayout candidate was then independently rebuilt against the same base: all 734 source objects, 732 whole-identical. Only CUIList numerical labels and the same CMsgBalloon weak getter addition differ. The complete allocated-section attributes, existing instructions, semantic relocations, named symbol attributes and visibility pass independently; ELF and DOL retain exactly the hashes above. No player/box/remote header change is needed for this result.

The native CPlayer and CUIPlayerID producers are external contract probes. They supply layout, virtual-order, by-value return, target and EH evidence; their bodies and outside unit splits are not part of this prerequisite. CPlayer.h, CPiiBox.h and CRemoteMemory.h are not changed by the published prerequisite. Draft #1107 supplies the first owner's complete player/box representation and #1106 adds player hitstop methods; both are preserved as independently owned work. #1107's proposed CPlayer header currently omits main's nonconst GetController declaration. That declaration and const IsJoined must remain available when the owner proposal integrates because existing and reconstructed consumers call them. Issue #217 remains open for its complete 29-function inventory.

dtk automatically relocates the unchanged CUIList split block after CRomFont to restore address ordering. Its first-source owner approved retaining this ordering-only normalization; no split range, size or attribute changes.
