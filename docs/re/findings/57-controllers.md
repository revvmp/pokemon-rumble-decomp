# F-57-1: Controller interfaces and native unit boundaries

Issue #57 owns 26 original functions, 5,300 instruction bytes, from `0x8006E6A0` through `0x8006FBE4`. The preceding issue ends with an unrelated destructor; the following issue begins `CDisconMsg`. RTTI, vtable targets, exception records and independent controller-global constructors establish five complete classes in this interval.

| Class | Native text | Object size | Additional state |
| --- | --- | --- | --- |
| `CControllerClassic` | `0x8006E6A0`–`0x8006EA50` | `0x34` | None |
| `CControllerComposition` | `0x8006EA50`–`0x8006F0B8` | `0x40` | `std::vector<IController*>` at `0x34` |
| `CControllerGCPad` | `0x8006F0C0`–`0x8006F350` | `0x34` | None |
| `CControllerNunchaku` | `0x8006F350`–`0x8006F5C8` | `0x34` | None |
| `CControllerWiiRemote` | `0x8006F5D0`–`0x8006FBE4` | `0x38` | Classic-input selection Boolean at `0x34` |

The class spellings are original RTTI names. Their interfaces override the existing `BController` virtual slots without adding virtual slots. Method and field names are descriptive. The Wii Remote constructor calls `BController()` and clears its additional Boolean. The composition vector has data, size and capacity words at `0x34`, `0x38` and `0x3C`. Its assignment method at `0x8006EC10` checks self-assignment, grows using the same three capacity regimes as the existing MSL vector, copies pointer elements with `memmove`, and updates size.

`GetControllerTable()` at `0x80124A50` returns the composition object at `0x803E9088`, also returned through the existing `GetUIInput()` interface. `IsExtensionControllerEnabled()` at `0x80124970` reads the Boolean written by `0x80124960`. Native initialization at `0x801248C0` sets `BController::mChannel`, offset 4, for all sixteen entries in the four-by-four controller table. The additive `SetChannel(s32)` setter exposes that existing field without altering object layout.

The free `GetControllerDisconnectState()` at `0x8006FA00` walks the composition vector. It returns the maximum state across non-null controllers: 0 connected, 1 disconnected GameCube, 2 disconnected Nunchaku with its Wii Remote still connected, 3 disconnected Classic with its Wii Remote still connected, 4 disconnected Wii Remote or missing parent Wii Remote, and 5 unsupported extension while the separate extension-controller mode is disabled. The original returns a signed integer and materializes temporary integer arguments for a reference-taking maximum operation.

## Motor pattern record

`ScriptBinding_StartRumble`, `0x80151F70`–`0x801520E4`, converts the script pattern index with the existing signed `CValue::ToInt` and selects a record from `0x803550B8` with stride 12 before calling controller virtual slot 9. The three records are `{0, 0x804B7728, 0x804B772C}`, `{1, 0x804B7730, 0x804B7734}`, and `{2, 0x804B7738, 0x804B773C}`. The first word is the sequential pattern index. Its signed `int` declaration follows the script index; only nonnegative stored values are observable here, so signedness is not independently distinguished by those constants.

`CControllerWiiRemote::StartMotor` passes the pointer at offset 4 to the existing typed `StartWiiRumble(int, const RumbleStep*)`. `CControllerGCPad::StartMotor` passes offset 8 to `StartGCRumble`. Consequently `MotorPattern` is a complete twelve-byte record with one index and two typed pattern pointers. No opaque padding or guessed payload is required.

## Reconstruction control

Unchanged `GAME_COMPILER` Wii/1.1 and `cflags_game` reproduce all five Classic functions, all five GameCube functions, all three Nunchaku functions, all six Wii Remote member functions and the disconnect-state query: twenty original functions. The four concrete units have been linked from source together and the full frozen DOL passes. The subsequent controls in F-57-2 and F-57-3 reproduce five composition function bodies; update remains incomplete, so this finding does not establish completion of the original issue.

The native Boolean stages in the GameCube and Wii Remote input checks correspond to nested readability, connection and processing predicates. Collapsing all stages into one expression shortens the routines. The Classic button mapper declares the native hold word before its result and compares positive stick thresholds with the constant as the left operand; the equivalent reversed comparison selects different floating comparison instructions.

## F-57-2: Const motor interface control

The native composition motor loops only read their controller collection. Declaring `StartMotor(const MotorPattern&) const` and `StopMotor() const` across `IController`, `BController`, composition, GameCube and Wii Remote reproduces their complete 120-byte and 104-byte instruction streams under unchanged Wii/1.1 game flags. The former non-const declarations left five and three differing prologue instructions respectively. All five GameCube functions and all seven Wii Remote functions retain their exact native instruction streams with the corrected interface. This independently checks the concrete overrides rather than inferring const qualification from the empty base implementations.

The correction changes mangled method names at the existing eight native addresses, preserving their sizes, virtual slots and object layouts. Composition update and pointer-vector assignment remain incomplete; this interface control does not complete issue #57.

## F-57-3: Pointer-vector assignment context

The complete 756-byte `SetControllers` instruction stream is reproduced by ordinary vector copy assignment. The constructor layer delegates allocation to the existing deleter layer, whose allocator size check and allocation path match both native exception paths. The internal pointer-copy helper receives its iterators by const reference. Its signed element distance feeds `memmove`, while assignment separately preserves the requested element count across allocation.

With value iterator parameters in that copy helper, Wii/1.1 combines the two distance calculations and eliminates the retained end iterator, producing 736 bytes. Const reference parameters preserve the native beginning and ending iterators and their two distance calculations. Using the duplicate constructor-layer allocator instead leaves four non-relocation differences in the second exception path; delegation to the existing base allocator resolves those too. These are source-level controls under unchanged game flags.

Current object controls reproduce five of the six composition functions, 1,172 of its 1,596 instruction bytes. Update remains 424 bytes with four reordered instruction positions near its start, three of which are reported by the non-relocation comparison. The complete issue remains open. The four concrete controller units remain source-selected together and the full frozen DOL passes with the proposed vector additions; composition remains `NonMatching`.

## F-57-4: Concrete controller source acceptance

The four complete concrete units reconstruct twenty original issue functions and 3,704 instruction bytes: Classic five/920, GameCube five/648, Nunchaku three/612 and Wii Remote seven/1,524. Every final ELF function at its original address and size compares byte-for-byte with the frozen original DOL. A fresh link MAP attributes each function uniquely to its compiled controller source object, and the full DOL verifies with all four units source-selected together. Composition remains the original fallback in the selected graph; its five exact function-object controls are not credited as linked source.

The GameCube glyph pool at `0x8039B0E0` contains six twelve-byte wide literals, ending at `0x8039B128`. The previous metadata represented it as one opaque 72-byte object. Identifying the six observed wide strings resolves objdiff's interior-symbol annotation discrepancy on two identical `addi` instructions: all five GameCube functions now report 100%, matching the independently exact final instruction bytes. The literal contents and addresses do not change.

## F-57-5: Pointer-vector iterator construction context

The pointer specialization now models iterators as four-byte classes with a pointer member, a constructor receiving the pointer by const reference, and an implicit trivial copy constructor. A corresponding const iterator preserves const access to pointer elements. Under unchanged Wii/1.1 game flags, this construction context reproduces all six composition function bodies, including every relocation target: 1,596 instruction bytes. The former raw pointer typedef reordered the initial collection load and zero stores in the 424-byte update routine. An explicit copy constructor changes the return ABI and is not equivalent.

An independent consumer control in issue #219 uses the same header overlay: factory 432 bytes and all twelve targets, its 136-byte exception record and five destructor sites, Draw 436 bytes and all 32 targets, and comparator 112 bytes match. The 2006 committee paper [N2103, page 7](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2103.pdf) documents Metrowerks vector iterators as either raw pointers or a debug iterator representation. It supports the representation family, not this exact inferred declaration or spelling. No original pointer-vector header has been recovered.

This is a bounded shared-header prerequisite. Composition remains NonMatching until its complete source-linked data and exception tables pass the frozen DOL gate; exact function objects alone do not establish original issue completion.

## F-57-6: Complete original controller inventory

All 26 functions and 5,300 instruction bytes in the original issue inventory are source-linked together across the five controller units. Composition contributes the remaining six functions and 1,596 bytes. Every final ELF function has its original address, size and bytes, with one corresponding compiled-source row in a freshly generated MAP. The selected link graph contains each controller source object and excludes its original fallback. The complete rebuilt DOL equals the frozen original byte-for-byte, including the retained exception tables, literals, RTTI and vtables.

The native cleanup destructor at `0x8005A9F0`, 96 bytes, is `std::tr1::detail::shared_ptr_holder<char, std::detail::default_delete<char[]> >::~shared_ptr_holder`. Its source body and both delete targets match the native routine. The composition assignment exception record references this same cleanup guard twice. Naming that existing outside symbol lets the linker resolve the emitted weak duplicate to its original instance; leaving it unnamed instead retains an extra destructor and exception record in the controller unit. Its address, size, split and implementation ownership remain unchanged.

The complete control runs `configure.py --map`, a fresh-output `ninja` and the frozen-manifest verifier, each exiting zero. `unitdiff.py` reports all six composition functions at 100%. The immutable native26 audit also checks the original issue REST inventory, source selection, unique MAP coverage and frozen DOL hash. No compiler option, original manifest or executable byte was changed to obtain acceptance.

## Traps

- A seeded `main.elf.MAP` can be stale even after a successful current link. Default `configure.py` does not enable map generation. The first controller control had a current ELF with source symbols while its seed MAP still named original fallback objects. Generate with `configure.py --map`, relink and verify before using a MAP to attribute source coverage; inspect timestamps and actual symbol entries. Never infer source attribution from an inherited map.
- Composition scratch controls are not accepted source. The former raw pointer iterator gives a 424-byte update with four reordered instruction positions, three visible in the non-relocation comparison; F-57-5 resolves that object mismatch with a class iterator. The former motor-loop differences are resolved by F-57-2, and pointer-vector assignment is resolved by F-57-3. Wii/1.3 gives the same update residuals as Wii/1.1. Ordinary initialization forms, const controller pointers, early-continue guards and getter access leave this residual unchanged.
- The unresolved F-735-1 candidates were checked externally on this source, without changing repository flags. `-inline auto,deferred` retains the same update residual and exact 756-byte assignment; `-O4,s` instead produces 460-byte update and 376-byte assignment bodies. That size mode is rejected for this reconstructed composition unit, not asserted impossible for every game source. The selected flags remain unchanged.
- A reference header API is not evidence of use by this caller. Petari's `VEC2(const _VEC2&)` constructor was tested only in an external shadow header; base-vector accumulation and constant-base-vector initialization produce different, larger update bodies. A by-value `SetStick(VEC2)` experiment also adds later instruction differences and retains the original entry mismatch. Neither type/interface change was retained.
